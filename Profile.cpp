//////////////////////////////////////////////////////////////////////////
//
// pgworkload
//
// Copyright (C) 2020, EnterpriseDB Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/interpolators/catmull_rom.hpp>

#include "include/Profile.h"

namespace bpt = boost::posix_time;


Profile::Profile(int threads) :
    m_threads(threads),
    m_start_time(boost::posix_time::microsec_clock::local_time())
{

}


Profile::~Profile()
{

}


bool Profile::load_profile(const std::string& profile_path)
{
    // If there's no profile path specified, just populate the
    // workload array with values of 100.
    if (profile_path == "")
    {
        for (int i=0; i < TOTAL_POINTS; i++)
            m_profile_data[i] = 100;

        std::cout << "Default workload profile of 100% constant activity loaded." << std::endl;
        return true;
    }

    // Vector to hold the input profile
    std::vector<std::array<double, 2>> points(MAX_CONFIG_POINTS);

    // Open the profile
    std::ifstream inp(profile_path, std::ios::in);
    if (!inp.is_open())
    {
        std::cout << "Error " << errno << " - " << strerror(errno) << ": " << profile_path << std::endl;
        return false;
    }

    // Loop through each line in the profile, and attempt to load the data
    int point_count = 0;
    int line_count = 0;
    std::string line;

    while (getline(inp, line))
    {
        // Ignore comments and blanks
        if (line[0] != '#' && line != "")
        {
            // Attempt to load a double. Ignore any errors, but dump a message
            try
            {
                points[point_count] = {std::stod(line)};
                point_count++;
            }
            catch (const std::exception& e)
            {
                std::cout << "Ignoring invalid value on line " << line_count << ": '" << line << "'" << std::endl;
            }
        }
        line_count++;

        // If we have more data than we want, throw a warning and quit loading
        if (point_count >= MAX_CONFIG_POINTS)
        {
            std::cout << "More than 168 values found. Ignoring additional ones." << std::endl;
            break;
        }
    }

    // We need at least 4 points for Catmull-Rom
    if (point_count < MIN_CONFIG_POINTS)
    {
        std::cout << "At least 4 values must be included in the profile definition. Exiting." << std::endl;
        return false;
    }

    // Trim the vector so it matches the size of what was loaded
    for (int i = MAX_CONFIG_POINTS-1; i >= point_count; i--)
        points.erase(points.begin() + i);

    // Do the Catmull-Rom thing
    boost::math::catmull_rom<std::array<double, 2>> cr(std::move(points));

    // Loop through the values and create an array of the final set of values,
    // interpolating them from the CR graph.
    auto max_s = cr.max_parameter();
    for (int i = 0; i < TOTAL_POINTS; i++)
    {
        double arg = max_s*i/double(TOTAL_POINTS);
        m_profile_data[i] = cr(arg)[0];
    }

    std::cout << "Workload profile loaded from: " << profile_path << std::endl;

    return true;
}


// Return the expected workload in threads at this point in time
double Profile::get_workload()
{
    // Get the time diff
    bpt::ptime current_time = bpt::microsec_clock::local_time();
    bpt::time_duration time_diff = current_time - m_start_time;

    // Figure out what minute to access in the profile data
    int minute = ((time_diff.hours() * 60) + time_diff.minutes()) % TOTAL_POINTS;

    // Get the number of threads we expect
    int threads = m_threads * m_profile_data[minute] / 100;

    return threads;
}
