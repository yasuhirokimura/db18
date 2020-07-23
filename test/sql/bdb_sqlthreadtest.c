/*
** 2002 January 15 + Jan 2018 - BDB Adaptations
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file implements a simple standalone program used to test whether
** or not the SQLite library is threadsafe.
**
** Testing the thread safety of SQLite is difficult because there are very
** few places in the code that are even potentially unsafe, and those
** places execute for very short periods of time.  So even if the library
** is compiled with its mutexes disabled, it is likely to work correctly
** in a multi-threaded program most of the time.  
**
** This file is NOT part of the standard SQLite library.  It is used for
** testing only.
*/
#include "sqlite3.h"
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
extern int getopt(int, char * const *, const char *);
extern int optind;
extern char *optarg;

/* Wrap Windows thread API to make it look POSIXey. */
typedef HANDLE thread_t;
#define thread_create(thrp, attr, func, arg)                                \
    (((*(thrp) = CreateThread(NULL, 0,                                      \
        (LPTHREAD_START_ROUTINE)(func), (arg), 0, NULL)) == NULL) ? -1 : 0) 
#define thread_self()  GetCurrentThreadId()
#define thread_detach(m)    CloseHandle(m)

typedef HANDLE mutex_t;
#define mutex_init(m, attr)                                                 \
    (((*(m) = CreateMutex(NULL, FALSE, NULL)) != NULL) ? 0 : -1)
#define mutex_lock(m)                                                       \
    ((WaitForSingleObject(*(m), INFINITE) == WAIT_OBJECT_0) ? 0 : -1)
#define mutex_unlock(m)       (ReleaseMutex(*(m)) ? 0 : -1)

typedef HANDLE cond_t;
#define cond_signal(m)      SetEvent(*(m))
#define cond_wait(m, n)     SignalObjectAndWait(*(n), *(m), INFINITE, FALSE)

mutex_t Lock;
cond_t Cond;

#define snprintf _snprintf

#else
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

typedef pthread_t thread_t;
#define thread_create(thrp, attr, func, arg)                                   \
    pthread_create((thrp), (attr), (func), (arg))
#define thread_self()         pthread_self()
#define thread_detach(m)      pthread_detach(m)

typedef pthread_mutex_t mutex_t;
#define mutex_init(m, attr)   pthread_mutex_init((m), (attr))
#define mutex_lock(m)         pthread_mutex_lock(m)
#define mutex_unlock(m)       pthread_mutex_unlock(m)

typedef pthread_cond_t cond_t;
#define cond_signal(m)          pthread_cond_signal(m)
#define cond_wait(m, n)             pthread_cond_wait(m, n)

pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Cond = PTHREAD_COND_INITIALIZER;
#endif

			/* Command line options and default values. */
int DbShareFactor = 2;	/* -d	threads sharing each db	*/
int Iterations = 10;	/* -i 	iterations for worker bee loop */
int MaxSeconds = 300;	/* -m 	Exit "Alarm clock" if runs > 5 minutes. */
int Queries = 20;	/* -q	#queries performed in each iteration */
int Threads = 10;	/* -t	total number of threads started */
int Verbose = 0;	/* -v	vebose messages, default is none  */

int Threads_active = 0;

/*
** Come here to die.
*/
static void Exit(int rc){
  exit(rc);
}

extern char *sqlite3_mprintf(const char *zFormat, ...);
extern char *sqlite3_vmprintf(const char *zFormat, va_list);

/*
** When a lock occurs, yield.
*/
static int 
db_is_locked(void *NotUsed, int iCount)
{
	/* sched_yield(); */
	if (Verbose)
		printf("BUSY %s #%d\n", (char *)NotUsed, iCount);
#ifdef _WIN32
	Sleep(100);
#else
	usleep(100);
#endif
	return iCount < 25;
}

/*
** Used to accumulate query results by db_query()
*/
struct QueryResult {
    const char *zFile;  /* Filename - used for error reporting */
    int nElem;          /* Number of used entries in azElem[] */
    int nAlloc;         /* Number of slots allocated for azElem[] */
    char **azElem;      /* The result of the query */
};

