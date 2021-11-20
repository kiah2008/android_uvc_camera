cmake_minimum_required(VERSION 3.1)
project(coreutils
        VERSION 0.0.1
        LANGUAGES CXX
        )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   -DANDROID_NDK -DLOG_NDEBUG -O3 -fstrict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID_NDK -DLOG_NDEBUG -O3 -fstrict-aliasing")

#add_definitions("-Wall -std=c++14 -DANDROID_NDK -DLOG_NDEBUG -DACCESS_RAW_DESCRIPTORS")
add_definitions("-Wall -std=c++14 -DANDROID_NDK -DUSE_LOGALL  -DACCESS_RAW_DESCRIPTORS")

aux_source_directory(${PROJECT_SOURCE_DIR}/base BASE_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/cpu CPU_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/thread THREAD_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/utils UTIL_SRCS)

set(SOURCES
        ${BASE_SRCS}
        ${UTIL_SRCS}
        ${THREAD_SRCS}
        ${CPU_SRCS}
        )

add_library(${PROJECT_NAME} SHARED ${SOURCES})

#set_target_properties(uvccamera PROPERTIES
#  VERSION ${libuvccamera_VERSION}
#  SOVERSION ${libuvccamera_VERSION_MAJOR}
#  # Exported name of target within namespace LibUVC
##  EXPORT_NAME uvccamera
#)


LIST(APPEND ALL_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_BINARY_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

include_directories(
        ${ALL_INCLUDE}
)

add_library(Cutils::CutilsShared ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        log
        dl
        m
        android
        )

install(
        TARGETS ${PROJECT_NAME}
        EXPORT lib${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION include
        PUBLIC_HEADER DESTINATION include/coreutils
)

install(
        EXPORT lib${PROJECT_NAME}Targets
        FILE lib${PROJECT_NAME}Targets.cmake
        NAMESPACE CoreUtils::
        DESTINATION lib/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)

configure_package_config_file(
        lib${PROJECT_NAME}Config.cmake.in ${PROJECT_BINARY_DIR}/lib${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})

write_basic_package_version_file(
        lib${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})
