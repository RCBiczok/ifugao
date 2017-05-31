cmake_minimum_required(VERSION 3.5)
project(ifugao)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

find_package(GMP REQUIRED)

set(COMMON_LIBRARIEY ${GMP_LIBRARIES})

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -funroll-loops")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal -Wno-vla")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# Guard against in-source builds and bad build-type strings
#
include(ConfigSafeGuards)

## MAIN ###############################################################################

set(CORE_HEADERS
        include/newick-tools/newick-tools.h
        include/newick-tools/parse_ntree.h
        include/util.h
        include/tree.h
        include/terraces.h
        include/input_parser.h
        include/ifugao.h)

set(CORE_SRCS
        src/newick-tools/lex_ntree.cpp
        src/newick-tools/newick-tools.cpp
        src/newick-tools/ntree.cpp
        src/newick-tools/parse_ntree.cpp
        src/newick-tools/rtree.cpp
        src/ifugao.cpp
        src/input_parser.cpp
        src/terraces.cpp
        src/tree.cpp
        src/util.cpp)

add_library(terraces ${CORE_HEADERS} ${CORE_SRCS})
add_executable(terraces_main src/main.cpp)
target_link_libraries(terraces_main terraces ${COMMON_LIBRARIEY})

## TESTS & BENCHMARK ##################################################################

include(CTest)
enable_testing()

include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)

#set(gtest_disable_pthreads on) #needed in MinGW
download_project(
        PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        UPDATE_DISCONNECTED 1
)
include_directories(SYSTEM "${googletest_SOURCE_DIR}/include")
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

# Disable tests in googlebenchmark
set(BENCHMARK_ENABLE_TESTING OFF)
download_project(
        PROJ                googlebenchmark
        GIT_REPOSITORY      https://github.com/google/benchmark.git
        GIT_TAG             master
        UPDATE_DISCONNECTED 1
)
add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR} EXCLUDE_FROM_ALL)
include_directories(SYSTEM "${googlebenchmark_SOURCE_DIR}/include")

include_directories(test)

set(TEST_HEADERS
        test/algorithm_test.h
        test/input_parser_test.h
        test/terraces_test.h)

add_executable(terraces_tests test/gtest.cpp ${CORE_HEADERS} ${TEST_HEADERS})
target_link_libraries(terraces_tests terraces ${COMMON_LIBRARIEY} gtest gmock)
add_test(terraces_tests terraces_tests)

add_executable(terraces_benchmarks test/gbenchmark.cpp ${CORE_HEADERS} ${TEST_HEADERS})
target_link_libraries(terraces_benchmarks terraces ${COMMON_LIBRARIEY} benchmark)
add_test(terraces_benchmarks terraces_benchmarks)