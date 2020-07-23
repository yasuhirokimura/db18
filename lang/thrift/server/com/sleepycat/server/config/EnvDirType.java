/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server.config;

/**
 * The types of directories configured for environments.
 */
public enum EnvDirType {
    HOME, DATA, LOG, BLOB;

    @Override
    public String toString() {
        return this.name().toLowerCase();
    }
}