/*
** The callback function for db_query
*/
static int 
db_query_callback(
    void *pUser,	/* Pointer to the QueryResult structure */
    int nArg,		/* Number of columns in this result row */
    char **azArg,	/* Text of data in all columns */
    char **notUsed	/* Names of the columns */
    )
{
	struct QueryResult *pResult = (struct QueryResult *)pUser;
	int	i;

	if (pResult->nElem + nArg >= pResult->nAlloc) {
		if (pResult->nAlloc == 0)
			pResult->nAlloc = nArg + 1;
		else
			pResult->nAlloc = pResult->nAlloc * 2 + nArg + 1;
		
		pResult->azElem = realloc(pResult->azElem,
		    pResult->nAlloc * sizeof(char *));
		if (pResult->azElem == NULL) {
			fprintf(stderr, "%s: malloc failed\n", pResult->zFile);
			return 1;
		}
	}
	if (azArg == NULL)
		return 0;
	for (i = 0; i < nArg; i++) {
		pResult->azElem[pResult->nElem++] =
		    sqlite3_mprintf("%s", azArg[i] ? azArg[i] : "");
	}
	notUsed = (char **)NULL;
	return 0;
}

/*
** Execute a query against the database.  NULL values are returned
** as an empty string.  The list is terminated by a single NULL pointer.
*/
char **
db_query(sqlite3 * db, const char *zFile, const char *zFormat,...)
{
	char           *zSql;
	int		rc;
	char           *zErrMsg = 0;
	va_list		ap;
	struct QueryResult sResult;

	va_start(ap, zFormat);
	zSql = sqlite3_vmprintf(zFormat, ap);
	va_end(ap);
	memset(&sResult, 0, sizeof(sResult));
	sResult.zFile = zFile;
	if (Verbose)
		printf("QUERY %s: %s\n", zFile, zSql);
	do {
		rc = sqlite3_exec(db, zSql,
		    db_query_callback, &sResult, &zErrMsg);
	} while (rc == SQLITE_BUSY || rc == SQLITE_SCHEMA);
	if (Verbose)
		printf("DONE %s %s\n", zFile, zSql);
	if (zErrMsg) {
		printf("%s: query failed: %s - %s\n", zFile, zSql, zErrMsg);
		sqlite3_free(zSql);
		Exit(1);
	}
	sqlite3_free(zSql);
	if (sResult.azElem == NULL)
		db_query_callback(&sResult, 0, 0, 0);
	sResult.azElem[sResult.nElem] = 0;
	return sResult.azElem;
}

/*
 * db_execute_ --
 * Execute an SQL statement, exit if checkret && there are any errors.
 */
void 
db_execute(sqlite3 * db, const char *zFile, int checkret, const char *zFormat,...)
{
	char           *zSql;
	int		rc;
	char           *zErrMsg = 0;
	va_list		ap;

	va_start(ap, zFormat);
	zSql = sqlite3_vmprintf(zFormat, ap);
	va_end(ap);
	if (Verbose)
		printf("EXEC %s: %s\n", zFile, zSql);
	do {
		rc = sqlite3_exec(db, zSql, 0, 0, &zErrMsg);
	} while (rc == SQLITE_BUSY || rc == SQLITE_SCHEMA);
	if (Verbose)
		printf("DONE %s: %s\n", zFile, zSql);
	if (zErrMsg) {
		fprintf(stderr, "%s: command %s: %s - %s\n", zFile,
		    checkret ? "failed" : "returned an acceptable warning",
		    zSql, zErrMsg);
		sqlite3_free(zSql);
		if (checkret)
			Exit(1);
	} else
		sqlite3_free(zSql);
}

/*
** Free the results of a db_query() call.
*/
void 
db_query_free(char **az)
{
	int	i;

	for (i = 0; az[i]; i++)
		sqlite3_free(az[i]);
	free(az);
}

/*
** Check results
*/
void 
db_check(const char *zFile, const char *zMsg, char **az,...)
{
	va_list		ap;
	int		i;
	char           *z;

	va_start(ap, az);
	for (i = 0; (z = va_arg(ap, char *)) != 0; i++) {
		if (az[i] != NULL && strcmp(az[i], z) == 0)
			continue;
		printf("%s: %s: bad result in column %d: got %s, expected %s\n",
			zFile, zMsg, i + 1, az[i], z);
		db_query_free(az);
		Exit(1);
	}
	va_end(ap);
	db_query_free(az);
}

/*
 * worker_bee -
 *	Repeatedly create a sql table, run some queries, then drop it.
 *
 *	The argument (from sqlite) is a string: <thread#>.<table name>. 
 */
