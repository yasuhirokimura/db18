/*-
 * Copyright (c) 2001, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 */

package db.repquote;

import java.util.ArrayList;

import com.sleepycat.db.ReplicationHostAddress;
import com.sleepycat.db.ReplicationManagerAckPolicy;
import com.sleepycat.db.ReplicationManagerStartPolicy;
import com.sleepycat.db.ReplicationManagerSiteConfig;

public class RepConfig
{
    /* Constant values used in the RepQuote application. */
    public static final String progname = "RepQuoteExample";
    public static final int CACHESIZE = 10 * 1024 * 1024;
    public static final int SLEEPTIME = 5000;

    /* Member variables containing configuration information. */
    public ReplicationManagerAckPolicy ackPolicy;
    public boolean bulk; /* Whether bulk transfer should be performed. */
    public String home; /* The home directory for rep files. */
     /* Stores an optional set of "other" hosts. */
    private final ArrayList<RepRemoteHost> otherHosts;
    public int priority; /* Priority within the replication group. */
    public ReplicationManagerStartPolicy startPolicy;
    /* Local Host configuration to listen to. */
    private final ReplicationManagerSiteConfig thisHost;
    public boolean verbose;

    /* Member variables used internally. */
    private int currOtherHost;
    private boolean gotListenAddress;

    public RepConfig()
    {
        startPolicy = ReplicationManagerStartPolicy.REP_ELECTION;
        home = "";
        gotListenAddress = false;
        priority = 100;
        verbose = false;
        currOtherHost = 0;
        thisHost = new ReplicationManagerSiteConfig();
        otherHosts = new ArrayList<>();
        ackPolicy = ReplicationManagerAckPolicy.QUORUM;
        bulk = false;
    }

    public java.io.File getHome()
    {
        return new java.io.File(home);
    }

    public void setThisHost(String host, int port, boolean creator)
    {
        gotListenAddress = true;
        thisHost.setHost(host);
        thisHost.setPort(port);
        thisHost.setGroupCreator(creator);
        thisHost.setLocalSite(true);
    }

    public ReplicationManagerSiteConfig getThisHost()
    {
        if (!gotListenAddress)
            System.err.println("Warning: no host specified, returning default.");
        return thisHost;
    }

    public ReplicationHostAddress getThisHostAddress()
    {
        if (!gotListenAddress)
              System.err.println("Warning: no host specified, returning default.");
        return thisHost.getAddress();
      }

    public boolean gotListenAddress() {
        return gotListenAddress;
    }

    public void addOtherHost(String host, int port, boolean peer)
    {
        ReplicationHostAddress newInfo =
            new ReplicationHostAddress(host, port);
        RepRemoteHost newHost = new RepRemoteHost(newInfo, peer);
        otherHosts.add(newHost);
    }

    public RepRemoteHost getFirstOtherHost()
    {
        currOtherHost = 0;
        if (otherHosts.isEmpty())
            return null;
        return otherHosts.get(currOtherHost);
    }

    public RepRemoteHost getNextOtherHost()
    {
        currOtherHost++;
        if (currOtherHost >= otherHosts.size())
            return null;
        return otherHosts.get(currOtherHost);
    }

    public RepRemoteHost getOtherHost(int i)
    {
        if (i >= otherHosts.size())
            return null;
        return otherHosts.get(i);
    }

}
