/*
 * Copyright (c) 2016, 2020 Oracle and/or its affiliates.  All rights reserved.
 * 
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

/* 
 * Head file for MacDonald Self-Service Ordering System.
 */

#include <sys/types.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iomanip>
#include <db_cxx.h>

#ifdef _WIN32
#define CLEANUP_CMD "rmdir envhome /q/s"
#define COPY_CMD \
    "XCOPY ..\\..\\examples\\cxx\\excxx_order\\product.txt envhome"
#define PRODUCTFILE "envhome\\product.txt"
#else
#define CLEANUP_CMD "rm -rf envhome"
#define COPY_CMD "cp ../examples/cxx/excxx_order/product.txt ./envhome"
#define PRODUCTFILE "envhome/product.txt"
#endif

#define MAKEENV_CMD "mkdir envhome"

#define DEFAULT_HOMEDIR "envhome"             /* Environment home directory. */
#define PRODUCTDB "productDB.db"                   /* Product database name. */
#define ORDERDB "orderDB.db"                         /* Order database name. */
#define MAXLEN 20                                   /* The length of string. */
/*
 * BUFFERLENGTH is used in bulk operation.
 * The buffer must be at least as large as the page size of
 * the underlying database,aligned for unsigned integer access,
 * and be a mutiple of 1024 bytes in size.
 */
#define BUFFERLENGTH (16 * 1024)
#define progname "MacDonald Self-service Ordering System"

using namespace std;

/*
 * Product data structure.
 */
struct product {
public:
	int product_no;                     /* The ID number of the product. */
	char product_name[MAXLEN];               /* The name of the product. */

	int product_price;                      /* The price of the product. */
	char product_type[MAXLEN];               /* The type of the product. */
};

/*
 * Order data structure.
 */
struct order {
public:
	int product_no;                     /* The ID number of the product. */
	int product_num;                       /* The number of the product. */
};

/*
 * Customer data structure.
 */
class Customer 
{
public:
	void customer_run(Db *productdbp, Db *orderdbp, Db *productsdbp);

	void operate_product(Db *productdbp, Db *orderdbp);
	void order_product(Db *productdbp, Db *orderdbp);
	void delete_order(Db *orderdbp);
	void insert_into_order(product pro, Db *orderdbp, int num);
	int delete_from_order(Db *orderdbp, int productno);

	int search_product_no(int productno, Db *productdbp, product &pro);

	int display_order_information(product pro, order ord);
	int display_product_information(product pro);

	void display_product_database(Db *productdbp);
	void display_order_database(Db *orderdbp, Db *productdbp);

	void display_product_based_on_type(Db *productsdbp);
	void display_sdbp(string type, Db *productsdbp);
};

/*
 * Administrator data structure.
 */
class Administrator
{
public:
	Administrator();
	int admin_login();
	int admin_run();

	void init_DbEnv();
	void init_DbEnv_Recover();
	void open_productdb();
	void open_orderdb();

	void bulk_read_product();
	void show_product();
	void print(product pro);
	void read_product(string filename);
	void bulk_update_product(string filename);
	void delete_product();
	product search_product_no(int productno, Db *productdb);

	Db * get_productdb();
	Db * get_orderdb();
	Db * get_productsdb();

	void close_db();
	void throwException(DbEnv *dbenvp, int ret, const char *msg);
private:
	DbEnv *dbenv;                                 /* Environment handle. */
	Db *product_dbp;         /* Database containing product information. */
	Db *product_type_sdbp;  /* Secondary database based on product type. */
	Db *order_dbp;             /* Database containing order information. */
};

/* 
 * Function prototypes.
 */
int get_product_type(Db *sdbp, const Dbt *pkey, const Dbt *pdata, Dbt *skey);
