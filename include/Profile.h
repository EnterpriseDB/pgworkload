//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGWORKLOAD_PROFILE_H
#define PGWORKLOAD_PROFILE_H

#include <string>
#include <array>

#include <boost/date_time/posix_time/posix_time.hpp>

const int MIN_CONFIG_POINTS = 4; // Required by Catmull-Rom
const int MAX_CONFIG_POINTS = 168; // 1 week in hours
const int TOTAL_POINTS = 10080; // 1 week in minutes


class Profile {

public:
    ~Profile();
    Profile();

    bool load_profile(const std::string& profile_path);
    double get_workload();

private:
    std::string m_profile_def;
    std::array<double, TOTAL_POINTS> m_profile_data;
    boost::posix_time::ptime m_start_time;
};


#endif //PGWORKLOAD_PROFILE_H
