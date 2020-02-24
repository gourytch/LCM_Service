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

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GINAC REQUIRED ginac)
pkg_check_modules(LibEvent REQUIRED libevent)
pkg_check_modules(LibEventMT REQUIRED libevent_pthreads)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
