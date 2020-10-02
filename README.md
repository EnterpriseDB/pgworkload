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
  -d [ --debug ] arg            enable debugging output
  -o [ --operations ] arg (=10) number of operations per client thread (-1 == 
                                infinite)
  -p [ --profile ] arg          filename to load the Profile from
  -s [ --scale ] arg            pgbench scale factor (required)
  -i [ --think ] arg (=0)       maximum think time in seconds between queries
  -t [ --threads ] arg (=5)     number of client threads

```

For example:

```bash
$ ./pgworkload -s 100 -t 10 -o 10000 -c "port=5435 user=pgworkload password=pgworkload dbname=pgworkload"
```

## Profiles

The profile specifies the percentage of threads that should be active
at any time. A default profile is included in *profile.conf*.

Each non-comment line must contain a single value between 0 and 100,
representing the percentage of users that is the target for the
corresponding point in time over a week long period (168 hours).
At least 4 values must be specified, and at most, 168. The values will be
evenly distributed over the 168 hour timeline, with interpolated values
being generated for each minute using the Catmull-Rom algorithm.

When pgworkload runs, it assumes the start of the profile (the profile
epoch) is on the previous Monday at 00:00:00 (today, if it is Monday).
This allows predictability in the workload, and the ability to stop and
start pgworkload and continue following the profile without starting at
the beginning again.

## Building

pgworkload is a cmake based C++ project. It requires PostgreSQL client 
libraries and the Boost library.

Use *cmake* to configure the build, and then run *make*.

## TODO

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