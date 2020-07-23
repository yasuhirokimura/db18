/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server.function;

/**
 * A supplier interface that allows checked exception to be thrown.
 */
@FunctionalInterface
public interface SupplierEx<T> {
    T getWithException() throws Exception;
}
