/*-
 * Copyright (c) 1997, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 * $Id$
 */

#include <sys/types.h>

#include <errno.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <db_cxx.h>

#include <db.h>
#include <stdint.h>
#include "../sql/c/ex_sql_utils.h"

using std::ostream;
using std::cout;
using std::cerr;

void db_setup(const char *, const char *, ostream&);
void db_teardown(const char *, const char *, ostream&);
static int usage();

const char *progname = "EnvLoadSqlExample";			/* Program name. */

//
// An example of a program creating/configuring a Berkeley DB environment.
//

static int
ex_sql_load(db_handle *db)
{
	/* Create table university. */
	echo_info("STEP1. Create Table university");
	/* Load data from table data source. */
	load_table_from_file(db, university_sample_data, 0); 

	return 0;
}


int
main(int argc, char *argv[])
{
	//
	// Note: it may be easiest to put all Berkeley DB operations in a
	// try block, as seen here.  Alternatively, you can change the
	// ErrorModel in the DbEnv so that exceptions are never thrown
	// and check error returns from all methods.
	//
	try {
		const char *data_dir, *home;
		
		
		//
		// All of the shared database files live in home,
		// but data files live in data_dir.
		//
		home = "TESTDIR";
		data_dir = "data";

		for (int argnum = 1; argnum < argc; ++argnum) {
			if (strcmp(argv[argnum], "-h") == 0) {
				if (++argnum >= argc)
					return (usage());
				home = argv[argnum];
			}
			else if (strcmp(argv[argnum], "-d") == 0) {
				if (++argnum >= argc)
					return (usage());
				data_dir = argv[argnum];
			}
			else {
				return (usage());
			}
		}

		cout << "Setup env\n";
		db_setup(home, data_dir, cerr);

		cout << "Teardown env\n";
		db_teardown(home, data_dir, cerr);

		db_handle *db;

		/* Setup environment */
		db = setup("./excxx_env_sql.db");

		/* Run example. */
		ex_sql_load(db);

		/* End. */
		cleanup(db);

		return (EXIT_SUCCESS);
	}
	catch (DbException &dbe) {
		cerr << "EnvLoadSqlExample: " << dbe.what() << "\n";
		return (EXIT_FAILURE);
	}

	
}

// Note that any of the db calls can throw DbException
void
db_setup(const char *home, const char *data_dir, ostream& err_stream)
{
	const char * err1 = "DbEnv::open: No such file or directory";
	const char * err2 = "Db::open: No such file or directory";
	//
	// Create an environment object and initialize it for error
	// reporting.
	//
	DbEnv *dbenv = new DbEnv((u_int32_t)0);
	dbenv->set_error_stream(&err_stream);
	dbenv->set_errpfx(progname);

	//
	// We want to specify the shared memory buffer pool cachesize,
	// but everything else is the default.
	//
	dbenv->set_cachesize(0, 64 * 1024, 0);

	// Databases are in a subdirectory.
	(void)dbenv->set_data_dir(data_dir);

	// Open the environment with full transactional support.
	try {
		dbenv->open(home,
		    DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | 
		    DB_INIT_TXN, 0);
	}
	catch (DbException &dbe) {
		cerr << "EnvLoadSqlExample: " << dbe.what() << "\n";
		if (!strcmp(dbe.what(), err1)){
			cout << "Please check whether "
			    << "home dir \"" << home << "\" exists.\n";
		}
		exit (-1);
	}

	// Open a database in the environment to verify the data_dir
	// has been set correctly.
	// Create a database handle, using the environment.	
	Db *db = new Db(dbenv, 0) ;

	// Open the database. 
	try {
		db->open(NULL, "EvnExample_db1.db", 
		    NULL, DB_BTREE, DB_CREATE, 0644);
	}
	catch (DbException &dbe) {
		cerr << "EnvLoadSqlExample: " << dbe.what() << "\n";
		if (!strcmp(dbe.what(), err2)){
			cout << "Please check whether data dir \"" << data_dir
			    << "\" exists under \"" << home << "\"\n";
		}
		exit (-1);
	}

	// Close the database handle.
	db->close(0) ;
	delete db;

	// Close the handle.
	dbenv->close(0);
	delete dbenv;
}

void
db_teardown(const char *home, const char *data_dir, ostream& err_stream)
{
	// Remove the shared database regions.
	DbEnv *dbenv = new DbEnv((u_int32_t)0);

	dbenv->set_error_stream(&err_stream);
	dbenv->set_errpfx(progname);

	(void)dbenv->set_data_dir(data_dir);
	dbenv->remove(home, 0);
	delete dbenv;
}

static int
usage()
{
	cerr << "usage: excxx_env_sql [-h home] [-d data_dir]\n";
	return (EXIT_FAILURE);
}


/* Setup database environment. */
db_handle *
setup(const char* db_name)
{
	db_handle *db;
	/* Open database. */
	sqlite3_open(db_name, &db);
	error_handler(db);

	return db;
}

/* Clean up the database environment. */
void
cleanup(db_handle *db)
{
	int rc;
	/* Close database and end program. */
	rc = sqlite3_close(db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "DB CLOSE ERROR. ERRCODE: %d.\n", rc);
		exit(EXIT_FAILURE);
	}
	printf("DONE.\n");
}

/*
 * Output Control Functions.
 */
/* Output message with line between. */
void
echo_info(const char* info)
{
	int i;
	char ch = '=';

	for (i = 0; i < 80; i++)
		printf("%c", ch);
	
	printf("\n%s\n", info);
}

