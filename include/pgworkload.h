//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_MAIN_H
#define PGWORKLOAD_MAIN_H

#include <boost/thread.hpp>

extern bool DEBUG;

extern boost::mutex cout_lock;

extern int active_threads;
extern boost::mutex active_threads_lock;


int main(int argc, const char *argv[]);
void status();

#endif //PGWORKLOAD_MAIN_H
