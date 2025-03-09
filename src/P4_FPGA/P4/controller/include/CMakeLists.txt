cmake_minimum_required(VERSION 3.7)
project(MyP4Runtime)

set(CMAKE_CXX_STANDARD 14)

include_directories($ENV{SDE_INSTALL}/include)
link_directories($ENV{SDE_INSTALL}/lib)

add_library(MyP4Runtime p4_program.cpp p4_program.h p4_table.cpp p4_table.h)

target_link_libraries(MyP4Runtime
                      pthread
                      driver
                      target_sys
                      target_utils
                      )