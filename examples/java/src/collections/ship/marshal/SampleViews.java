/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.marshal;

import com.sleepycat.bind.EntityBinding;
import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.serial.ClassCatalog;
import com.sleepycat.bind.serial.TupleSerialBinding;
import com.sleepycat.bind.tuple.TupleBinding;
import com.sleepycat.bind.tuple.TupleInput;
import com.sleepycat.bind.tuple.TupleOutput;
import com.sleepycat.collections.StoredSortedMap;
import com.sleepycat.collections.StoredSortedValueSet;
import com.sleepycat.util.RuntimeExceptionWrapper;

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
        // key/data entry pair to a combined data object; a "tricky" binding
        // that uses transient fields is used--see PartBinding, etc, for
        // details.  For keys, a one-to-one binding is implemented with
        // EntryBinding classes to bind the stored tuple entry to a key Object.
        //
        ClassCatalog catalog = db.getClassCatalog();
        EntryBinding<PartKey> partKeyBinding =
            new MarshalledKeyBinding<>(PartKey.class);
        EntityBinding<Part> partDataBinding =
            new MarshalledEntityBinding<>(catalog, Part.class);
        EntryBinding<SupplierKey> supplierKeyBinding =
            new MarshalledKeyBinding<>(SupplierKey.class);
        EntityBinding<Supplier> supplierDataBinding =
            new MarshalledEntityBinding<>(catalog, Supplier.class);
        EntryBinding<ShipmentKey> shipmentKeyBinding =
            new MarshalledKeyBinding<>(ShipmentKey.class);
        EntityBinding<Shipment> shipmentDataBinding =
            new MarshalledEntityBinding<>(catalog, Shipment.class);
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
     * MarshalledKeyBinding is used to bind the stored key tuple entry to a key
     * object representation.  To do this, it calls the MarshalledKey interface
     * implemented by the key class.
     */
    private static class MarshalledKeyBinding<E extends MarshalledKey> extends TupleBinding<E> {

        private Class<E> keyClass;

        /**
         * Construct the binding object.
         */
        private MarshalledKeyBinding(Class<E> keyClass) {

            this.keyClass = keyClass;
        }

        /**
         * Create the key object from the stored key tuple entry.
         */
        @Override
        public E entryToObject(TupleInput input) {

            try {
                E key = keyClass.newInstance();
                key.unmarshalKey(input);
                return key;
            } catch (IllegalAccessException | InstantiationException e) {
                throw new RuntimeExceptionWrapper(e);
            }
        }

        /**
         * Create the stored key tuple entry from the key object.
         */
        @Override
        public void objectToEntry(E object, TupleOutput output) {

            object.marshalKey(output);
        }
    }

    /**
     * MarshalledEntityBinding is used to bind the stored key/data entry pair
     * to a combined to an entity object representation.  To do this, it calls
     * the MarshalledEnt interface implemented by the entity class.
     *
     * <p> The binding is "tricky" in that it uses the entity class for both
     * the stored data entry and the combined entity object.  To do this,
     * entity's key field(s) are transient and are set by the binding after the
     * data object has been deserialized. This avoids the use of a "data" class
     * completely. </p>
     */
    private static class MarshalledEntityBinding
            <E extends MarshalledEnt> extends TupleSerialBinding<E,E> {

        /**
         * Construct the binding object.
         */
        private MarshalledEntityBinding(ClassCatalog classCatalog,
                                        Class<E> entityClass) {

            super(classCatalog, entityClass);
        }

        /**
         * Create the entity by combining the stored key and data.
         * This "tricky" binding returns the stored data as the entity, but
         * first it sets the transient key fields from the stored key.
         */
        @Override
        public E entryToObject(TupleInput tupleInput, E javaInput) {

            javaInput.unmarshalPrimaryKey(tupleInput);
            return javaInput;
        }

        /**
         * Create the stored key from the entity.
         */
        @Override
        public void objectToKey(E object, TupleOutput output) {

            object.marshalPrimaryKey(output);
        }

        /**
         * Return the entity as the stored data.  There is nothing to do here
         * since the entity's key fields are transient.
         */
        @Override
        public E objectToData(E object) {

            return object;
        }
    }
}
