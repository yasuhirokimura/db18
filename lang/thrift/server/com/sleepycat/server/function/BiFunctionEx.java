/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server.function;

/**
 * A functional interface that takes two arguments and allows checked exception
 * to be thrown.
 */
@FunctionalInterface
public interface BiFunctionEx<T, U, R> {
    R applyWithException(T t, U u) throws Exception;
}
