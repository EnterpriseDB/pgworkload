# pgworkload

pgworkload is a workload generator for PostgreSQL and EDB Postgres Advanced 
Server. At present it requires the standard schema from pgbench, and will 
execute the standard pgbench workload.

pgworkload is *not* a benchmarking tool. It is intended to simulate a workload
on a Postgres server over time, reflecting increases in user and transaction 
numbers over time, for example, with reduced numbers of users late at night
and at the weekend.

*pgworkload is a work in progress.*

## Usage

Before pgworkload can be used, a Postgres database must be created that has 
been initialised by pgbench, for example:

```bash
$ ./pgbench -i -s 100 -p 5435 -U pgworkload pgworkload
```

Once the database has been setup, pgworkload can be run:

```bash
$ pgworkload --help
Allowed options:
  -h [ --help ]                 display help message
  -c [ --connection ] arg       PostgreSQL connection string
  -o [ --operations ] arg (=10) number of operations per client thread (-1 == 
                                infinite)
  -s [ --scale ] arg            pgbench scale factor (required)
  -t [ --threads ] arg (=5)     number of client threads
```

For example:

```bash
$ ./pgworkload -s 100 -t 10 -o 10000 -c "port=5435 user=pgworkload password=pgworkload dbname=pgworkload"
```

## Building

pgworkload is a cmake based C++ project. It requires PostgreSQL client 
libraries and the Boost library.

Use *cmake* to configure the build, and then run *make*.

## TODO

* Add workload Profile support to vary the number of clients over time.
* Add random think times to the standard workload.
* Add the ability to initialise the schema.
* Add the ability to provide custom workload scripts as a mix of SQL 
queries, variables and expressions 
(e.g. SET foo = random(1,50) ), and control statements (e.g. SLEEP 5).
* Error handling for database operations.
* Exception injection; allow the workload to be varied outside of the 
Profile, to simulate unexpected load or lack thereof.

## Licence

pgworkload is licenced under the PostgreSQL licence.

Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.