#add_custom_target(p4_build # for sde version before 9.6.0
#                  COMMAND
#                  P4_NAME=test $ENV{SDE}/p4_build.sh ${CMAKE_CURRENT_SOURCE_DIR}/test.p4)

set(CMAKE_INSTALL_PREFIX $ENV{SDE_INSTALL})
set(CMAKE_MODULE_PATH    $ENV{SDE}/cmake)

set(P4_NAME P4AllReduce)
set(P4_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test.p4)

set(P4_BUILD_DIRECTORY p4build)

add_subdirectory($ENV{SDE}/p4studio/ ${P4_BUILD_DIRECTORY} EXCLUDE_FROM_ALL)

add_custom_target(p4_build # for SDE version after 9.6.0
                  COMMAND make clean && make install
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${P4_BUILD_DIRECTORY})

#add_custom_target(p4_insight
#                  COMMAND su -c "xvfb-run $ENV{SDE_INSTALL}/bin/p4i" alex
#                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})