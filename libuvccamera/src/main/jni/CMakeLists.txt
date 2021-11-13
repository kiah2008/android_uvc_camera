cmake_minimum_required(VERSION 3.10.0)
project(yuv_libs)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/outputs)

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/libjpeg-turbo)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/libusb)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/libuvc)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/libuvccamera)


