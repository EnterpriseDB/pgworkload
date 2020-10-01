cmake_minimum_required(VERSION 3.11)
project(pgworkload)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.70.0 COMPONENTS system thread program_options REQUIRED)
find_package(PostgreSQL REQUIRED)

if(Boost_FOUND)
        message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
        message(STATUS "Boost_VERSION: ${Boost_VERSION}")
endif()

add_executable(
        ${PROJECT_NAME}
        pgworkload.cpp
        Client.cpp
        DB.cpp
        Profile.cpp
        include/pgworkload.h
        include/Client.h
        include/DB.h
        include/Profile.h
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${Boost_INCLUDE_DIRS}
        PRIVATE ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(
        ${PROJECT_NAME}
        ${Boost_LIBRARIES}
        ${PostgreSQL_LIBRARIES}
)
