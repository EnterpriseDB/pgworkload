//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_PG_H
#define PGWORKLOAD_PG_H

#include <string>

#include <libpq-fe.h>

class pgconn {

public:
    ~pgconn();
    pgconn(int client, const std::string& connstr);

    bool connect();
    std::string exec_scalar(const std::string& query);
    std::string get_last_error() { return m_last_error; };

private:
    int m_client;
    std::string m_connstr, m_last_error;
    PGconn *m_conn;
};


#endif //PGWORKLOAD_PG_H
