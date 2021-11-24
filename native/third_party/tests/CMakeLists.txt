cmake_minimum_required(VERSION 3.10.0)

project(thrid_party-test )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)

INCLUDE(cmd_function)

set_my_cxx_flags()
set_my_exe_link_flags()

add_my_includes(gtest opencv)

add_my_libs(gtest opencvlibs)

if(ANDROID)
	set ( EXTRA_LIBS z log )
else()
	set ( EXTRA_LIBS "" )
endif()

add_executable(third_test third_test_main.cc)

MESSAGE(STATUS "opencv all libs:${opencv_all}")
target_link_libraries(
	third_test
    ${EXTRA_LIBS}
    ${opencv_LIBRARIES}
	${gtest_LIBRARIES}
    gtest_main
    )
