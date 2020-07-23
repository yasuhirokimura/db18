/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.basic;

import java.io.File;
import java.io.FileNotFoundException;

import com.sleepycat.bind.serial.StoredClassCatalog;
import com.sleepycat.db.Database;
import com.sleepycat.db.DatabaseConfig;
import com.sleepycat.db.DatabaseException;
import com.sleepycat.db.DatabaseType;
import com.sleepycat.db.Environment;
import com.sleepycat.db.EnvironmentConfig;

/**
 * SampleDatabase defines the storage containers, indices and foreign keys
 * for the sample database.
 *
 * @author Mark Hayes
 */
public class SampleDatabase {

    private static final String CLASS_CATALOG = "java_class_catalog";
    private static final String SUPPLIER_STORE = "supplier_store";
    private static final String PART_STORE = "part_store";
    private static final String SHIPMENT_STORE = "shipment_store";

    private final Environment env;
    private final Database partDb;
    private final Database supplierDb;
    private final Database shipmentDb;
    private final StoredClassCatalog javaCatalog;

    /**
     * Open all storage containers, indices, and catalogs.
     * @param homeDirectory
     * @throws com.sleepycat.db.DatabaseException
     * @throws java.io.FileNotFoundException
     */
    public SampleDatabase(String homeDirectory)
        throws DatabaseException, FileNotFoundException {

        // Open the Berkeley DB environment in transactional mode.
        //
        System.out.println("Opening environment in: " + homeDirectory);
        EnvironmentConfig envConfig = new EnvironmentConfig();
        envConfig.setTransactional(true);
        envConfig.setAllowCreate(true);
        envConfig.setInitializeCache(true);
        envConfig.setInitializeLocking(true);
        env = new Environment(new File(homeDirectory), envConfig);

        // Set the Berkeley DB config for opening all stores.
        //
        DatabaseConfig dbConfig = new DatabaseConfig();
        dbConfig.setTransactional(true);
        dbConfig.setAllowCreate(true);
        dbConfig.setType(DatabaseType.BTREE);

        // Create the Serial class catalog.  This holds the serialized class
        // format for all database records of serial format.
        //
        Database catalogDb = env.openDatabase(null, CLASS_CATALOG, null,
                                              dbConfig);
        javaCatalog = new StoredClassCatalog(catalogDb);

        // Open the Berkeley DB database for the part, supplier and shipment
        // stores.  The stores are opened with no duplicate keys allowed.
        //
        partDb = env.openDatabase(null, PART_STORE, null, dbConfig);

        supplierDb = env.openDatabase(null, SUPPLIER_STORE, null, dbConfig);

        shipmentDb = env.openDatabase(null, SHIPMENT_STORE, null, dbConfig);
    }

    /**
     * Return the storage environment for the database.
     * @return 
     */
    public final Environment getEnvironment() {

        return env;
    }

    /**
     * Return the class catalog.
     * @return 
     */
    public final StoredClassCatalog getClassCatalog() {

        return javaCatalog;
    }

    /**
     * Return the part storage container.
     * @return 
     */
    public final Database getPartDatabase() {

        return partDb;
    }

    /**
     * Return the supplier storage container.
     * @return 
     */
    public final Database getSupplierDatabase() {

        return supplierDb;
    }

    /**
     * Return the shipment storage container.
     * @return 
     */
    public final Database getShipmentDatabase() {

        return shipmentDb;
    }

    /**
     * Close all databases and the environment.
     * @throws com.sleepycat.db.DatabaseException
     */
    public void close()
        throws DatabaseException {

        partDb.close();
        supplierDb.close();
        shipmentDb.close();
        // And don't forget to close the catalog and the environment.
        javaCatalog.close();
        env.close();
    }
}
