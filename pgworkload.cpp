//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "include/pgworkload.h"
#include "include/Client.h"
#include "include/Profile.h"


namespace bpo = boost::program_options;
namespace bpt = boost::posix_time;
namespace btt = boost::this_thread;

// Debug mode?
bool DEBUG = false;

// Global mutex for writing to cout
boost::mutex cout_lock;

// Active threads tally
boost::atomic<int> active_threads;

int main(int argc, const char *argv[])
{
    int threads, operations, scale, think;
    std::string connstr, profile_file;

    active_threads = 0;

    // Command line option handling
    bpo::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "display help message")
            ("connection,c", bpo::value<std::string>(), "PostgreSQL connection string")
            ("debug,d", bpo::value<bool>(), "enable debugging output")
            ("operations,o", bpo::value<int>()->default_value(10), "number of operations per client thread (-1 == infinite)")
            ("profile,p", bpo::value<std::string>(), "filename to load the Profile from")
            ("scale,s", bpo::value<int>()->required(), "pgbench scale factor (required)")
            ("think,i", bpo::value<int>()->default_value(0), "maximum think time in seconds between queries")
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

        // Debugging?
        if (vm.count("debug"))
            DEBUG = true;

        connstr = vm["connection"].as<std::string>();
        threads = vm["threads"].as<int>();
        operations = vm["operations"].as<int>();

        if (vm.count("profile"))
            profile_file = vm["profile"].as<std::string>();

        scale = vm["scale"].as<int>();
        think = vm["think"].as<int>();
        std::cout << "Using " << threads << " threads and " << operations << " operations per thread, with " \
                    << think << " seconds think time." << std::endl;
    }
    catch (const bpo::error &ex)
    {
        std::cerr << ex.what() << '\n';
        std::cout << desc << "\n";
        return 1;
    }

    // Load the Profile if specified
    Profile *profile = new Profile(threads);
    if (!profile->load_profile(profile_file))
        return 1;

    // Create a vector of threads and kick off the clients
    std::vector<boost::thread *> clients;

    // Start the timer
    bpt::ptime start_time = bpt::microsec_clock::local_time();

    for (int i = 0; i < threads; ++i)
    {
        Client * c = new Client(i + 1, scale, operations, think, connstr, profile);
        clients.push_back(new boost::thread(&Client::run, c));
    }

    std::cout << "Running..." << std::endl;

    if (!DEBUG)
        boost::thread t{status};

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


// Display the current status
void status()
{
    while (1)
    {
        btt::sleep_for(boost::chrono::seconds{1});
        std::cout << std::dec << "\rActive threads: " << active_threads << "         " << std::flush;
    }
}