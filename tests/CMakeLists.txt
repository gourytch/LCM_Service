cmake_minimum_required(VERSION 3.10)

project(tests)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Boost_DEBUG ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.65 COMPONENTS unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

add_executable(test_empty test_empty.cpp)
add_executable(test_ginac test_ginac.cpp)
target_link_libraries(test_ginac ${GINAC_LIBRARIES})

add_executable(test_calculator test_calculator.cpp)
target_link_libraries(test_calculator ${GINAC_LIBRARIES})

add_test(NAME Test_For_Test COMMAND test_empty)
add_test(NAME Test_GiNaC COMMAND test_ginac)
add_test(NAME Test_Calcuator COMMAND test_calculator)
