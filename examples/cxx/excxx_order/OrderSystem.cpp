/*
 * Copyright (c) 2016, 2019 Oracle and/or its affiliates.  All rights reserved.
 * 
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

/* 
 * MacDonald Self-Service Ordering System
 *
 * This application provides the communication interface via command line.
 * There are mainly two role entrances for the system: administrator and
 * customer.
 * 
 * Administrators manage the environment initialization and the operation 
 * on product database. Operation on the product database incudes inserting 
 * product into the product database by reading directly or by bulk operation.
 *	
 * Customers manage the operation on order database and display the product
 * database. Operation on the order database includes inserting the product
 * into the order database, displaying order database and product database.
 */

#include "OrderSystem.h"

/* 
 * get_product_type -- 
 * 	extracts a secondary key from a primary key/data pair.
 */
int get_product_type(Db *sdbp, const Dbt *pkey, const Dbt *pdata, Dbt *skey)
{  
	/* Initialize Dbt. */
	memset(skey, 0, sizeof(Dbt));

	skey->set_data(((product *)(pdata->get_data()))->product_type);
	skey->set_size(sizeof(((product *)(pdata->get_data()))->product_type));

	return (0);
}

/*
 * Adminisrator implementation module.
 */
Administrator::Administrator() 
    :dbenv(NULL),
    product_dbp(NULL),
    order_dbp(NULL),
    product_type_sdbp(NULL) 
{
	/*
 	 * No parameter constructor function initializes the pointer 
 	 * variables of the environment. Including database environment,
 	 * product database, secondary product type database
 	 * and order database.
 	 */
}
/* 
 * Administrator login function.
 */
int Administrator::admin_login()
{
	int adminflag = 1;
	string empno, password;

	cout << "================================================" << endl;
	cout << "Administrator Login" << endl;
	cout << "================================================" << endl;

	while (adminflag == 1) {
		cout << "Input ID number:";
		cin >> empno;

		cout << "Input password:";
		cin >> password;

		/*
		 * Match admin number and password and judge 
		 * whether the user is administrator.
		 */
		if ((empno == "admin") && (password == "admin")) {
			cout << "login successfully." << endl;
			/* Reset the loop condition. */
			adminflag = 0;
		} else 
			cout << "login failed." << endl;
	}

	return (1);
}

/*
 * Administrator run system.
 */ 
int Administrator::admin_run()
{
	string productfile = PRODUCTFILE;

	cout << "================================================" << endl;
	cout << "Initialize the environment" << endl;
	cout << "-r   Initialize the Env with recover." << endl;
	cout << "-n   Initialize the Env without recover." << endl;
	cout << "================================================" << endl;

	int econdition = 1;
	while (econdition == 1) {
		char eflag;
		cin >> eflag;

		switch (eflag) {
		case 'r':
			/* Initialize the environment with recover flag. */
			this->init_DbEnv_Recover();
			/* Reset the loop condition. */
			econdition = 0;
			break;
		case 'n':
			/* Initialize the environment without recover flag. */
			this->init_DbEnv();
			/* Reset the loop condition. */
			econdition = 0;
			break;
		default:
			cout << "Wrong input, please input again:" << endl;
			break;
		}
	}

	/*
	 * After the environment initialization is finished, 
	 * we need to initialize all related databases.
	 */

	open_productdb();	/* Initialize the product database. */
	open_orderdb();		  /* Initialize the order database. */
	

	/*
	 * After we initialize product and order database, we need to 
	 * load product data into the product database. We provide two 
	 * methods for administrator to load product data into database. 
	 * One is by bulk operation and the other is by normal operation.
	 */

	cout << "=================================================" << endl;
	cout << "Load the file into database." << endl;
	cout << "-b   Bulk Read product into database." << endl;
	cout << "-r   Read product into database." << endl;
	cout << "=================================================" << endl;
	cout << "Please choose the command:" << endl;

	int lcondition = 1;
	while (lcondition == 1) {
		char lflag;
		cin >> lflag;

		switch (lflag) {
		case 'b':
			/* Bulk update the product database. */
			this->bulk_update_product(productfile);
			/* Reset the loop condition. */
			lcondition = 0;
			break;
		case 'r':
			/* Read file into the product database. */
			this->read_product(productfile);
			/* Reset the loop condition. */
			lcondition = 0;
			break;
		default:
			cout << "Wrong input, please input again:" << endl;
			break;
		}
	}
	
	cout << "Initialize the environment successfully!" << endl;
	cout << "==================================================" << endl;
	cout << endl;
	cout << "Welcome to MacDonald Self-Service Ordering System" << endl;
	cout << endl;

	/* 
	 * The system provides two methods for administrator to
	 * dispaly product database. One is by bulk operation and 
	 * the other is by normal operation. 
	 */ 
	int ccondition = 1;
	while (ccondition == 1) {
		cout << "=================================================";
		cout << endl;
		cout << "Command Instructions:" << endl;
		cout << "-b    Bulk display product database." << endl;
		cout << "-l    Loop display product database." << endl;
		cout << "-s    Start system." << endl;
		cout << "Please choose the command:" << endl;
		char cflag;
		cin >> cflag;
		cout << "=================================================";
		cout << endl;

		switch (cflag) {
		case 'b':
			/* Bulk read the product database. */
			this->bulk_read_product();
			break;
		case 's':
			/* Reset the loop condition. */
			ccondition = 0;
			break;
		case 'l':
			/* Loop display the product list. */
			this->show_product();
			break;
		default:
			/* Start the system. */
			cout << "Wrong input." << endl;
			break;
		}
	}
	return (0);
}

