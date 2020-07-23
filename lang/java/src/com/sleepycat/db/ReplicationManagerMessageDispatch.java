/*-
 * Copyright (c) 2011, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.db;

public interface ReplicationManagerMessageDispatch {

    void dispatch(ReplicationChannel chan, java.util.Set messages, boolean need_response);	
}
