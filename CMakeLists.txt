# демонстрационный сервис
# Least Common Multiplier Calculation Service
# реализующий вычисление наименьшего общего кратного
#
cmake_minimum_required(VERSION 3.10)

project(LCM_Service VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GINAC REQUIRED ginac)

set(SOURCES src/*.cpp)

include_directories("src/" ${GINAC_INCLUDE_DIRS})
link_libraries(${GINAC_LIBRARIES})

add_executable(test_empty tests/test_empty.cpp)
add_executable(test_ginac tests/test_ginac.cpp)
add_executable(test_divisors tests/test_lcm.cpp)

