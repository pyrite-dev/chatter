cmake_minimum_required(VERSION 3.11)
project(chatter)

file(GLOB CR_SRC src/*.c)
add_library(cr STATIC ${CR_SRC})
target_include_directories(cr PUBLIC include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(cr PRIVATE DEBUG)
endif()

add_executable(cr-load load.c)
target_link_libraries(cr-load PRIVATE cr)
