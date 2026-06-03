cmake_minimum_required(VERSION 3.11)
project(chatter)

file(GLOB CR_SRC src/*.c)
add_library(cr STATIC ${CR_SRC})
target_include_directories(cr PUBLIC include)

add_executable(cr-load load.c)
target_link_libraries(cr-load PRIVATE cr)
