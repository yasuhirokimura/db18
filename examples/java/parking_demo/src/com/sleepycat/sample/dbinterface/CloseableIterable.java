/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package com.sleepycat.sample.dbinterface;

/**
 * An interface that combines both Iterable and AutoCloseable.
 * <p/>
 * Only one Iterator object per CloseableIterable object can be open at the
 * same time.
 */
public interface CloseableIterable<T> extends Iterable<T>, AutoCloseable {
}
