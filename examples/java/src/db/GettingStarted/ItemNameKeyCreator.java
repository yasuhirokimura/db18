/*-
 * Copyright (c) 2004, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

// File: ItemNameKeyCreator.java

package db.GettingStarted;

import com.sleepycat.bind.tuple.TupleBinding;
import com.sleepycat.db.SecondaryKeyCreator;
import com.sleepycat.db.DatabaseEntry;
import com.sleepycat.db.DatabaseException;
import com.sleepycat.db.SecondaryDatabase;

public class ItemNameKeyCreator<E> implements SecondaryKeyCreator {

    private final TupleBinding<E> theBinding;

    // Use the constructor to set the tuple binding
    ItemNameKeyCreator(TupleBinding<E> binding) {
        theBinding = binding;
    }

    // Abstract method that we must implement
    @Override
    public boolean createSecondaryKey(SecondaryDatabase secDb,
             DatabaseEntry keyEntry,    // From the primary
             DatabaseEntry dataEntry,   // From the primary
             DatabaseEntry resultEntry) // set the key data on this.
         throws DatabaseException {

        if (dataEntry != null) {
            // Convert dataEntry to an Inventory object
            Inventory inventoryItem =
                  (Inventory)theBinding.entryToObject(dataEntry);
            // Get the item name and use that as the key
            String theItem = inventoryItem.getItemName();
            resultEntry.setData(theItem.getBytes());
        }
        return true;
    }
}
