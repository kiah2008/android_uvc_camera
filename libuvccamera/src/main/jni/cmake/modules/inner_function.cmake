cmake_minimum_required(VERSION 3.6.0)

macro(inner_add_lib)
	SET(module_name ${ARGV0})
	MESSAGE("add lib name:${module_name}")
	if(${module_name} STREQUAL "opencvlibs")
		add_opencv_lib()
	elseif(${module_name} STREQUAL "gtest")
		add_gtest_lib()
	else()
		#ADD_SUBDIRECTORY(${project_root_dir}/library/${module_name} ${CMAKE_CURRENT_BINARY_DIR}/${module_name})
	endif()
endmacro()

macro(inner_add_include)
	SET(module_name ${ARGV0})
	MESSAGE("add include name:${module_name}")
	if(${module_name} STREQUAL "opencv")
		add_opencv_include()
	elseif(${module_name} STREQUAL "gtest")
		add_gtest_include()
	else()
		#INCLUDE_DIRECTORIES(${project_root_dir}/library/${module_name}/include)
	endif()
endmacro()

#=======================================================================================
macro(add_gtest_lib)
#	ADD_SUBDIRECTORY(${project_root_dir}/third_party/googletest ${CMAKE_CURRENT_BINARY_DIR}/gtest)
	INCLUDE(${project_root_dir}/third_party/googletest/prebuilt//gtest.cmake)
endmacro()
macro(add_gtest_include)
	INCLUDE_DIRECTORIES(${project_root_dir}/third_party/googletest/include)
endmacro()

macro(add_opencv_lib)
	#android
	if(ANDROID)
		INCLUDE(${project_root_dir}/third_party/opencv/prebuilt//opencv.cmake)
	else()
		FIND_PACKAGE( OpenCV REQUIRED )
		SET(opencv_all ${OpenCV_LIBS})
	endif()
endmacro()

macro(add_opencv_include)
	#android
	if(ANDROID)
		INCLUDE_DIRECTORIES(${project_root_dir}/third_party/opencv/prebuilt/include)
	else()
		FIND_PACKAGE( OpenCV REQUIRED )
		INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
	endif()
endmacro()
