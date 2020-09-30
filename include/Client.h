//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_CLIENT_H
#define PGWORKLOAD_CLIENT_H

#include "DB.h"

class Client {
public:
    Client(int client, int scale, int operations, const std::string& connstr);
    ~Client();

    bool connect();
    void disconnect();

    void run();
    void transaction();

private:
    int m_client, m_scale, m_operations;
    DB *m_conn;
    std::string m_connstr;
};


#endif //PGWORKLOAD_CLIENT_H
