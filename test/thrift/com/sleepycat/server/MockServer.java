/*-
 * Copyright (c) 2002, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

package com.sleepycat.server;

import org.apache.thrift.server.TServer;

public class MockServer extends TServer {

    public MockServer() {
        super(new TServer.Args(null));
    }

    @Override
    public void serve() {
    }
}
