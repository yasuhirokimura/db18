/*-
 * Copyright (c) 1997, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package db.repquote;

import com.sleepycat.db.ReplicationHostAddress;

public class RepRemoteHost{
    private final ReplicationHostAddress addr;
    private final boolean isPeer;

    public RepRemoteHost(ReplicationHostAddress remoteAddr, boolean hostIsPeer){
        addr = remoteAddr;
        isPeer = hostIsPeer;
    }

    public ReplicationHostAddress getAddress(){
        return addr;
    }

    public boolean isPeer(){
        return isPeer;
    }
}
