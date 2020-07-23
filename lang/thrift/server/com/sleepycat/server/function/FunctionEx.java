/*-
 * Copyright (c) 2002, 2018 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server.function;

/**
 * A functional interface that allows checked exception to be thrown.
 */
@FunctionalInterface
public interface FunctionEx<T, R> {
    R applyWithException(T t) throws Exception;
}
