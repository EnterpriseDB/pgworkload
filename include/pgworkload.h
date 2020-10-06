//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_MAIN_H
#define PGWORKLOAD_MAIN_H

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

extern bool DEBUG;

extern boost::mutex cout_lock;

extern boost::atomic<int> active_threads;

int main(int argc, const char *argv[]);
void status();

#endif //PGWORKLOAD_MAIN_H
