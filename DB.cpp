//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <libpq-fe.h>

#include "include/pgworkload.h"
#include "include/DB.h"

namespace btt = boost::this_thread;


DB::DB(int client, const std::string& connstr) :
    m_client(client),
    m_connstr(connstr),
    m_conn(NULL)
{

}


DB::~DB()
{
    if (m_conn)
        PQfinish(m_conn);
}


// Connect to the server
bool DB::connect()
{
    const char *cstr = m_connstr.c_str();
    if (cstr != NULL)
    {
        m_conn = PQconnectdb(cstr);

        if (PQstatus(m_conn) != CONNECTION_OK)
        {
            std::string error = std::string(PQerrorMessage(m_conn));
            boost::algorithm::trim(error);
            m_last_error = error;
            cout_lock.lock();
            std::cout << "Client: " << m_client << ", thread: " << btt::get_id() << ", error connecting: " << error << std::endl;
            cout_lock.unlock();

            PQfinish(m_conn);
            m_conn = NULL;
            return false;
        }
    }

    if (m_conn != NULL && DEBUG)
    {
        std::string pid = this->exec_scalar("SELECT pg_backend_pid();");
        std::string version = this->exec_scalar("SELECT version();");
        cout_lock.lock();
        std::cout << "Client: " << m_client << ", thread: " << btt::get_id() << ", backend PID: " << pid << ", version: " << version << std::endl;
        cout_lock.unlock();
    }

    return (m_conn != NULL);
}


// Connect to the server
void DB::disconnect()
{
    if (m_conn)
    {
        PQfinish(m_conn);
        m_conn = NULL;
    }
}


// Execute a query, returning a scalar result
std::string DB::exec_scalar(const std::string& query)
{
    PGresult *res;
    std::string value = "";

    if (DEBUG)
    {
        cout_lock.lock();
        std::cout << "Client: " << m_client << ", executing: " << query << std::endl;
        cout_lock.unlock();
    }

    res = PQexec(m_conn, query.c_str());

    if (PQntuples(res) >= 1 && PQnfields(res) >= 1)
        value = std::string(PQgetvalue(res, 0, 0));

    PQclear(res);

    return value;
}
