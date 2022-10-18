cmake_minimum_required(VERSION 3.23)
project(apvcpu C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")

add_executable(apvcpu main.c apv_alu.h apv_types.h apv_alu.c apv_types.c)
include_directories(apvcpu "/usr/local/lib")
target_link_libraries(apvcpu gmp)