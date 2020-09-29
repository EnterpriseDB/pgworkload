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

#include <boost/math/interpolators/catmull_rom.hpp>

#include "include/Profile.h"

const int MIN_CONFIG_POINTS = 4; // Required by Catmull-Rom
const int MAX_CONFIG_POINTS = 168; // 1 week in hours
const int TOTAL_POINTS = 10080; // 1 week in minutes


Profile::Profile()
{

}


Profile::~Profile()
{

}


bool Profile::load_profile(const std::string& profile_path)
{
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
    int i = 0;
    int l = 0;
    std::string line;

    while (getline(inp, line))
    {
        // Ignore comments and blanks
        if (line[0] != '#' && line != "")
        {
            // Attempt to load a double. Ignore any errors, but dump a message
            try
            {
                points[i] = {std::stod(line)};
                i++;
            }
            catch (const std::exception& e)
            {
                std::cout << "Ignoring invalid value on line " << l << ": '" << line << "'" << std::endl;
            }
        }
        l++;

        // If we have more data than we want, throw a warning and quit loading
        if (i >= MAX_CONFIG_POINTS)
        {
            std::cout << "More than 168 values found. Ignoring additional ones." << std::endl;
            break;
        }
    }

    // We need at least 4 points for Catmull-Rom
    if (i < MIN_CONFIG_POINTS)
    {
        std::cout << "At least 4 values must be included in the profile definition. Exiting." << std::endl;
        return false;
    }

    // Trim the vector so it matches the size of what was loaded
    for (int x = MAX_CONFIG_POINTS-1; x >= i; x--)
        points.erase(points.begin() + x);

    // Do the Catmull-Rom thing
    boost::math::catmull_rom<std::array<double, 2>> cr(std::move(points));

    // Loop through the values and create an array of the final set of values,
    // interpolating them from the CR graph.
    auto max_s = cr.max_parameter();
    for (int i = 0; i < TOTAL_POINTS; i++)
    {
        double arg = max_s*i/double(TOTAL_POINTS);
        m_profile_data = cr(arg);
    }

    return true;
}