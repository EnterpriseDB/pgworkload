//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_DB_H
#define PGWORKLOAD_DB_H

#include <string>

#include <libpq-fe.h>

class DB {

public:
    ~DB();
    DB(int client, const std::string& connstr);

    bool connect();
    void disconnect();

    std::string exec_scalar(const std::string& query);
    std::string get_last_error() { return m_last_error; };

private:
    int m_client;
    std::string m_connstr, m_last_error;
    PGconn *m_conn;
};


#endif //PGWORKLOAD_DB_H
