cmake_minimum_required(VERSION 3.10)

add_library(calculator STATIC calculator.cpp)

get_filename_component(proto "protos/lcm_calculator.proto" ABSOLUTE)
get_filename_component(proto_path "${proto}" PATH)

set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/lcm_calculator.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/lcm_calculator.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/lcm_calculator.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/lcm_calculator.grpc.pb.h")

add_custom_command(
      OUTPUT "${proto_srcs}"
             "${proto_hdrs}"
             "${grpc_srcs}"
             "${grpc_hdrs}"
      COMMAND ${PROTOC}
      ARGS    --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
              --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
              -I "${proto_path}"
              --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN}"
              "${proto}"
      DEPENDS "${proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

link_libraries(${ProtoBuf_LIBRARIES} ${gRPC_LIBRARIES})
add_executable(client_grpc client_grpc.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(client_grpc
                      ${gRPC_LIBRARIES}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}

                     )

add_executable(server_grpc server_grpc.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(server_grpc
                      calculator
                      ${GINAC_LIBRARIES}
                      ${gRPC_LIBRARIES}
                     )

add_executable(server_libevent server_libevent.cpp)
target_link_libraries(server_libevent
                      calculator
                      ${LibEvent_LIBRARIES}
                      ${GINAC_LIBRARIES})

add_executable(client_libevent client_libevent.cpp)
target_link_libraries(client_libevent
                      calculator
                      ${LibEvent_LIBRARIES}
                      ${GINAC_LIBRARIES})
