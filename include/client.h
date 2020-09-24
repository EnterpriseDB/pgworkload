//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_CLIENT_H
#define PGWORKLOAD_CLIENT_H

#include "pg.h"

class client {
public:
    client(int client, int scale, int operations, const std::string& connstr);
    ~client();

    bool connect();

    void run();
    void transaction();

private:
    int m_client, m_scale, m_operations;
    pgconn *m_conn;
    std::string m_connstr;
};


#endif //PGWORKLOAD_CLIENT_H
