/*-
 * Copyright (c) 2006, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file EXAMPLES-LICENSE for license information.
 *
 * $Id$
 */

#include <db_cxx.h>

class SimpleConfigInfo {
public:
    SimpleConfigInfo();
    virtual ~SimpleConfigInfo();

public:
    char* home;
};


SimpleConfigInfo::SimpleConfigInfo()
{
    home = NULL;
}

SimpleConfigInfo::~SimpleConfigInfo()
{
}

