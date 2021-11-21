cmake_minimum_required(VERSION 2.6.2)

project(googletest-distribution)

enable_testing()

option(BUILD_GTEST "Builds the googletest subproject" OFF)

#Note that googlemock target already builds googletest
option(BUILD_GMOCK "Builds the googlemock subproject" ON)


if (BUILD_GMOCK)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/googlemock)
endif ()

if (BUILD_GTEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/googletest)
endif ()
