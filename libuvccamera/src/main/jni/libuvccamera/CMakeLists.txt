cmake_minimum_required(VERSION 3.1)
project(libuvccamera
  VERSION 0.0.6
  LANGUAGES C
)

# Xcode and Visual Studio do not using CMAKE_BUILD_TYPE cache variable
# so we use Release build type only with single configuration generators.
if (NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()


macro(debug_message var)
  message(WARNING "=====  ${var} ${ARGN}")
endmacro()

option(BUILD_EXAMPLE "Build example program" OFF)
option(BUILD_TEST "Build test program" OFF)
option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" OFF)

set(libuvccamera_DESCRIPTION "A cross-platform library for USB video devices")

set(CMAKE_C_FLAGS "-DANDROID_NDK -DLOG_NDEBUG -DACCESS_RAW_DESCRIPTORS -O3 -fstrict-aliasing ${CMAKE_C_FLAGS}")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/_onload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utilbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UVCCamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UVCPreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UVCButtonCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UVCStatusCallback.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serenegiant_usb_UVCCamera.cpp
)

add_library(uvccamera SHARED ${SOURCES})

set_target_properties(uvccamera PROPERTIES
  VERSION ${libuvccamera_VERSION}
  SOVERSION ${libuvccamera_VERSION_MAJOR}
  # Exported name of target within namespace LibUVC
#  EXPORT_NAME uvccamera
)


LIST(APPEND ALL_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ../utils/
        ${CMAKE_CURRENT_SOURCE_DIR}/../rapidjson/include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${LibUSB_SOURCE_DIR}/
        ${libjpeg-turbo_SOURCE_DIR}/
        ${libjpeg-turbo_BINARY_DIR}/
        ${libuvc_SOURCE_DIR}/include/
        ${libuvc_SOURCE_DIR}/include/libuvc/
        )

include_directories(
        ${ALL_INCLUDE}
)

add_library(LibUVCCamera::UVCCameraShared ALIAS uvccamera)

target_link_libraries(uvccamera
        PRIVATE
        LibUVC::UVCShared
        LibUSB::LibUSB
        jpeg
        jnigraphics
        log
        android
)
install(
  TARGETS ${UVC_TARGETS}
  EXPORT libuvccameraTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libuvccamera
)
