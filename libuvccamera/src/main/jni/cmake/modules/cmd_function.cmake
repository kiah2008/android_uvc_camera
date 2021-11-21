#get current dir path
set(project_root_dir ${CMAKE_CURRENT_LIST_DIR}/../..)

include(${CMAKE_CURRENT_LIST_DIR}/inner_function.cmake)

#get api dir
if(ANDROID STREQUAL "1")
	set(platform_libs z log)
else()
	set(platform_libs "")
endif()

macro(set_my_cxx_flags)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++14 -pthread -O3 -Wnon-virtual-dtor  -Wno-unused-parameter")
endmacro()

macro(set_my_shared_link_flags)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wextra -flto")
endmacro()

macro(set_my_exe_link_flags)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -Wextra -flto")
endmacro()


macro(add_my_libs)
    foreach(ARG ${ARGN})
	# MESSAGE("ARG " ${ARG})
    	inner_add_lib(${ARG})
    endforeach()
endmacro()

macro(add_my_includes)
    foreach(ARG ${ARGN})
    		inner_add_include(${ARG})
    endforeach()
endmacro()