/*
 * Output columns for following exec_sql(). We've to use ANSI C declaration 
 * here to eliminate warnings in Visual Studio.
 */
static int
print_column_callback(void *data, int n_columns,
		      char **col_values, char **col_names)
{
	int i;

	printf("  "); /* Display indent. */
	for (i = 0; i < n_columns; i++) {
		printf("%s\t", 
		    col_values[i] == NULL ? "" : col_values[i]);
	}
	printf("\n");

	return 0;
}

/*
 * Execute a given sql expression and print result automatically. This
 * function will print error and exit if any error occurs.
 *
 * This function always return sqlite result code.
 */
static int
exec_sql_internal(db_handle* db, const char* sql, int silent)
{
	int rc; /* Result code. */

	if (!silent)
		printf("SQL: %s\n", sql);
	/*
	 * Execute a sql expression. The result will be printed by
	 * the callback function.
	 *
	 * The 5th argument of sqlite3_exec() is errmsg buffer. In out case, 
	 * the program does not use it and use sqlite3_errmsg() to get the most 
	 * recent error message in error_handler(). The advantage is that we 
	 * do not need to manage those errmsg buffers by hand. But if we share 
	 * a connection object(sqlite3*) in multi-threads, the error message 
	 * of one thread may be overwritten by other threads. However, sharing 
	 * a connection object between multi-threads is not a recommended 
	 * method in sqlite.
	 */
	if (!silent)
		rc = sqlite3_exec(db, sql, print_column_callback, 0, NULL);
	else
		rc = sqlite3_exec(db, sql, NULL, 0, NULL);

	error_handler(db);

	return rc;
}

int
exec_sql(db_handle* db, const char* sql)
{
	return exec_sql_internal(db, sql, 0);
}

/*
 * This is the default error handler for all examples. It always return 
 * the recent sqlite result code.
 *
 * You have to use sqlite3_extended_result_codes() instead of sqlite3_errcode() 
 * when extended result codes are enabled. These examples does not use extended 
 * code.
 */
int
error_handler(db_handle *db)
{
	int err_code = sqlite3_errcode(db);

	switch(err_code) {
	case SQLITE_OK:
	case SQLITE_DONE:
	case SQLITE_ROW:
		/* Do nothing. */
		break;
	default:
		fprintf(stderr, "ERROR: %s. ERRCODE: %d.\n",
			sqlite3_errmsg(db), err_code);
		exit(err_code);
	}
	return err_code;
}



/*
 * Pre-load database, create a table and insert rows by given a csv file.
 */
#define BUF_SIZE		1024
#define TABLE_COLS		16
#define SQL_COMMAND_SIZE	4096

/*
 * Here is the definition of the university sample table. As we can see from 
 * below, the university table has 9 columns with types including int and 
 * varchar.
 */
const sample_data university_sample_data = {
	"university",
	"\tDROP TABLE IF EXISTS university;\n"
	"\tCREATE TABLE university\n"
	"\t(\n"
		"\t\trank       int,\n"
		"\t\tname       varchar(75),\n"
		"\t\tdomains    varchar(75),\n"
		"\t\tcountry    varchar(30),\n"
		"\t\tregion     varchar(30),\n"
		"\t\tsize       int,\n"
		"\t\tvisibility int,\n"
		"\t\trich       int,\n"
		"\t\tscholar    int\n"
	 "\t);",
	"../examples/sql/data/university.csv", 9};

static char items[TABLE_COLS][BUF_SIZE];

/* Open the sample csv file handle. */
static FILE*
open_csv_file(const char* data_source)
{
	FILE* fp;

	fp = fopen(data_source, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s open error.", data_source);
		exit(EXIT_FAILURE);
	}

	return fp;
}

/* Get data line-by-line and insert it into the database. */
static int
iterate_csv_file(FILE* fp, int n_cols)
{
	static char file_line[BUF_SIZE * TABLE_COLS];
	const char delims[] = ",";
	char *result;
	int i;

	/* Skip header row. */
	if (ftell(fp) == 0) {
		fgets(file_line, sizeof(file_line), fp);
	}

	/* Get one line. */
	if (fgets(file_line, sizeof(file_line), fp) == NULL) {
		fclose(fp);
		return 0;
	}

	/* Token sentence by delimiters "," */
	result = strtok(file_line, delims);
	for (i = 0; result != NULL && i < n_cols; i++) {
		strcpy(items[i], result);
		result = strtok(NULL, delims);
	}

	return 1;
}

/* Common utility: load given csv file into database. */
void
load_table_from_file(sqlite3* db, sample_data data, int silent)
{
	FILE *fp;
	int i, n;
	char buf[SQL_COMMAND_SIZE];

	sprintf(buf, "Load data source %s into database.",
		data.source_file);
	echo_info(buf);
	/* Create table by given SQL expression. */
	exec_sql_internal(db, data.sql_create, silent);

	fp = open_csv_file(data.source_file);
	while (iterate_csv_file(fp, data.ncolumn) != 0) {
		/* Get data line by line and put it into database. */
		i = sprintf(buf, "INSERT INTO %s VALUES(", data.table_name);
		if (data.ncolumn > 0 ) {
			for (n = 0; n < data.ncolumn; n++) {
				i += sprintf(buf + i, "'%s',", items[n]);
			}
			/* -3 to delete last "\n'," */
			sprintf(buf + i - 3, "');");
			exec_sql_internal(db, buf, silent);
		}
	}
	printf("Load done.\n");
}
