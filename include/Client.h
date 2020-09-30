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
#include "Profile.h"

class Client {
public:
    Client(int client, int scale, int operations, int think, const std::string& connstr, Profile *profile);
    ~Client();

    bool connect();
    void disconnect();

    void run();
    void transaction();

private:
    int m_client, m_scale, m_operations, m_think;
    DB *m_conn;
    Profile *m_profile;
    std::string m_connstr;
};


#endif //PGWORKLOAD_CLIENT_H
