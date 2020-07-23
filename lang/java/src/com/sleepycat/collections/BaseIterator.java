/*-
 * Copyright (c) 2000, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 */

package com.sleepycat.collections;

import java.util.ListIterator;

/**
 * Common interface for BlockIterator and StoredIterator.  This is an abstract
 * class rather than in interface to prevent exposing these methods in javadoc.
 */
abstract class BaseIterator<E> implements ListIterator<E> {

    abstract ListIterator<E> dup();

    abstract boolean isCurrentData(Object currentData);

    abstract boolean moveToIndex(int index);
}
