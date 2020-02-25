cmake_minimum_required(VERSION 3.10)


add_library(calculator STATIC calculator.cpp)

# SERVER
add_executable(server_libevent server_libevent.cpp)

target_compile_options(
    server_libevent
    INTERFACE
    ${GINAC_CFLAGS}
    ${LibEventMT_CFLAGS}
)

target_link_libraries(
    server_libevent
    calculator
    ${GINAC_LIBRARIES}
    ${LibEventMT_LIBRARIES}
    Threads::Threads
)

# CLIENT
add_executable(client_libevent client_libevent.cpp)

target_compile_options(
    client_libevent
    INTERFACE
    ${LibEvent_CFLAGS}
)

target_link_libraries(
    client_libevent
    ${LibEvent_LIBRARIES}
    ${LibEvent_LDFLAGS}
)
