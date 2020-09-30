//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "include/pgworkload.h"
#include "include/Client.h"
#include "include/DB.h"

namespace btt = boost::this_thread;

Client::Client(int client, int scale, int operations, int think, const std::string& connstr, Profile *profile) :
    m_client(client),
    m_scale(scale),
    m_operations(operations),
    m_think(think),
    m_connstr(connstr),
    m_profile(profile),
    m_conn(NULL)
{

}

Client::~Client()
{
    if (m_conn)
        delete m_conn;
}


// Connect to the server
bool Client::connect()
{
    m_conn = new DB(m_client, m_connstr);
    if (!m_conn->connect())
    {
        std::cout << "Client: " << m_client << ", thread: " << btt::get_id() << ", error connecting: " << \
                m_conn->get_last_error() << ". Exiting." << std::endl;
        return false;
    }

    return true;
}


// Connect to the server
void Client::disconnect()
{
    m_conn->disconnect();
}


// Run the workload, in a loop
void Client::run()
{
    // Initialise the PRNG
    srand((unsigned) time(0));

    long operations = 0;

    // Main loop
    while(1)
    {
        // Only run if the profile says we're not at maximum load for the time
        int workload = m_profile->get_workload();

        // Sleep for a minimum of 1 second, otherwise the maximum sleep time
        int sleep = (m_think == 0) ? 1 : m_think;

        // Lock the thread counter and figure out whether or not to sleep
        active_threads_lock.lock();
        if (active_threads >= workload)
        {
            active_threads_lock.unlock();
            if (DEBUG)
            {
                cout_lock.lock();
                std::cout << "Client: " << std::dec << m_client << ", thread: " << btt::get_id() << \
                            ", sleeping for " << sleep << " seconds because " << active_threads << \
                            " are running and the profile defines " << workload << " at this time." << std::endl;
                cout_lock.unlock();
            }
            btt::sleep(boost::posix_time::seconds(sleep));
            continue;
        }

        // We're moving ahead, so increment the client count and go
        active_threads++;
        active_threads_lock.unlock();

        // Connect, run a transaction, and then disconnect
        if (!this->connect())
            return;

        this->transaction();

        this->disconnect();

        // Decrement the thread counter
        active_threads_lock.lock();
        active_threads--;
        active_threads_lock.unlock();

        // If operations is negative, run indefinitely.
        if (m_operations != -1)
        {
            operations++;
            if (operations >= m_operations)
                break;
        }
    }
}


// Run a single transaction
void Client::transaction()
{
    long aid, bid, tid, delta;
    std::ostringstream query;

    // Random factors
    aid = (rand() % (m_scale * 100000)) + 1;
    bid = (rand() % m_scale) + 1;
    tid = (rand() % (m_scale * 10)) + 1;
    delta = (rand() % 10000) - 5000;

    // Begin the transaction
    m_conn->exec_scalar("BEGIN;");

    // Update the balance of an account
    query.str("");
    query << "UPDATE pgbench_accounts SET abalance = abalance + " << delta << " WHERE aid = " << aid << ";";
    m_conn->exec_scalar(query.str());

    // Think time
    long think = (m_think == 0) ? 0 : (rand() % (m_think * 1000) + 1);
    if (DEBUG)
    {
        cout_lock.lock();
        std::cout << "Client: " << m_client << ", thread: " << btt::get_id() << ", thinking for " << \
                    think << " milliseconds." << std::endl;
        cout_lock.unlock();
    }
    btt::sleep(boost::posix_time::milliseconds(think));

    // Select the account balance
    query.str("");
    query << "SELECT abalance FROM pgbench_accounts WHERE aid = " << aid << ";";
    m_conn->exec_scalar(query.str());

    // Update the teller balance
    query.str("");
    query << "UPDATE pgbench_tellers SET tbalance = tbalance + " << delta << " WHERE tid = " << tid << ";";
    m_conn->exec_scalar(query.str());

    // Update the branch balance
    query.str("");
    query << "UPDATE pgbench_branches SET bbalance = bbalance + " << delta << " WHERE bid = " << bid << ";";
    m_conn->exec_scalar(query.str());

    // Update the history
    query.str("");
    query << "INSERT INTO pgbench_history (tid, bid, aid, delta, mtime) VALUES (" << tid << ", " << bid << ", "
          << aid << ", " << delta << ", CURRENT_TIMESTAMP);";
    m_conn->exec_scalar(query.str());

    // Commit the transaction
    m_conn->exec_scalar("COMMIT;");
}