/* 
 * Initialize the environment without recover flag.
 */
void Administrator::init_DbEnv() 
{
	int ret = 0;

	/* Initialize the new environment. */
	this->dbenv = new DbEnv((u_int32_t)0);
	/* Set up error handling. */
	this->dbenv->set_error_stream(&cerr);
	this->dbenv->set_errpfx(progname);

	try {
		/* Set up environment cachesize. */
		if ((ret = this->dbenv->set_cachesize(0, 64 * 1024, 0)) != 0)
			throwException(
			    this->dbenv, ret, "DB_ENV->set_cachesize");

		/* Open the environemnt with full transactional support. */
		if ((ret = this->dbenv->open(
		    DEFAULT_HOMEDIR, DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | 
		    DB_INIT_MPOOL | DB_INIT_TXN, 0)) != 0)
			throwException(this->dbenv, ret, "DB_ENV->open");
	} catch (DbException & dbe) {
		cerr << "initDbenv" << dbe.what() << endl;
		throw dbe;
	}
}

/*
 * Initialize the environment with recover flag.
 */
void Administrator::init_DbEnv_Recover()
{
	int ret = 0;

	/* Initialize the new environment. */
	this->dbenv = new DbEnv((u_int32_t)0);
	/* Set up error handling. */
	this->dbenv->set_error_stream(&cerr);
	this->dbenv->set_errpfx(progname);

	try {
		/* Set up environment cachesize. */
		if ((ret = this->dbenv->set_cachesize(0, 64 * 1024, 0)) != 0)
			throwException(
			    this->dbenv, ret, "DB_ENV->set_cachesize");

		/* Open the environemnt with full transactional support. */
		if ((ret = this->dbenv->open(
		    DEFAULT_HOMEDIR, DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | 
		    DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER, 0)) != 0)
			throwException(this->dbenv, ret, "DB_ENV->open");
	} catch (DbException &dbe) {
		cerr << "initDbenv" << dbe.what() << endl;
		throw dbe;
	}
}

/*
 * Initialize the product database.
 */
