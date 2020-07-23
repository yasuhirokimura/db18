/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.client;

/**
 * LockNotGrantedException is thrown when lock or transaction timeouts have been
 * configured and a database operation has timed out.
 */
public class SLockNotGrantedException extends SDatabaseException {

    public SLockNotGrantedException(String message, int errno) {
        super(message, errno);
    }

}
