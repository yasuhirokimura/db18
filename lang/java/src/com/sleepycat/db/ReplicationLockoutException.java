/*-
 * Copyright (c) 1997, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */
package com.sleepycat.db;

import com.sleepycat.db.internal.DbEnv;

/**
Thrown when an operation was blocked by client/master synchronization.
*/
public class ReplicationLockoutException extends DatabaseException {
    /* package */ ReplicationLockoutException(final String s,
                                   final int errno,
                                   final DbEnv dbenv) {
        super(s, errno, dbenv);
    }
}