void Administrator::open_productdb()
{
	int ret;
	ret = 0;

	/* Initialize product Db handle. */
	this->product_dbp = new Db(this->dbenv, 0);
	/* Set up error handling for product database. */
	this->product_dbp->set_error_stream(&cerr);
	this->product_dbp->set_errpfx(progname);

	try {
		/* Set up database pagesize. */
		if ((ret = this->product_dbp->set_pagesize(1024)) != 0)
			throwException(
			    this->dbenv, ret, "ProductDB->set_pagesize");

		/* Open the database using btree access method. */
		if ((ret = this->product_dbp->open(NULL, 
		    PRODUCTDB, "primary", DB_BTREE, DB_CREATE, 0644)) != 0)
			throwException(this->dbenv, ret, "ProductDB->open");

		/* Initialize the secondary database. */
		this->product_type_sdbp = new Db(this->dbenv, 0);

		/* Set up secondary database flags. */
		if ((ret = 
		    this->product_type_sdbp->set_flags(
		    DB_DUP | DB_DUPSORT)) != 0)
			throwException(
			    this->dbenv, ret, "ProductDB->set_flags");

		/* Open the secondary database using btree access method. */
		if ((ret = this->product_type_sdbp->open(NULL, 
		    PRODUCTDB, "secondary", DB_BTREE, DB_CREATE, 0644)) != 0)
			throwException(
			    this->dbenv, ret, "ProductSecondaryDB->open");

		/* 
		 * Associate the secondary database with 
		 * the primary database. 
		 */
		if ((ret = this->product_dbp->associate(NULL, 
		    this->product_type_sdbp, get_product_type, 0)) != 0)
			throwException(
			    this->dbenv, ret, "ProductSecondaryDB->associate");
	} catch (DbException &dbe) {
		cerr << "initProductDb" << dbe.what() << endl;
	}
}

/*
 * Initialize the order database.
 */ 
void Administrator::open_orderdb()
{
	int ret = 0;

	/* Initialize order Db handle. */
	this->order_dbp = new Db(this->dbenv, 0);
	/* Set up error handling for order database. */
	this->order_dbp->set_error_stream(&cerr);
	this->order_dbp->set_errpfx(progname);

	try {
		/* Set up database pagesize. */
		if ((ret = this->order_dbp->set_pagesize(1024)) != 0)
			throwException(
			    this->dbenv, ret, "OrderDB->set_pagesize");

		/* Open the database using btree access method. */
		if ((ret = this->order_dbp->open(NULL, 
		    ORDERDB, "primary", DB_BTREE, DB_CREATE, 0644)) != 0)
			throwException(this->dbenv, ret, "OrderDB->open");
	} catch (DbException &dbe) {
		cerr << "initOrderDb" << dbe.what() << endl;
	}
}

/* 
 * Bulk update product database.
 */ 
void Administrator::bulk_update_product(string filename)
{
	/* Initialize Dbts. */
	Dbt key, data;
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	DbMultipleKeyDataBuilder *ptrkd;

	/* Initialize data object. */
	product *pro;
	pro = (product *)malloc(sizeof(product));
	memset(pro, 0, sizeof(product));

	void *kbuf;
	u_int32_t klen;
	klen = BUFFERLENGTH;
	kbuf = malloc(klen);

	memset(kbuf, 0, klen);
	key.set_data(kbuf);
	key.set_ulen(klen);
	key.set_flags(DB_DBT_USERMEM | DB_DBT_BULK);

	char productprice[MAXLEN];
	char producttype[MAXLEN];
	char productno[MAXLEN];

	int ret, count, flag;
	ret = 0;
	count = 0;
	flag = 1;

	/* Open the product file. */
	ifstream inFile(filename.c_str(), ios::in);
	if (!inFile) {
		cerr << "Could not open file: " 
		    << filename << ". Giving up." << endl;
	}

	while (flag == 1) {
		/* Initialize the count variable. */
		count = 0;
		/* Initialize the key/data builder. */
		ptrkd = new DbMultipleKeyDataBuilder(key);

		/*
		 * Bulk update the product database every five entries.
		 */
		while (!inFile.eof() && count <= 5) {
			string stringBuf;

			/* Get the current line of data. */
			getline(inFile, stringBuf);
			if (stringBuf.length() <= 0)
				break;

			count++;

			/* Reassign the memory of object in each loop. */
			memset(pro, 0, sizeof(product));
			memset(productprice, 0, sizeof(int));
			memset(productno, 0, sizeof(int));
			memset(producttype, 0, sizeof(int));

			/* Scan the line into the product related variables. */
			sscanf(stringBuf.c_str(),
			    "%20[^&]&%20[^&]&%20[^&]&%20[^\n]", productno, 
			    pro->product_name, productprice, producttype);

			pro->product_no = atoi(productno);
			pro->product_price = atoi(productprice);
			strcpy(pro->product_type, producttype);

			ptrkd->append(&pro->product_no, 
			    sizeof(int), pro, sizeof(product));
		}

		if (ptrkd != NULL) {
			delete ptrkd;
			ptrkd = NULL;
		}

		/* Put the data into the  product database. */
		if ((ret = this->product_dbp->put(
		    NULL, &key, &data, DB_MULTIPLE_KEY)) != 0)
			throwException(dbenv, ret, "DB->get");

		/* Check whether we reach the end of file. */
		if (inFile.eof() == true)
			flag = 0;
		else
			flag = 1;
	}
	/* Close the product file. */
	inFile.close();

	if (pro != NULL) {
		delete pro;
		pro = NULL;
	}
	if (kbuf != NULL) {
		kbuf = NULL;
	}
}

