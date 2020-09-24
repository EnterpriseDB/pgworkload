//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>

#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "include/main.h"
#include "include/client.h"

namespace bpo = boost::program_options;
namespace bpt = boost::posix_time;

// Global mutex for writing to cout
boost::mutex cout_lock;

int main(int argc, const char *argv[])
{
    int threads, operations, scale;
    std::string connstr, workload;

    // Command line option handling
    bpo::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "display help message")
            ("connection,c", bpo::value<std::string>(), "PostgreSQL connection string")
            ("operations,o", bpo::value<int>()->default_value(10), "number of operations per client thread (-1 == infinite)")
            ("scale,s", bpo::value<int>()->required(), "pgbench scale factor (required)")
            ("threads,t", bpo::value<int>()->default_value(5), "number of client threads");

    bpo::variables_map vm;

    try {
        bpo::store(parse_command_line(argc, argv, desc), vm);
        bpo::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 1;
        }

        connstr = vm["connection"].as<std::string>();
        threads = vm["threads"].as<int>();
        operations = vm["operations"].as<int>();
        scale = vm["scale"].as<int>();
        std::cout << "Using " << threads << " threads and " << operations << " operations per thread.\n";
    }
    catch (const bpo::error &ex)
    {
        std::cerr << ex.what() << '\n';
        std::cout << desc << "\n";
        return 1;
    }

    // Create a vector of threads and kick off the clients
    std::vector<boost::thread *> clients;

    // Start the timer
    bpt::ptime start_time = bpt::microsec_clock::local_time();

    for (int i = 0; i < threads; ++i)
    {
        client * c = new client(i + 1, scale, operations, connstr);
        clients.push_back(new boost::thread(&client::run, c));
    }

    for (int i = 0; i < threads; ++i)
    {
        clients[i]->join();
        delete clients[i];
    }

    // Finish timing
    bpt::ptime end_time = bpt::microsec_clock::local_time();
    bpt::time_duration duration = end_time - start_time;

    double transactions = threads * operations;
    double seconds = duration.total_milliseconds() / (double)1000;
    double tps = transactions / (duration.total_milliseconds() / (double)1000);
    std::cout << transactions << " transactions completed in " << \
            seconds << " seconds (" << tps << " transactions per second)." << std::endl;

    return 0;
}
