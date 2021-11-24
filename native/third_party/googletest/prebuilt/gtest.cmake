MESSAGE(STATUS "opencv.cmake current dir:${CMAKE_CURRENT_LIST_DIR}")

add_library(gtest STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libgtest.a")

add_library(gmock STATIC IMPORTED)
set_target_properties(gmock PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libgmock.a")

add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest_main PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${ANDROID_ABI}/libgtest_main.a")

set(gtest_INCLUDE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

set(gtest_LIBRARIES
        gtest
        gmock
        gtest_main
        )