/* 
 * Fill product into product database.
 */
void Administrator::read_product(string filename)
{
	char productprice[MAXLEN];
	char producttype[MAXLEN];
	char productno[MAXLEN];

	product pro;
	int ret;

	ret = 0;

	/* Open the product file. */
	ifstream inFile(filename.c_str(), ios::in);
	if (!inFile) {
		cerr << "Could not open file: " 
		    << filename << ". Giving up." << endl;
	}

	while (!inFile.eof()) {
		string stringBuf;

		/* Get the current line of data. */
		getline(inFile, stringBuf);

		if (stringBuf.length() <= 0)
			break;

		/* Reassign the memory of object in each loop. */
		memset(&pro, 0, sizeof(product));
		memset(productprice, 0, sizeof(productprice));
		memset(productno, 0, sizeof(productno));
		memset(producttype, 0, sizeof(producttype));

		/* Scan the line into the structure. */
		sscanf(stringBuf.c_str(), 
		    "%20[^&]&%20[^&]&%20[^&]&%20[^\n]", productno, 
		    pro.product_name, productprice, producttype);

		pro.product_no = atoi(productno);
		pro.product_price = atoi(productprice);
		strcpy(pro.product_type, producttype);

		/* Initialize the Dbts. */
		Dbt key(&pro.product_no, sizeof(int));
		Dbt data(&pro, sizeof(product));

		/* Put the data into product database. */
		if ((ret = this->product_dbp->put(
		    NULL, &key, &data, DB_NOOVERWRITE)) != 0)
			throwException(this->dbenv, ret, "DB->put");
	}
	/* Close the product file. */
	inFile.close();
}

/* 
 * Display all products.
 */ 
void Administrator::show_product()
{
	product pro;
	Dbt key, data;
	Dbc *pro_cursor;
	int ret;

	/* Initialize Dbts and data object. */
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	memset(&pro, 0, sizeof(product));

	pro_cursor = NULL;
	ret = 0;

	data.set_data(&pro);
	data.set_ulen(sizeof(product));
	data.set_flags(DB_DBT_USERMEM);

	/* Get a cursor. */
	if ((ret = this->product_dbp->cursor(NULL, &pro_cursor, 0)) != 0)
		throwException(this->dbenv, ret, "ProductDB->open");

	cout << "Product list display" << endl;

	/* Iterate over the database, retrieving each record in turn. */
	while ((ret = pro_cursor->get(&key, &data, DB_NEXT)) == 0) {
		/* Print product information. */
		print(pro);
	}
	/* Cursor must be closed. */
	pro_cursor->close();
}

/*
 * Search the product according to the productno and
 * return the corresponding product.
 */ 
product Administrator::search_product_no(int productno, Db *productdb)
{
	Dbt key, data;
	product pro;
	int ret;

	/* Initialize Dbts and data object. */
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	memset(&pro, 0, sizeof(product));
	ret = 0;

	/* Set up the database record's key. */
	key.set_data(&productno);
	key.set_ulen(sizeof(int));
	key.set_flags(DB_DBT_USERMEM);

	data.set_data(&pro);
	data.set_ulen(sizeof(product));
	data.set_flags(DB_DBT_USERMEM);

	/* Get the product according to the productno. */
	if ((ret = productdb->get(NULL, &key, &data, 0)) != 0)
		throwException(this->dbenv, ret, "Db->get");

	/* Print the product information. */
	this->print(pro);

	return pro;
}
 
/* 
 * Print product information.
 */
void Administrator::print(product pro)
{
	cout << pro.product_no << " " 
	    << pro.product_name << " " << pro.product_price << "RMB" << " " 
	    << pro.product_type << endl;
}

/* 
 * Bulk read from product database.
 */
