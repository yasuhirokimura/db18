/*-
 * Copyright (c) 2002, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package collections.ship.basic;

import java.io.Serializable;

/**
 * A ShipmentKey serves as the key in the key/data pair for a shipment entity.
 *
 * <p> In this sample, ShipmentKey is used both as the storage entry for the
 * key as well as the object binding to the key.  Because it is used directly
 * as storage data using serial format, it must be Serializable. </p>
 *
 * @author Mark Hayes
 */
public class ShipmentKey implements Serializable {

    private final String partNumber;
    private final String supplierNumber;

    public ShipmentKey(String partNumber, String supplierNumber) {

        this.partNumber = partNumber;
        this.supplierNumber = supplierNumber;
    }

    public final String getPartNumber() {

        return partNumber;
    }

    public final String getSupplierNumber() {

        return supplierNumber;
    }

    @Override
    public String toString() {

        return "[ShipmentKey: supplier=" + supplierNumber +
	    " part=" + partNumber + ']';
    }
}
