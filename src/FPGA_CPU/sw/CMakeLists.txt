cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(rps LANGUAGES CXX VERSION 0.1)

file(READ gen/RPSDummyTop.print RPSDummyTop_print)
file(READ gen/RPSTop.print RPSTop_print)
file(READ gen/RPSClientTop.print RPSClientTop_print)

file(READ gen/RPSDummyTop.fix RPSDummyTop_fix)
file(READ gen/RPSTop.fix RPSTop_fix)
file(READ gen/RPSClientTop.fix RPSClientTop_fix)

configure_file (gen.h.in gen.h)

add_definitions(-mavx512f)
add_compile_options(-O3 -g -march=native)

aux_source_directory(src SRCS)

add_executable(rps
	${SRCS}
	rps.cpp
)

target_link_libraries(rps
	qdma
	pthread
	numa
	gflags
)

add_executable(check_status
	${SRCS}
	check_status.cpp
)

target_link_libraries(check_status
	qdma
	pthread
	gflags
)