void Administrator::bulk_read_product() 
{
	Dbt key, data;
	Dbt kp, dp;
	DbMultipleKeyDataIterator *ptrkd;
	Dbc *bulk_cursor;

	product pro;
	int ret;

	void *dbuf;
	u_int32_t dlen;

	/* Initialize Dbts. */
	memset(&data, 0, sizeof(Dbt));
	memset(&key, 0, sizeof(Dbt));
	memset(&kp, 0, sizeof(Dbt));
	memset(&dp, 0, sizeof(Dbt));

	ptrkd = NULL;
	bulk_cursor = NULL;
	memset(&pro, 0, sizeof(product));
	ret = 0;

	dlen = BUFFERLENGTH;
	dbuf = malloc(dlen);
	memset(dbuf, 0, dlen);

	/* Set up the database record's key. */
	key.set_data(&pro.product_no);
	key.set_size(sizeof(int));

	data.set_flags(DB_DBT_USERMEM);
	data.set_data(dbuf);
	data.set_ulen(dlen);

	/* Get a cursor. */
	this->product_dbp->cursor(NULL, &bulk_cursor, DB_CURSOR_BULK);
	ret = bulk_cursor->get(&key, &data, DB_NEXT | DB_MULTIPLE_KEY);

	while (ret != DB_NOTFOUND) {
		ptrkd = new DbMultipleKeyDataIterator(data);
		while (ptrkd->next(kp, dp) == true) {
			cout << ((product *)dp.get_data())->product_no
			    << " " << ((product *)dp.get_data())->product_name
			    << " " << ((product *)dp.get_data())->product_price
			    << "RMB" << " " 
			    << ((product *)dp.get_data())->product_type << endl;

			memset(&pro, 0, sizeof(product));
		}
		ret = bulk_cursor->get(&key, &data, DB_NEXT | DB_MULTIPLE_KEY);
	}

	if (ptrkd != NULL) {
		delete ptrkd;
		ptrkd = NULL;
	}
	if (bulk_cursor != NULL) {
		bulk_cursor->close();
	}
	if (dbuf != NULL) {
		dbuf = NULL;
	}
}

/*
 * Get the product database handle.
 */ 
Db * Administrator::get_productdb()
{
	return product_dbp;
}

/*
 * Get the order database handle.
 */ 
Db * Administrator::get_orderdb()
{
	return order_dbp;
}

/*
 * Get the Secondary database handle.
 */
Db * Administrator::get_productsdb()
{
	return product_type_sdbp;
}

/*
 * Close all databases and the environment.
 */
void Administrator::close_db()
{
	int ret = 0;
	if (this->product_dbp != NULL) {
		ret = this->product_dbp->close(0);

	if (ret != 0)
		this->product_dbp->err(ret, "Db->close");

		delete this->product_dbp;
		this->product_dbp = NULL;
	}

	if (this->product_type_sdbp != NULL) {
		ret = this->product_type_sdbp->close(0);

		if (ret != 0)
			this->product_type_sdbp->err(ret, "Db->close");

		delete this->product_type_sdbp;
		this->product_type_sdbp = NULL;
	}

	if (this->order_dbp != NULL) {
		ret = this->order_dbp->close(0);

		if (ret != 0)
			this->order_dbp->err(ret, "Db->close");

		delete this->order_dbp;
		this->order_dbp = NULL;
	}

	if (dbenv != NULL) {
		ret = this->dbenv->close(0);

		if (ret != 0)
			this->dbenv->err(ret, "DbEnv->close");

		delete this->dbenv;
		this->dbenv = NULL;
	}
}

/*
 * Throw Exception method.
 */
void Administrator::throwException(DbEnv *dbenvp, int ret, const char *msg)
{
	if (dbenvp != NULL && msg != NULL) {
		dbenvp->err(ret, msg);
	}
	throw DbException(ret);
}

/*
 * Customer implementation module.
 */
