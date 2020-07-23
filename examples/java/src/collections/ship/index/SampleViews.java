/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.index;

import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.serial.ClassCatalog;
import com.sleepycat.bind.serial.SerialBinding;
import com.sleepycat.collections.StoredEntrySet;
import com.sleepycat.collections.StoredSortedMap;

/**
 * SampleViews defines the data bindings and collection views for the sample
 * database.
 *
 * @author Mark Hayes
 */
public class SampleViews {

    private final StoredSortedMap<PartKey, PartData> partMap;
    private final StoredSortedMap<SupplierKey, SupplierData> supplierMap;
    private final StoredSortedMap<ShipmentKey, ShipmentData> shipmentMap;
    private final StoredSortedMap<PartKey, ShipmentData> shipmentByPartMap;
    private final StoredSortedMap<SupplierKey, ShipmentData> shipmentBySupplierMap;
    private final StoredSortedMap<String, SupplierData> supplierByCityMap;

    /**
     * Create the data bindings and collection views.
     * @param db
     */
    public SampleViews(SampleDatabase db) {

        // Create the data bindings.
        // In this sample, the stored key and data entries are used directly
        // rather than mapping them to separate objects. Therefore, no binding
        // classes are defined here and the SerialBinding class is used.
        //
        ClassCatalog catalog = db.getClassCatalog();
        EntryBinding<PartKey> partKeyBinding =
            new SerialBinding<>(catalog, PartKey.class);
        EntryBinding<PartData> partDataBinding =
            new SerialBinding<>(catalog, PartData.class);
        EntryBinding<SupplierKey> supplierKeyBinding =
            new SerialBinding<>(catalog, SupplierKey.class);
        EntryBinding<SupplierData> supplierDataBinding =
            new SerialBinding<>(catalog, SupplierData.class);
        EntryBinding<ShipmentKey> shipmentKeyBinding =
            new SerialBinding<>(catalog, ShipmentKey.class);
        EntryBinding<ShipmentData> shipmentDataBinding =
            new SerialBinding<>(catalog, ShipmentData.class);
        EntryBinding<String> cityKeyBinding =
            new SerialBinding<>(catalog, String.class);

        // Create map views for all stores and indices.
        // StoredSortedMap is not used since the stores and indices are
        // ordered by serialized key objects, which do not provide a very
        // useful ordering.
        //
        partMap =
            new StoredSortedMap<>(db.getPartDatabase(),
                                partKeyBinding, partDataBinding, true);
        supplierMap =
            new StoredSortedMap<>(db.getSupplierDatabase(),
                                supplierKeyBinding, supplierDataBinding, true);
        shipmentMap =
            new StoredSortedMap<>(db.getShipmentDatabase(),
                                shipmentKeyBinding, shipmentDataBinding, true);
        shipmentByPartMap =
            new StoredSortedMap<>(db.getShipmentByPartDatabase(),
                                partKeyBinding, shipmentDataBinding, true);
        shipmentBySupplierMap =
            new StoredSortedMap<>(db.getShipmentBySupplierDatabase(),
                                supplierKeyBinding, shipmentDataBinding, true);
        supplierByCityMap =
            new StoredSortedMap<>(db.getSupplierByCityDatabase(),
                                cityKeyBinding, supplierDataBinding, true);
    }

    // The views returned below can be accessed using the java.util.Map or
    // java.util.Set interfaces, or using the StoredSortedMap and
    // StoredEntrySet classes, which provide additional methods.  The entry
    // sets could be obtained directly from the Map.entrySet() method, but
    // convenience methods are provided here to return them in order to avoid
    // down-casting elsewhere.

    /**
     * Return a map view of the part storage container.
     * @return 
     */
    public final StoredSortedMap<PartKey, PartData> getPartMap() {

        return partMap;
    }

    /**
     * Return a map view of the supplier storage container.
     * @return 
     */
    public final StoredSortedMap<SupplierKey, SupplierData> getSupplierMap() {

        return supplierMap;
    }

    /**
     * Return a map view of the shipment storage container.
     * @return 
     */
    public final StoredSortedMap<ShipmentKey, ShipmentData> getShipmentMap() {

        return shipmentMap;
    }

    /**
     * Return an entry set view of the part storage container.
     * @return 
     */
    public final StoredEntrySet<PartKey, PartData> getPartEntrySet() {

        return (StoredEntrySet<PartKey, PartData>) partMap.entrySet();
    }

    /**
     * Return an entry set view of the supplier storage container.
     * @return 
     */
    public final StoredEntrySet<SupplierKey, SupplierData> getSupplierEntrySet() {

        return (StoredEntrySet<SupplierKey, SupplierData>) supplierMap.entrySet();
    }

    /**
     * Return an entry set view of the shipment storage container.
     * @return 
     */
    public final StoredEntrySet<ShipmentKey, ShipmentData> getShipmentEntrySet() {

        return (StoredEntrySet<ShipmentKey, ShipmentData>) shipmentMap.entrySet();
    }

    /**
     * Return a map view of the shipment-by-part index.
     * @return 
     */
    public StoredSortedMap<PartKey, ShipmentData> getShipmentByPartMap() {

        return shipmentByPartMap;
    }

    /**
     * Return a map view of the shipment-by-supplier index.
     * @return 
     */
    public StoredSortedMap<SupplierKey, ShipmentData> getShipmentBySupplierMap() {

        return shipmentBySupplierMap;
    }

    /**
     * Return a map view of the supplier-by-city index.
     * @return 
     */
    public final StoredSortedMap<String, SupplierData> getSupplierByCityMap() {

        return supplierByCityMap;
    }
}
