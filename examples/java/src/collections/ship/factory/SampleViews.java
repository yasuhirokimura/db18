/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.factory;

import com.sleepycat.collections.StoredSortedMap;
import com.sleepycat.collections.StoredSortedValueSet;
import com.sleepycat.collections.TupleSerialFactory;

/**
 * SampleViews defines the data bindings and collection views for the sample
 * database.
 *
 * @author Mark Hayes
 */
public class SampleViews {

    private final StoredSortedMap<PartKey, Part> partMap;
    private final StoredSortedMap<SupplierKey, Supplier> supplierMap;
    private final StoredSortedMap<ShipmentKey, Shipment> shipmentMap;
    private final StoredSortedMap<PartKey, Shipment> shipmentByPartMap;
    private final StoredSortedMap<SupplierKey, Shipment> shipmentBySupplierMap;
    private final StoredSortedMap<String, Supplier> supplierByCityMap;

    /**
     * Create the data bindings and collection views.
     * @param db
     */
    public SampleViews(SampleDatabase db) {

        // Use the TupleSerialFactory for a Serial/Tuple-based database
        // where marshalling interfaces are used.
        //
        TupleSerialFactory factory = db.getFactory();

        // Create map views for all stores and indices.
        // StoredSortedMap is used since the stores and indices are ordered
        // (they use the DB_BTREE access method).
        //
        partMap =
            factory.newSortedMap(db.getPartDatabase(),
                                 PartKey.class, Part.class, true);
        supplierMap =
            factory.newSortedMap(db.getSupplierDatabase(),
                                 SupplierKey.class, Supplier.class, true);
        shipmentMap =
            factory.newSortedMap(db.getShipmentDatabase(),
                                 ShipmentKey.class, Shipment.class, true);
        shipmentByPartMap =
            factory.newSortedMap(db.getShipmentByPartDatabase(),
                                 PartKey.class, Shipment.class, true);
        shipmentBySupplierMap =
            factory.newSortedMap(db.getShipmentBySupplierDatabase(),
                                 SupplierKey.class, Shipment.class, true);
        supplierByCityMap =
            factory.newSortedMap(db.getSupplierByCityDatabase(),
                                 String.class, Supplier.class, true);
    }

    // The views returned below can be accessed using the java.util.Map or
    // java.util.Set interfaces, or using the StoredMap and StoredValueSet
    // classes, which provide additional methods.  The entity sets could be
    // obtained directly from the Map.values() method but convenience methods
    // are provided here to return them in order to avoid down-casting
    // elsewhere.

    /**
     * Return a map view of the part storage container.
     * @return 
     */
    public StoredSortedMap<PartKey, Part> getPartMap() {

        return partMap;
    }

    /**
     * Return a map view of the supplier storage container.
     * @return 
     */
    public StoredSortedMap<SupplierKey, Supplier> getSupplierMap() {

        return supplierMap;
    }

    /**
     * Return a map view of the shipment storage container.
     * @return 
     */
    public StoredSortedMap<ShipmentKey, Shipment> getShipmentMap() {

        return shipmentMap;
    }

    /**
     * Return an entity set view of the part storage container.
     * @return 
     */
    public StoredSortedValueSet<Part> getPartSet() {

        return (StoredSortedValueSet<Part>) partMap.values();
    }

    /**
     * Return an entity set view of the supplier storage container.
     * @return 
     */
    public StoredSortedValueSet<Supplier> getSupplierSet() {

        return (StoredSortedValueSet<Supplier>) supplierMap.values();
    }

    /**
     * Return an entity set view of the shipment storage container.
     * @return 
     */
    public StoredSortedValueSet<Shipment> getShipmentSet() {

        return (StoredSortedValueSet<Shipment>) shipmentMap.values();
    }

    /**
     * Return a map view of the shipment-by-part index.
     * @return 
     */
    public StoredSortedMap<PartKey, Shipment> getShipmentByPartMap() {

        return shipmentByPartMap;
    }

    /**
     * Return a map view of the shipment-by-supplier index.
     * @return 
     */
    public StoredSortedMap<SupplierKey, Shipment> getShipmentBySupplierMap() {

        return shipmentBySupplierMap;
    }

    /**
     * Return a map view of the supplier-by-city index.
     * @return 
     */
    public StoredSortedMap<String, Supplier> getSupplierByCityMap() {

        return supplierByCityMap;
    }
}
