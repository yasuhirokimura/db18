/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.factory;

import java.io.Serializable;

import com.sleepycat.bind.tuple.MarshalledTupleKeyEntity;
import com.sleepycat.bind.tuple.TupleInput;
import com.sleepycat.bind.tuple.TupleOutput;

/**
 * A Supplier represents the combined key/data pair for a supplier entity.
 *
 * <p> In this sample, Supplier is bound to the stored key/data entry by
 * implementing the MarshalledTupleKeyEntity interface. </p>
 *
 * <p> The binding is "tricky" in that it uses this class for both the stored
 * data entry and the combined entity object.  To do this, the key field(s) are
 * transient and are set by the binding after the data object has been
 * deserialized. This avoids the use of a SupplierData class completely. </p>
 *
 * <p> Since this class is used directly for data storage, it must be
 * Serializable. </p>
 *
 * @author Mark Hayes
 */
public class Supplier implements Serializable, MarshalledTupleKeyEntity {

    static final String CITY_KEY = "city";

    private transient String number;
    private final String name;
    private final int status;
    private final String city;

    public Supplier(String number, String name, int status, String city) {

        this.number = number;
        this.name = name;
        this.status = status;
        this.city = city;
    }

    public final String getNumber() {

        return number;
    }

    public final String getName() {

        return name;
    }

    public final int getStatus() {

        return status;
    }

    public final String getCity() {

        return city;
    }

    @Override
    public String toString() {

        return "[Supplier: number=" + number +
	    " name=" + name +
	    " status=" + status +
	    " city=" + city + ']';
    }

    // --- MarshalledTupleKeyEntity implementation ---

    @Override
    public void marshalPrimaryKey(TupleOutput keyOutput) {

        keyOutput.writeString(this.number);
    }

    @Override
    public void unmarshalPrimaryKey(TupleInput keyInput) {

        this.number = keyInput.readString();
    }

    /**
     * {@inheritDoc}
     * @throws UnsupportedOperationException
     */
    @Override
    public boolean marshalSecondaryKey(String keyName, TupleOutput keyOutput) {

        if (keyName.equals(CITY_KEY)) {
            if (this.city != null) {
                keyOutput.writeString(this.city);
                return true;
            } else {
                return false;
            }
        } else {
            throw new UnsupportedOperationException(keyName);
        }
    }

    /**
     * {@inheritDoc}
     * @throws UnsupportedOperationException
     */
    @Override
    public boolean nullifyForeignKey(String keyName) {

        throw new UnsupportedOperationException(keyName);
    }
}
