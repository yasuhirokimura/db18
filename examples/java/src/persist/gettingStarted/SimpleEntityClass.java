/*-
 * Copyright (c) 2008, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package persist.gettingStarted;

import com.sleepycat.persist.model.Entity;
import com.sleepycat.persist.model.PrimaryKey;
import static com.sleepycat.persist.model.Relationship.*;
import com.sleepycat.persist.model.SecondaryKey;

@Entity
public class SimpleEntityClass {

    // Primary key is pKey
    @PrimaryKey
    private String pKey;

    // Secondary key is the sKey
    @SecondaryKey(relate=MANY_TO_ONE)
    private String sKey;

    public void setpKey(String data) {
        pKey = data;
    }

    public void setsKey(String data) {
        sKey = data;
    }

    public String getpKey() {
        return pKey;
    }

    public String getsKey() {
        return sKey;
    }
}