static void    *
worker_bee(void *pArg)
{
	sqlite3	*db;
	char	*zFilename = (char *)pArg;
	int	i, cnt;
	int	t = atoi(zFilename);
	char	**az, *s;
	char	z1[30], z2[30];

	mutex_lock(&Lock);
	/* Skip over the thread number to get the table name. */
	s = strchr(zFilename, '.');
	assert(s != NULL);
	sqlite3_open(s + 1, &db);
	if (Verbose)
		system("ls -lR testdb-*");
	mutex_unlock(&Lock);
	if (db == NULL) {
		printf("%s: can't open\n", zFilename);
		Exit(1);
	}
	sqlite3_busy_handler(db, db_is_locked, zFilename);
	printf("Thread %lx %s: START\n", (long)thread_self(), zFilename);
	fflush(stdout);

	/* Each iterations does
	 *	create table
	 *	2 * Queries inserts,
	 *	select count(*)
	 *	select avg(b)
	 *	delete half of the records
	 *	select avg(b)
	 * 	1 * Queries selects
	 *	drop table
	 * for 6 + 3 * Queries steps.
	 */
	for (cnt = 0; cnt < Iterations; cnt++) {
		db_execute(db, zFilename, 0, "CREATE TABLE t%d(a,b,c);", t);
		/*
		 * Insert two records for each query to be performed later.
		 *	(i, i * 2, i squared)
		 */
		for (i = 1; i <= 2 * Queries; i++) {
			db_execute(db, zFilename, 1,
			    "INSERT INTO t%d VALUES(%d,%d,%d);",
			    t, i, i * 2, i * i);
		}

		/* The count should be the number of records. */
		snprintf(z1, sizeof(z1), "%d", 2 * Queries);
		az = db_query(db, zFilename, "SELECT count(*) FROM t%d", t);
		db_check(zFilename, "tX count(*)", az, z1, 0);

		/*
		 * The avg should be one more than the number of inserted
		 * records (as a float), or null.
		 */
		if (Queries == 0)
			z1[0] = '\0';
		else
			snprintf(z1, sizeof(z1), "%d.0", 1 + 2 * Queries);
		az = db_query(db, zFilename, "SELECT avg(b) FROM t%d", t);
		db_check(zFilename, "tX avg", az, z1, 0);

		/*
		 * After deleting half of the records, expect the avg to be one
		 * more than the number of inserted records, as a float.
		 */
		db_execute(db, zFilename, 1,
			"DELETE FROM t%d WHERE a>%d", t, Queries);
		if (Queries == 0)
			z1[0] = '\0';
		else
			snprintf(z1, sizeof(z1), "%d.0", 1 + Queries);
		az = db_query(db, zFilename, "SELECT avg(b) FROM t%d", t);
		db_check(zFilename, "tX avg#2", az, z1, 0);
		for (i = 1; i <= Queries; i++) {
			az = db_query(db, zFilename,
			    "SELECT b, c FROM t%d WHERE a=%d", t, i);
			sprintf(z1, "%d", i * 2);
			sprintf(z2, "%d", i * i);
			db_check(zFilename, "readback", az, z1, z2, 0);
		}

		db_execute(db, zFilename, 0, "DROP TABLE t%d;", t);
	}
	if (Verbose) {
		printf("%lx %s: END iterations, about to sqlite3_close()\n",
		    (long)thread_self(), zFilename);
		fflush(stdout);
	}
	mutex_lock(&Lock);
	sqlite3_close(db);
	Threads_active--;
	if (Threads_active <= 0)
		cond_signal(&Cond);
	mutex_unlock(&Lock);
	printf("Thread %lx: success %s\n", (long)thread_self(), zFilename);
	return 0;
}

void timedout(int unused_sig)
{
	fprintf(stderr,
	    "bdb_sqlthreadtest will dump core after %d seconds; has it hung?\n",
	    MaxSeconds);
	unused_sig = 0;
	abort();
}

/*
 * usage --
 *	Print out messages for invalid command line options.
 */
void usage(const char *extra_message, ...)
{
	va_list ap;

	fprintf(stderr, "usage: bdb_sqlthreadtest "
		"[-d#<threads sharing a single db>] "
		"[-i#<iterations>] "
		"[-m#<maximum seconds to run> "
		"[-q#<queries/iteration> "
		"[-t#<threads>] "
		"[-v <turn on verbose messages>]\n");
	if (extra_message != NULL) {
		va_start(ap, extra_message);
		vfprintf(stderr, extra_message, ap);
		va_end(ap);
		fprintf(stderr, "\n");
	}
	exit(1);
}

