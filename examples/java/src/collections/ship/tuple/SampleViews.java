/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.tuple;

import com.sleepycat.bind.EntityBinding;
import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.serial.ClassCatalog;
import com.sleepycat.bind.serial.TupleSerialBinding;
import com.sleepycat.bind.tuple.TupleBinding;
import com.sleepycat.bind.tuple.TupleInput;
import com.sleepycat.bind.tuple.TupleOutput;
import com.sleepycat.collections.StoredSortedMap;
import com.sleepycat.collections.StoredSortedValueSet;

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
        // key/data entry pair to a combined data object.  For keys, a
        // one-to-one binding is implemented with EntryBinding classes to bind
        // the stored tuple entry to a key Object.
        //
        ClassCatalog catalog = db.getClassCatalog();
        EntryBinding<PartKey> partKeyBinding =
            new PartKeyBinding();
        EntityBinding<Part> partDataBinding =
            new PartBinding(catalog, PartData.class);
        EntryBinding<SupplierKey> supplierKeyBinding =
            new SupplierKeyBinding();
        EntityBinding<Supplier> supplierDataBinding =
            new SupplierBinding(catalog, SupplierData.class);
        EntryBinding<ShipmentKey> shipmentKeyBinding =
            new ShipmentKeyBinding();
        EntityBinding<Shipment> shipmentDataBinding =
            new ShipmentBinding(catalog, ShipmentData.class);
        EntryBinding<String> cityKeyBinding =
            TupleBinding.getPrimitiveBinding(String.class);

        // Create map views for all stores and indices.
        // StoredSortedMap is used since the stores and indices are ordered
        // (they use the DB_BTREE access method).
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
    public final StoredSortedMap<String, Supplier> getSupplierByCityMap() {

        return supplierByCityMap;
    }

    /**
     * PartKeyBinding is used to bind the stored key tuple entry for a part to
     * a key object representation.
     */
    private static class PartKeyBinding extends TupleBinding<PartKey> {

        /**
         * Construct the binding object.
         */
        private PartKeyBinding() {
        }

        /**
         * Create the key object from the stored key tuple entry.
         */
        @Override
        public PartKey entryToObject(TupleInput input) {

            String number = input.readString();
            return new PartKey(number);
        }

        /**
         * Create the stored key tuple entry from the key object.
         */
        @Override
        public void objectToEntry(PartKey object, TupleOutput output) {

            output.writeString(object.getNumber());
        }
    }

    /**
     * PartBinding is used to bind the stored key/data entry pair for a part
     * to a combined data object (entity).
     */
    private static class PartBinding
            extends TupleSerialBinding<PartData, Part> {

        /**
         * Construct the binding object.
         */
        private PartBinding
            (ClassCatalog classCatalog, Class<PartData> dataClass) {

            super(classCatalog, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Part entryToObject(TupleInput keyInput, PartData dataInput) {

            String number = keyInput.readString();
            return new Part(number, dataInput.getName(), dataInput.getColor(),
                            dataInput.getWeight(), dataInput.getCity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public void objectToKey(Part object, TupleOutput output) {

            output.writeString(object.getNumber());
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
     * SupplierKeyBinding is used to bind the stored key tuple entry for a
     * supplier to a key object representation.
     */
    private static class SupplierKeyBinding extends TupleBinding<SupplierKey> {

        /**
         * Construct the binding object.
         */
        private SupplierKeyBinding() {
        }

        /**
         * Create the key object from the stored key tuple entry.
         */
        @Override
        public SupplierKey entryToObject(TupleInput input) {

            String number = input.readString();
            return new SupplierKey(number);
        }

        /**
         * Create the stored key tuple entry from the key object.
         */
        @Override
        public void objectToEntry(SupplierKey object, TupleOutput output) {

            output.writeString(object.getNumber());
        }
    }

    /**
     * SupplierBinding is used to bind the stored key/data entry pair for a
     * supplier to a combined data object (entity).
     */
    private static class SupplierBinding
            extends TupleSerialBinding<SupplierData, Supplier> {

        /**
         * Construct the binding object.
         */
        private SupplierBinding(
                ClassCatalog classCatalog, Class<SupplierData> dataClass) {

            super(classCatalog, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Supplier entryToObject(
                TupleInput keyInput, SupplierData dataInput) {

            String number = keyInput.readString();
            return new Supplier(number, dataInput.getName(),
                                dataInput.getStatus(), dataInput.getCity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public void objectToKey(Supplier object, TupleOutput output) {

            output.writeString(object.getNumber());
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
     * ShipmentKeyBinding is used to bind the stored key tuple entry for a
     * shipment to a key object representation.
     */
    private static class ShipmentKeyBinding extends TupleBinding<ShipmentKey> {

        /**
         * Construct the binding object.
         */
        private ShipmentKeyBinding() {
        }

        /**
         * Create the key object from the stored key tuple entry.
         */
        @Override
        public ShipmentKey entryToObject(TupleInput input) {

            String partNumber = input.readString();
            String supplierNumber = input.readString();
            return new ShipmentKey(partNumber, supplierNumber);
        }

        /**
         * Create the stored key tuple entry from the key object.
         */
        @Override
        public void objectToEntry(ShipmentKey object, TupleOutput output) {

            output.writeString(object.getPartNumber());
            output.writeString(object.getSupplierNumber());
        }
    }

    /**
     * ShipmentBinding is used to bind the stored key/data entry pair for a
     * shipment to a combined data object (entity).
     */
    private static class ShipmentBinding
            extends TupleSerialBinding<ShipmentData, Shipment> {

        /**
         * Construct the binding object.
         */
        private ShipmentBinding(
                ClassCatalog classCatalog, Class<ShipmentData> dataClass) {

            super(classCatalog, dataClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         */
        @Override
        public Shipment entryToObject(
                TupleInput keyInput, ShipmentData dataInput) {

            String partNumber = keyInput.readString();
            String supplierNumber = keyInput.readString();
            return new Shipment(partNumber, supplierNumber,
                                dataInput.getQuantity());
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public void objectToKey(Shipment object, TupleOutput output) {

            output.writeString(object.getPartNumber());
            output.writeString(object.getSupplierNumber());
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