void Customer::customer_run(Db *productdbp, Db *orderdbp, Db *productsdbp)
{
	int flag = 1;
	/* Customer run the system. */
	while (flag == 1) {
		/*
		 * For the interface of first layer, customer can
		 * display all products, dispaly products according to
		 * the product type, operate the order and
		 * shut down the system.
		 */
		cout << "=================================================";
		cout << endl;
		cout << "Command Instructions:" << endl;
		cout << "-d    Display all product list." << endl;
		cout << "-t    Display products according to product type.";
		cout << endl;
		cout << "-o    Operate the order." << endl;
		cout << "-s    Shut down the system." << endl;
		cout << "Please choose the command:" << endl;
		
		char command;
		cin >> command;
		cout << "=================================================";
		cout << endl;

		switch (command) {
		case 'd':
			/* Display the product database. */
			this->display_product_database(productdbp);
			break;
		case 't':
			/* Display products according to product type. */
			this->display_product_based_on_type(productsdbp);
			break;
		case 'o':
			/* Order product into the order database. */
			this->operate_product(productdbp, orderdbp);
			break;
		case 's':
			/* Shut down the system. */
			flag = 0;
			break;
		default:
			/* Give some wrong warning. */
			cout << "Wrong input." << endl;
			break;
		}
	}
}

/* 
 * Operate the product.
 */
void Customer::operate_product(Db *productdbp, Db *orderdbp)
{
	int flag = 1;
	while (flag == 1) {
		/*
		 * For the operation on orderlist, customer can 
		 * order, display the order, confirm the order and
		 * delete from the order.
		 */
		cout << "=================================================";
		cout << endl;
		cout << "Operation on orderlist" << endl;
		cout << "=================================================";
		cout << endl;
		cout << "Command Instructions:" << endl;
		cout << "-o    Order the product list." << endl;
		cout << "-D    Display the order list." << endl;
		cout << "-d    Delete from the order list." << endl;
		cout << "-c    Quit from operation on orderlist." << endl;
		cout << "Please choose the command:" << endl;

		char command;
		cin >> command;
		cout << "=================================================";
		cout << endl;

		switch (command) {
		case 'o':
			/* Order the product. */
			this->order_product(productdbp, orderdbp);
			break;
		case 'c':
			/*
			 * Confirm the order database, 
			 * and quit from operation on order. 
			 */
			cout << "Thanks for your ordering!" << endl;
			/* Reset the flag. */
			flag = 0;
			break;
		case 'D':
			/* Display the order database. */
			this->display_order_database(orderdbp, productdbp);
			break;
		case 'd':
			/* Delete from the order database. */
			this->delete_order(orderdbp);
			break;
		default:
			/* Give some wrong warning. */
			cout << "Wrong input." << endl;
			break;
		}
	}
}

/*
 * Order product.
 */
void Customer::order_product(Db *productdbp, Db *orderdbp)
{
	product pro;
	order ord;

	int ret;
	int productnum;
	int productno;

	/* Initialzie data structure. */
	memset(&pro, 0, sizeof(product));
	memset(&ord, 0, sizeof(order));

	productnum = 0;
	productno = 0;

	/* Display the product database. */
	this->display_product_database(productdbp);

	int lcondition = 1;
	while (lcondition == 1) {
		cout << "Please input the product no:" << endl;
		cin >> productno;

		cout << "Please input the product num:" << endl;
		cin >> productnum;

		/* Search corresponding product from the product database. */
		ret = this->search_product_no(productno, productdbp, pro);

		if (ret == 0) {
			cout << "Confirmation:" << endl;
			cout << "You have ordered " 
			    << productnum << " " << pro.product_name << endl;

			/* Insert product into the order database. */
			this->insert_into_order(pro, orderdbp, productnum);
			lcondition = 0;
		} else {
			cout <<  "Wrong input." << endl;
		}
	}
}

/*
 * Search the product according to the productno and
 * return the corresponding product.
 */  
int Customer::search_product_no(int productno, Db *productdbp, product &pro)
{
	Dbt key, data;
	int ret;
	Dbc *pro_cursor;
	pro_cursor = NULL;

	/* Initialize Dbts and data object. */
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	memset(&pro, 0, sizeof(product));

	ret = 0;
	pro.product_no = productno;

	key.set_data(&(pro.product_no));
	key.set_size(sizeof(int));

	data.set_data(&pro);
	data.set_ulen(sizeof(product));
	data.set_flags(DB_DBT_USERMEM);

	/* Get a cursor. */
	if ((ret = productdbp->cursor(NULL, &pro_cursor, 0)) != 0)
		return ret;
	if ((ret = pro_cursor->get(&key, &data, DB_SET)) != 0)
		return ret;

	/* Cursor must be closed. */
	if (pro_cursor != NULL) {
		pro_cursor->close();
	}

	return ret;
}

