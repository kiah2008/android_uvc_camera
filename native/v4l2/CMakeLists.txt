cmake_minimum_required(VERSION 3.1)
project(v4l2uvc
        VERSION 0.0.6
        LANGUAGES C
        )


if (NOT CMAKE_CONFIGURATION_TYPES)
    if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "No build type selected, default to Release")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif ()
endif ()


set(${PROJECT_NAME}_DESCRIPTION "A cross-platform library for USB video devices")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DANDROID_NDK -DLOG_NDEBUG-O3 -fstrict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_NDK -DLOG_NDEBUG -O3 -fstrict-aliasing")

#add_definitions("-Wall -std=c++14 -DANDROID_NDK -DLOG_NDEBUG -DACCESS_RAW_DESCRIPTORS")
add_definitions("-Wall-DANDROID_NDK ")
set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/v4l2uvc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/avilib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
        )

add_executable(${PROJECT_NAME}
        ${SOURCES}
        )

#set_target_properties(uvccamera PROPERTIES
#  VERSION ${libuvccamera_VERSION}
#  SOVERSION ${libuvccamera_VERSION_MAJOR}
#  # Exported name of target within namespace LibUVC
##  EXPORT_NAME uvccamera
#)

LIST(APPEND ALL_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_BINARY_DIR}
        )

include_directories(
        ${ALL_INCLUDE}
)

add_executable(V4L2UVC::V4L2UVC ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        log
        dl
        m
        android
        )
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
