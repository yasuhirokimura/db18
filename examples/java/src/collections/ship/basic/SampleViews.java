/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.basic;

import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.serial.ClassCatalog;
import com.sleepycat.bind.serial.SerialBinding;
import com.sleepycat.collections.StoredEntrySet;
import com.sleepycat.collections.StoredMap;

/**
 * SampleViews defines the data bindings and collection views for the sample
 * database.
 *
 * @author Mark Hayes
 */
public class SampleViews {

    private final StoredMap<PartKey, PartData> partMap;
    private final StoredMap<SupplierKey, SupplierData> supplierMap;
    private final StoredMap<ShipmentKey, ShipmentData> shipmentMap;

    /**
     * Create the data bindings and collection views.
     * @param db
     */
    public SampleViews(SampleDatabase db) {

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

        // Create map views for all stores and indices.
        // StoredSortedMap is not used since the stores and indices are
        // ordered by serialized key objects, which do not provide a very
        // useful ordering.
        //
        partMap =
            new StoredMap<>(db.getPartDatabase(),
                          partKeyBinding, partDataBinding, true);
        supplierMap =
            new StoredMap<>(db.getSupplierDatabase(),
                          supplierKeyBinding, supplierDataBinding, true);
        shipmentMap =
            new StoredMap<>(db.getShipmentDatabase(),
                          shipmentKeyBinding, shipmentDataBinding, true);
    }

    // The views returned below can be accessed using the java.util.Map or
    // java.util.Set interfaces, or using the StoredMap and StoredEntrySet
    // classes, which provide additional methods.  The entry sets could be
    // obtained directly from the Map.entrySet() method, but convenience
    // methods are provided here to return them in order to avoid down-casting
    // elsewhere.

    /**
     * Return a map view of the part storage container.
     * @return 
     */
    public final StoredMap<PartKey, PartData> getPartMap() {

        return partMap;
    }

    /**
     * Return a map view of the supplier storage container.
     * @return 
     */
    public final StoredMap<SupplierKey, SupplierData> getSupplierMap() {

        return supplierMap;
    }

    /**
     * Return a map view of the shipment storage container.
     * @return 
     */
    public final StoredMap<ShipmentKey, ShipmentData> getShipmentMap() {

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
}
