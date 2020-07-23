/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.entity;

import com.sleepycat.bind.EntityBinding;
import com.sleepycat.bind.serial.ClassCatalog;
import com.sleepycat.bind.serial.SerialBinding;
import com.sleepycat.bind.serial.SerialSerialBinding;
import com.sleepycat.collections.StoredSortedMap;
import com.sleepycat.collections.StoredValueSet;

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

        // Create the data bindings.
        // In this sample, EntityBinding classes are used to bind the stored
        // key/data entry pair to a combined data object.  For keys, however,
        // the stored entry is used directly via a SerialBinding and no
        // special binding class is needed.
        //
        ClassCatalog catalog = db.getClassCatalog();
        SerialBinding<PartKey> partKeyBinding =
            new SerialBinding<>(catalog, PartKey.class);
        EntityBinding<Part> partDataBinding =
            new PartBinding(catalog, PartKey.class, PartData.class);
        SerialBinding<SupplierKey> supplierKeyBinding =
            new SerialBinding<>(catalog, SupplierKey.class);
        EntityBinding<Supplier> supplierDataBinding =
            new SupplierBinding(catalog, SupplierKey.class,
                                SupplierData.class);
        SerialBinding<ShipmentKey> shipmentKeyBinding =
            new SerialBinding<>(catalog, ShipmentKey.class);
        EntityBinding<Shipment> shipmentDataBinding =
            new ShipmentBinding(catalog, ShipmentKey.class,
                                ShipmentData.class);
        SerialBinding<String> cityKeyBinding =
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
    // StoredValueSet classes, which provide additional methods.  The entity
    // sets could be obtained directly from the Map.values() method but
    // convenience methods are provided here to return them in order to avoid
    // down-casting elsewhere.

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
    public StoredValueSet<Part> getPartSet() {

        return (StoredValueSet<Part>) partMap.values();
    }

    /**
     * Return an entity set view of the supplier storage container.
     * @return 
     */
    public StoredValueSet<Supplier> getSupplierSet() {

        return (StoredValueSet<Supplier>) supplierMap.values();
    }

    /**
     * Return an entity set view of the shipment storage container.
     * @return 
     */
    public StoredValueSet<Shipment> getShipmentSet() {

        return (StoredValueSet<Shipment>) shipmentMap.values();
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
    public final StoredSortedMap<String, Supplier> getSupplierByCityMap() {

        return supplierByCityMap;
    }

    /**
     * PartBinding is used to bind the stored key/data entry pair for a part
     * to a combined data object (entity).
     */
    private static class PartBinding
            extends SerialSerialBinding<PartKey, PartData, Part> {

        /**
         * Construct the binding object.
         */
        private PartBinding(ClassCatalog classCatalog,
                            Class<PartKey> keyClass,
                            Class<PartData> dataClass) {

            super(classCatalog, keyClass, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Part entryToObject(PartKey keyInput, PartData dataInput) {

            return new Part(keyInput.getNumber(), dataInput.getName(), dataInput.getColor(),
                            dataInput.getWeight(), dataInput.getCity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public PartKey objectToKey(Part object) {

            return new PartKey(object.getNumber());
        }

        /**
         * Create the stored data from the entity.
         */
        @Override
        public PartData objectToData(Part object) {

            return new PartData(object.getName(), object.getColor(),
                                 object.getWeight(), object.getCity());
        }
    }

    /**
     * SupplierBinding is used to bind the stored key/data entry pair for a
     * supplier to a combined data object (entity).
     */
    private static class SupplierBinding
            extends SerialSerialBinding<SupplierKey, SupplierData, Supplier> {

        /**
         * Construct the binding object.
         */
        private SupplierBinding(ClassCatalog classCatalog,
                                Class<SupplierKey> keyClass,
                                Class<SupplierData> dataClass) {

            super(classCatalog, keyClass, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Supplier entryToObject(SupplierKey keyInput, SupplierData dataInput) {

            SupplierKey key = (SupplierKey) keyInput;
            SupplierData data = (SupplierData) dataInput;
            return new Supplier(keyInput.getNumber(), dataInput.getName(),
                                dataInput.getStatus(), dataInput.getCity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public SupplierKey objectToKey(Supplier object) {

            return new SupplierKey(object.getNumber());
        }

        /**
         * Create the stored data from the entity.
         */
        @Override
        public SupplierData objectToData(Supplier object) {

            return new SupplierData(object.getName(), object.getStatus(),
                                     object.getCity());
        }
    }

    /**
     * ShipmentBinding is used to bind the stored key/data entry pair for a
     * shipment to a combined data object (entity).
     */
    private static class ShipmentBinding
            extends SerialSerialBinding<ShipmentKey, ShipmentData, Shipment> {

        /**
         * Construct the binding object.
         */
        private ShipmentBinding(ClassCatalog classCatalog,
                                Class<ShipmentKey> keyClass,
                                Class<ShipmentData> dataClass) {

            super(classCatalog, keyClass, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Shipment entryToObject(ShipmentKey keyInput, ShipmentData dataInput) {

            return new Shipment(keyInput.getPartNumber(), keyInput.getSupplierNumber(),
                                dataInput.getQuantity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public ShipmentKey objectToKey(Shipment object) {

            return new ShipmentKey(object.getPartNumber(),
                                   object.getSupplierNumber());
        }

        /**
         * Create the stored data from the entity.
         */
        @Override
        public ShipmentData objectToData(Shipment object) {

            return new ShipmentData(object.getQuantity());
        }
    }
}
