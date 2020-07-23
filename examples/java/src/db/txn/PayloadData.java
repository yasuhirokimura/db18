/*-
 * Copyright (c) 2005, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package db.txn;

import java.io.Serializable;

public class PayloadData implements Serializable {
    private final int oID;
    private final String threadName;
    private final double doubleData;

    PayloadData(int id, String name, double data) {
        oID = id;
        threadName = name;
        doubleData = data;
    }

    public double getDoubleData() { return doubleData; }
    public int getID() { return oID; }
    public String getThreadName() { return threadName; }
}