int 
main(int argc, char **argv)
{
	char           *zFile;
	int		i;
	thread_t	id;
	

#ifdef _WIN32
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#else
	setlinebuf(stdout);
	setlinebuf(stderr);
#endif
	while ((i = getopt(argc, argv, "d:i:m:q:t:v")) != EOF) {
		switch (i) {
		case 'd':
			if ((DbShareFactor = atoi(optarg)) < 1)
				usage("-d %d: #threads/db must be positive",
				    DbShareFactor);
			break;
		case 'i':
			if ((Iterations = atoi(optarg)) < 1)
				usage("-i %d: #iterations must be positive",
				    Iterations);
			break;
		case 'm':
			if ((MaxSeconds = atoi(optarg)) < 1)
				usage("-m %d: #maxruntime must be positive",
				    MaxSeconds);
			break;
		case 'q':
			if ((Queries = atoi(optarg)) < 0)
				usage("-q %d: #queries cannot be negative",
				    Queries);
			break;
		case 't':
			if ((Threads = atoi(optarg)) < 1)
				usage("-t %d: #threads must be positive",
				    Threads);
			break;
		case 'v':
			Verbose = 1;
			break;
		default:
			usage(NULL);
			/* NOTREACHED */
			break;
		}
	}
	if (optind != argc)
		usage("Unrecognized argument: %s", argv[optind]);
	printf("%s -d%d -i%d -m%d -q%d -t%d%s\n",
	    argv[0], DbShareFactor, Iterations,
	    MaxSeconds, Queries, Threads, Verbose ? " -v" : "");
	/* Initialize Mutex and Cond Handle for Windows. */
#ifdef _WIN32
	Lock = CreateMutex(NULL, FALSE, NULL);
	Cond = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
    /* SIGALRM is not available on Windows */
#ifndef _WIN32
	signal(SIGALRM, timedout);
	alarm(MaxSeconds);
#endif
	for (i = 0; i < Threads; i++) {
		zFile = sqlite3_mprintf("%d.testdb-%d",
		    i % DbShareFactor + 1, (i + DbShareFactor) / DbShareFactor);
		if ((i % DbShareFactor) == 0) {
			/*
			 * Remove both the database file and any leftover
			 * journal (environment) directory.
			 */
			char *zDb = &zFile[2];
			char *rmJournal;

			if (unlink(zDb) < 0 && errno != ENOENT)
				fprintf(stderr, "%s: unlink %s: %s\n",
				    argv[0], zDb, strerror(errno));
#ifdef _WIN32
			/* 
			 * Use Windows-specific command line here,
			 * to avoid delete error on Windows.
			 */
			rmJournal = sqlite3_mprintf("IF NOT EXIST %s-journal\\ (exit -1)", zDb);
			if (system(rmJournal) == 0) {
				rmJournal = sqlite3_mprintf("rd /s /q %s-journal\\", zDb);
				if (system(rmJournal) != 0) {
					fprintf(stderr, "%s: %s: %s\n",
						argv[0], rmJournal, strerror(errno));
					exit(1);
				}
			}
#else
			rmJournal = sqlite3_mprintf("rm -rf %s-journal/", zDb);
			if (system(rmJournal) != 0) {
				fprintf(stderr, "%s: %s: %s\n",
					argv[0], rmJournal, strerror(errno));
				exit(1);
			}
#endif	
			sqlite3_free(rmJournal);
		}
		mutex_lock(&Lock);
		Threads_active++;
		mutex_unlock(&Lock);
		thread_create(&id, 0, worker_bee, (void *)zFile);
		thread_detach(id);
	}
	mutex_lock(&Lock);
	while (Threads_active > 0) {
		if ((i = cond_wait(&Cond, &Lock)) != 0)
			fprintf(stderr,
			    "thread_cond_wait with %d threads left: %s\n",
			    Threads_active, strerror(i));
	}
	mutex_unlock(&Lock);
	for (i = 0; i < Threads; i++) {
		char	zBuf[200];
		sprintf(zBuf, "testdb-%d", (i + 1) / DbShareFactor);
		unlink(zBuf);
	}
	/* The calculation below matches the sql commands in worker_bee(). */
	printf("bdb_sqlthreadtest completed with 0 errors out of %d steps\n",
	    Threads * Iterations * (3 * Queries + 6));
	return 0;
}
