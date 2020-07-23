/*-
 * Copyright (c) 2010, 2018 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 */
#include "sqliteInt.h"

#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
int sqlite3PcacheReleaseMemory(int nReq){ return nReq; }
#endif
