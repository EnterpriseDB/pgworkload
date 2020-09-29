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

class Profile {

public:
    ~Profile();
    Profile();

    bool load_profile(const std::string& profile_path);

private:
    std::string m_profile_def;
    std::array<double, 2> m_profile_data;
};


#endif //PGWORKLOAD_PROFILE_H