/* 
 * Delete from the order database.
 */
void Customer::delete_order(Db *orderdbp)
{
	int productno;
	int ret;

	productno = 0;
	ret = 0;

	cout << "Please input the product no:" << endl;
	cin >> productno;

	/* Delete the order product according to productno. */
	ret = this->delete_from_order(orderdbp, productno);
	if (ret == 0)
		cout << "Delete successfully!" << endl;
	else
		cout << "Fail to delete!" << endl;
}

/* 
 * Delete from the order.
 */
int Customer::delete_from_order(Db *orderdbp, int productno)
{
	Dbt key;
	int ret;
	ret = 0;

	/* Initialize Dbt. */
	memset(&key, 0, sizeof(Dbt));
	key.set_data(&productno);
	key.set_size(sizeof(int));

	/*Delete the product from order. */
	ret = orderdbp->del(NULL, &key, 0);

	return ret;
}

/* 
 * Insert the product into order database.
 */
void Customer::insert_into_order(product pro, Db *orderdbp, int num)
{
	order ord;
	string productno;

	/* Initialize data object. */
	memset(&ord, 0, sizeof(order));
	ord.product_no = pro.product_no;
	ord.product_num = num;

	/* Set up the database record's key. */
	Dbt key(&(pro.product_no), sizeof(int));
	/* Set up the database record's data. */
	Dbt data(&ord, sizeof(order));

	/* Put the data into database. */
	orderdbp->put(NULL, &key, &data, DB_NOOVERWRITE);

	cout << "Order successfully!" << endl;
}

/* 
 * Display the order database.
 */
void Customer::display_order_database(Db *orderdbp, Db *productdbp)
{
	Dbt key, data;
	Dbt key1, data1;
	Dbc *ord_cursor;
	Dbc *pro_cursor;

	int ret;
	int sum;

	order ord;
	product pro;

	ret = 0;
	sum = 0;

	ord_cursor = NULL;
	pro_cursor = NULL;

	/* Initialize data object. */
	memset(&ord, 0, sizeof(order));
	memset(&pro, 0, sizeof(product));

	/* Initialize Dbts. */
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	memset(&key1, 0, sizeof(Dbt));
	memset(&data1, 0, sizeof(Dbt));

	key.set_data(&(ord.product_no));
	key.set_size(sizeof(int));

	data.set_data(&ord);
	data.set_ulen(sizeof(order));
	data.set_flags(DB_DBT_USERMEM);

	/* Get a cursor. */
	if ((ret = orderdbp->cursor(NULL, &ord_cursor, 0)) != 0)
		goto err;

	/* Get a cursor. */
	if ((ret = productdbp->cursor(NULL, &pro_cursor, 0)) != 0)
		goto err;

	cout << "Order list display" << endl;

	/* Iterate over the database, retrieving each record in turn. */
	while ((ret = ord_cursor->get(&key, &data, DB_NEXT)) == 0) {
		/* Set up the database record's key. */
		key1.set_data(&(ord.product_no));
		key1.set_size(sizeof(int));

		data1.set_data(&pro);
		data1.set_ulen(sizeof(product));
		data1.set_flags(DB_DBT_USERMEM);

		/* Getting the product according to order information. */
		pro_cursor->get(&key1, &data1, DB_SET);
		this->display_order_information(pro, ord);

		/* Calculate the sum of order database. */
		sum += ord.product_num * pro.product_price;
	}
	cout << endl;
	cout << "Total money: " << sum << "RMB" << endl;
err:
	/* Cursor must be closed. */
	if (ord_cursor != NULL)
		ord_cursor->close();
	if (pro_cursor != NULL)
		pro_cursor->close();
}

/* 
 * Display the product database.
 */
