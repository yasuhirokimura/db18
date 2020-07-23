/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server.handle;

import java.io.File;
import java.io.IOException;

/**
 * A EnvironmentKey uniquely identifies an environment.
 */
public class EnvironmentKey extends FileKey {

    public EnvironmentKey(File envHome) throws IOException {
        super(envHome);
    }

}
