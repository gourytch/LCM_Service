cmake_minimum_required(VERSION 3.10)

project(LCM_Service VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Boost_DEBUG ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.65 COMPONENTS
             program_options
             unit_test_framework
             REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GINAC REQUIRED ginac)
pkg_check_modules(EVENT REQUIRED libevent)
pkg_check_modules(EVENT_PTHREADS REQUIRED libevent_pthreads)

add_library(calculator STATIC calculator.cpp)

add_executable(client client.cpp)
target_link_libraries(client
                      ${EVENT_LIBRARIES}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                     )

add_executable(server server.cpp)
target_link_libraries(server
                      calculator
                      ${GINAC_LIBRARIES}
                      ${EVENT_PTHREADS_LIBRARIES}
                     )
