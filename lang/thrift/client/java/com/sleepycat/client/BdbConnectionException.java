/*-
 * Copyright (c) 2002, 2018 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.client;

/**
 * Signals that an error occurred while attempting to connect to a BdbServer.
 */
public class BdbConnectionException extends SDatabaseException {

    public BdbConnectionException(String message, Throwable cause) {
        super(message, cause);
    }

}
