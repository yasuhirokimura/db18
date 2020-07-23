/*-
 * Copyright (c) 2008, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package persist.txn;

import com.sleepycat.persist.model.Entity;
import com.sleepycat.persist.model.PrimaryKey;

@Entity
public class PayloadDataEntity {
    @PrimaryKey
    private int oID;

    private String threadName;

    private double doubleData;

    PayloadDataEntity() {}

    public double getDoubleData() { return doubleData; }
    public int getID() { return oID; }
    public String getThreadName() { return threadName; }

    public void setDoubleData(double dd) { doubleData = dd; }
    public void setID(int id) { oID = id; }
    public void setThreadName(String tn) { threadName = tn; }
}
