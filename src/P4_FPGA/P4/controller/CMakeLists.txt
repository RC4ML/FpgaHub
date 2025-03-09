set(CMAKE_CXX_STANDARD 14)

include_directories($ENV{SDE_INSTALL}/include)
link_directories($ENV{SDE_INSTALL}/lib)

#add_executable(hello_bfrt main.cpp my_header.h my_switchd.cpp my_switchd.h my_table.cpp my_table.h)

#target_link_libraries(hello_bfrt -ldriver -lbfutils -lbfsys -lbf_switchd_lib -lpthread)

add_executable(hello_bfrt_test
               main_test.cpp
               my_header.h
               )

target_link_libraries(hello_bfrt_test
                      -lpthread
                      -ldriver
                      -lbfutils
                      -lbfsys
#                      -lbf_switchd_lib
#                      :bfshell_plugin_bf_rt.so
#                      :bfshell_plugin_bf_switchapi.so
#                      :bfshell_plugin_clish.so
#                      :bfshell_plugin_debug.so
#                      :bfshell_plugin_pipemgr.so
#                      -ltofinopdfixed_thrift
#                      -lacctonbf_driver
#                      -lplatform_thrift
#                      -lbf_switch
#                      -lbfdiags
#                      -lcrafter
#                      -ldiag_thrift
#                      -ldru_sim
#                      -lplatform_thrift
#                      -lpltfm_driver
#                      -lpltfm_mgr
#                      -lsai
#                      -ltcl_server
                      )