void Customer::display_product_database(Db *productdbp)
{
	product pro;
	Dbt key, data;
	Dbc *pro_cursor;
	int ret;

	/* Initialize Dbts and data object. */
	memset(&key, 0, sizeof(Dbt));
	memset(&data, 0, sizeof(Dbt));
	memset(&pro, 0, sizeof(product));

	pro_cursor = NULL;
	ret = 0;

	data.set_data(&pro);
	data.set_ulen(sizeof(product));
	data.set_flags(DB_DBT_USERMEM);

	/* Get a cursor. */
	if ((ret = productdbp->cursor(NULL, &pro_cursor, 0)) != 0)
		goto err;
	
	/* Iterate over the database, retrieving each record in turn. */
	while ((ret = pro_cursor->get(&key, &data, DB_NEXT)) == 0) {
		/* Display product details. */
		this->display_product_information(pro);
	}
err:
	/* Cursor must be closed. */
	if (pro_cursor != NULL)
		pro_cursor->close();
}

/* 
 * Display product database based on product type.
 */
void Customer::display_product_based_on_type(Db *productsdbp)
{
	int flag;
	string type;

	/* Initialize the loop condition. */
	flag = 1;

	while (flag == 1) {
		cout << "Please input the product type ";
		cout << "according to the chart below:" << endl;
		cout << "B --------------- Burger" << endl;
		cout << "S --------------- Snack" << endl;
		cout << "C --------------- Chicken" << endl;
		cout << "s --------------- Salad" << endl;
		cout << "D --------------- Drink" << endl;

		char ch;
		cin >> ch;

		switch (ch) {
		case 'B':
			type = "Burger";
			flag = 0;
			break;
		case 'S':
			type = "Snack";
			flag = 0;
			break;
		case 'C':
			type = "Chicken";
			flag = 0;
			break;
		case 's':
			type = "Salad";
			flag = 0;
			break;
		case 'D':
			type = "Drink";
			flag = 0;
			break;
		}
	}
	cout << "type:" << type << endl;
	this->display_sdbp(type, productsdbp);
}

/* 
 * Display product based on product type.
 */
void Customer::display_sdbp(string type, Db *productsdbp)
{
	Dbt key, data;
	Dbc *prosdb_cursor;
	product pro;
	int ret;
	char t[MAXLEN];

	/* Initialize Dbt. */
	memset(&key, 0, sizeof(Dbt));
	memset(t, 0, sizeof(t));
	prosdb_cursor = NULL;
	ret = 0;

	strcpy(t, type.c_str());

	key.set_data(t);
	key.set_size(sizeof(t));

	data.set_data(&pro);
	data.set_ulen(sizeof(product));
	data.set_flags(DB_DBT_USERMEM);

	/* Get a cursor. */
	if ((ret = productsdbp->cursor(NULL, &prosdb_cursor, 0)) != 0)
		goto err;
	/* Get the product according to product type. */
	if ((ret = prosdb_cursor->get(&key, &data, DB_SET)) != 0)
		goto err;

	while (ret != DB_NOTFOUND) {
		/* Display product details. */
		this->display_product_information(pro);
		/* Get the next product according to product type. */
		ret = prosdb_cursor->get(&key, &data, DB_NEXT_DUP);
	}
err:
	/* Cursor must be closed. */
	if (prosdb_cursor != NULL)
		prosdb_cursor->close();
}

/*
 * Display the product information.
 */
int Customer::display_product_information(product pro)
{
	cout << pro.product_no << " " 
	    << pro.product_name << " " 
	    << pro.product_price << "RMB" << " " 
	    << pro.product_type << endl;

	return (0);
}

/*
 * Display the order information.
 */ 
int Customer::display_order_information(product pro, order ord)
{
	cout << pro.product_no << " " << pro.product_name << " " 
	    << pro.product_price << "RMB" << " " << pro.product_type 
	    << " " << ord.product_num << endl;

	return (0);
}

int main() 
{
	/* Configure the running environment. */
	system(CLEANUP_CMD);
	system(MAKEENV_CMD);
	system(COPY_CMD);

	/* Administrator initialize the system environment. */
	Administrator admin;
	admin.admin_login();
	admin.admin_run();

	cout << endl;
	cout << "Environment setup successfully." << endl;
	cout << "System starts now." << endl;
	cout << endl;

	/* Customer run the system. */
	Customer cc;
	cc.customer_run(admin.get_productdb(), 
	    admin.get_orderdb(), admin.get_productsdb());

	/* Administrator close the database. */
	admin.close_db();
	cout << "Thanks for your using!" << endl;
}
