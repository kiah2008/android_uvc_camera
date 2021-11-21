cmake_minimum_required(VERSION 3.6.0)
set(CMAKE_VERBOSE_MAKEFILE ON)

include(ExternalProject)

SET(OPENCV_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv)

ExternalProject_Add(OPENCV_PREBUILT
        URL "https://github.com/opencv/opencv/releases/download/4.2.0/opencv-4.2.0-android-sdk.zip"
        URL_HASH MD5=a2b4df1b867f0d2bd73fc33158c30b79
        PREFIX ${OPENCV_SRC_PATH}
        CONFIGURE_COMMAND unzip -f ${OPENCV_SRC_PATH}/src/opencv-4.2.0-android-sdk.zip
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        )

SET(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
find_package(OpenCV REQUIRED)

foreach(lib_ ${OpenCV_LIBS})
    add_dependencies(${lib_} OPENCV_PREBUILT)
endforeach()

# file(MAKE_DIRECTORY ${OpenCV_INCLUDE_DIRS})

if(OpenCV_FOUND)
    message("========== libs ${OpenCV_LIBS}")
    message("========== headers ${OpenCV_INCLUDE_DIRS}")
endif()