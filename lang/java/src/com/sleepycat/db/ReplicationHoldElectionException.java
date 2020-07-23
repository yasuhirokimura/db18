/*-
 * Copyright (c) 1997, 2018 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */
package com.sleepycat.db;

import com.sleepycat.db.internal.DbEnv;

/**
An election is needed.  The application should call for an election by calling
{@link com.sleepycat.db.Environment#electReplicationMaster Environment.electReplicationMaster}.
*/
public class ReplicationHoldElectionException extends DatabaseException {
    /* package */ ReplicationHoldElectionException(final String s,
                                   final int errno,
                                   final DbEnv dbenv) {
        super(s, errno, dbenv);
    }
}