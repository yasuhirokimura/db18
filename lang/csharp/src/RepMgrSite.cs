/*-
 * Copyright (c) 2009, 2020 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 */
using System;
using System.Collections.Generic;
using System.Text;
using BerkeleyDB.Internal;

namespace BerkeleyDB {
    /// <summary>
    /// A class representing a replication site used by Replication Manager.
    /// </summary>
    public class RepMgrSite {
        
        /// <summary>
        /// Environment ID assigned by the replication manager. This is the same
        /// value that is passed to
        /// <see cref="DatabaseEnvironment.EventNotify"/> for the
        /// <see cref="NotificationEvent.REP_NEWMASTER"/> event.
        /// </summary>
        public int EId;
        /// <summary>
        /// The address of the site.
        /// </summary>
        public ReplicationHostAddress Address;
        /// <summary>
        /// The LSN of the highest log acknowledged as durable by this site.
        /// </summary>
        public LSN maxAcknowledgedLSN;
        /// <summary>
        /// If true, the site is connected.
        /// </summary>
        public bool isConnected;
        /// <summary>
        /// If true, the site can be elected as master.
        /// </summary>
        public bool isElectable;
        /// <summary>
        /// If true, the site is client-to-client peer.
        /// </summary>
        public bool isPeer;
        /// <summary>
        /// If true, the site is a view site.
        /// </summary>
        public bool isView;

        internal RepMgrSite(DB_REPMGR_SITE site) {
            EId = site.eid;
            Address = new ReplicationHostAddress(site.host, site.port);
            maxAcknowledgedLSN = new LSN(site.max_ack_lsn.file, site.max_ack_lsn.offset);
            isConnected = (site.status == DbConstants.DB_REPMGR_CONNECTED);
            isElectable = (site.flags & DbConstants.DB_REPMGR_ISELECTABLE) != 0;
            isPeer = (site.flags & DbConstants.DB_REPMGR_ISPEER) != 0;
            isView = (site.flags & DbConstants.DB_REPMGR_ISVIEW) != 0;
        }
    }
}
