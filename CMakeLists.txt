# демонстрационный сервис
# Least Common Multiplier Calculation Service
# реализующий вычисление наименьшего общего кратного
#
cmake_minimum_required(VERSION 3.10)
project(LCM_Service VERSION 1.0)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "ouput directory - libraries"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "ouput directory - binaries"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "ouput directory - static libs"
    )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(Boost_DEBUG ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.65 COMPONENTS
             program_options
             unit_test_framework
             REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GINAC REQUIRED ginac)
pkg_check_modules(ProtoBuf REQUIRED protobuf)
pkg_check_modules(gRPC REQUIRED grpc++)
pkg_check_modules(LibEvent REQUIRED libevent_pthreads)

find_program(PROTOC protoc)
find_program(gRPC_CPP_PLUGIN grpc_cpp_plugin)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
