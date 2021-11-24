cmake_minimum_required(VERSION 2.8.12)
project(LibUSB C)

set(VERSION 1.0.0)
string(REPLACE "." ";" VERSION_TRIPLET ${VERSION})
list(GET VERSION_TRIPLET 0 VERSION_MAJOR)
list(GET VERSION_TRIPLET 1 VERSION_MINOR)
list(GET VERSION_TRIPLET 2 VERSION_REVISION)

function(pad_number NUMBER OUTPUT_LEN)
  string(LENGTH "${${NUMBER}}" INPUT_LEN)
  if(INPUT_LEN LESS OUTPUT_LEN)
    math(EXPR ZEROES "${OUTPUT_LEN} - ${INPUT_LEN} - 1")
    set(NUM ${${NUMBER}})
    foreach(C RANGE ${ZEROES})
      set(NUM "0${NUM}")
    endforeach()
    set(${NUMBER} ${NUM} PARENT_SCOPE)
  endif()
endfunction()

pad_number(VERSION_MINOR 3)
pad_number(VERSION_REVISION 3)
set(LIBUSB_VERSION_NUMBER ${VERSION_MAJOR}${VERSION_MINOR}${VERSION_REVISION})

string(TIMESTAMP DEFAULT_BUILD "%Y%m%d")
set(BUILD ${DEFAULT_BUILD} CACHE STRING "Build string (default: ${DEFAULT_BUILD})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

macro(debug_message var)
    #message(WARNING "=====  ${var} ${ARGN}")
endmacro()

debug_message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

debug_message("VERSION = ${VERSION}, BUILD = ${BUILD}")

# Detect CPU type and whether we're building 64-bit or 32-bit code
math(EXPR BITS "${CMAKE_SIZEOF_VOID_P} * 8")
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} CMAKE_SYSTEM_PROCESSOR_LC)
set(COUNT 1)
foreach(ARCH ${CMAKE_OSX_ARCHITECTURES})
    if(COUNT GREATER 1)
        message(FATAL_ERROR "The libjpeg-turbo build system does not support multiple values in CMAKE_OSX_ARCHITECTURES.")
    endif()
    math(EXPR COUNT "${COUNT}+1")
endforeach()

if(CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86_64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "amd64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "i[0-9]86" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "x86" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "ia32")
    if(BITS EQUAL 64 OR CMAKE_C_COMPILER_ABI MATCHES "ELF X32")
        set(CPU_TYPE x86_64)
    else()
        set(CPU_TYPE i386)
    endif()
    if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL ${CPU_TYPE})
        set(CMAKE_SYSTEM_PROCESSOR ${CPU_TYPE})
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR_LC STREQUAL "aarch64" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^arm")
    if(BITS EQUAL 64)
        set(CPU_TYPE arm64)
    else()
        set(CPU_TYPE arm)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^ppc" OR
        CMAKE_SYSTEM_PROCESSOR_LC MATCHES "^powerpc")
    set(CPU_TYPE powerpc)
else()
    set(CPU_TYPE ${CMAKE_SYSTEM_PROCESSOR_LC})
endif()
if(CMAKE_OSX_ARCHITECTURES MATCHES "x86_64" OR
        CMAKE_OSX_ARCHITECTURES MATCHES "arm64" OR
        CMAKE_OSX_ARCHITECTURES MATCHES "i386")
    set(CPU_TYPE ${CMAKE_OSX_ARCHITECTURES})
endif()
if(CMAKE_OSX_ARCHITECTURES MATCHES "ppc")
    set(CPU_TYPE powerpc)
endif()
if(MSVC_IDE AND CMAKE_GENERATOR_PLATFORM MATCHES "arm64")
    set(CPU_TYPE arm64)
endif()

###############################################################################
# INSTALL DIRECTORIES
###############################################################################
#if(WIN32)
#    if(MSVC)
#        set(CMAKE_INSTALL_DEFAULT_PREFIX "c:/${CMAKE_PROJECT_NAME}")
#    else()
#        set(CMAKE_INSTALL_DEFAULT_PREFIX "c:/${CMAKE_PROJECT_NAME}-gcc")
#    endif()
#    if(BITS EQUAL 64)
#        set(CMAKE_INSTALL_DEFAULT_PREFIX "${CMAKE_INSTALL_DEFAULT_PREFIX}64")
#    endif()
#else()
#    if(NOT CMAKE_INSTALL_DEFAULT_PREFIX)
#        set(CMAKE_INSTALL_DEFAULT_PREFIX /opt/${CMAKE_PROJECT_NAME})
#    endif()
#endif()
#if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_DEFAULT_PREFIX}" CACHE PATH
#            "Directory into which to install ${CMAKE_PROJECT_NAME} (default: ${CMAKE_INSTALL_DEFAULT_PREFIX})"
#            FORCE)
#endif()

###############################################################################
# CONFIGURATION OPTIONS
###############################################################################

macro(boolean_number var)
    if(${var})
        set(${var} 1 ${ARGN})
    else()
        set(${var} 0 ${ARGN})
    endif()
endmacro()

option(ENABLE_SHARED "Build shared libraries" TRUE)
boolean_number(ENABLE_SHARED)
option(ENABLE_STATIC "Build static libraries" FALSE)
boolean_number(ENABLE_STATIC)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Use the maximum optimization level for release builds
    foreach(var CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${var} MATCHES "-O2")
            string(REGEX REPLACE "-O2" "-O3" ${var} "${${var}}")
        endif()
    endforeach()
endif()

set(CMAKE_C_FLAGS "-fvisibility=hidden -DANDROID_NDK -DLOG_NDEBUG -DACCESS_RAW_DESCRIPTORS ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UC}}")

set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UC}}")

# Include directories and compiler definitions
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(USB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/core.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/descriptor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/hotplug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/io.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/sync.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/strerror.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/android_usbfs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/poll_posix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/threads_posix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/android_netlink.c
        )

LIST(APPEND ALL_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils/
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os
        ${CMAKE_CURRENT_SOURCE_DIR}/android/
        )

include_directories(
        ${ALL_INCLUDE}
)

set(USB_LIBS_DEP log m z)

add_library(LibUSB
        SHARED
        ${USB_SOURCES}
        )

set_target_properties(LibUSB PROPERTIES SOVERSION ${VERSION_MAJOR}
        VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}
        LIBRARY_OUTPUT_NAME "usb1.0"
        )

target_link_libraries(LibUSB
        ${USB_LIBS_DEP}
        )

add_library(LibUSB::LibUSB ALIAS LibUSB)



install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libusb/)

install(TARGETS LibUSB  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${ANDROID_ABI})
