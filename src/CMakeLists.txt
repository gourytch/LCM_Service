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
add_executable(client client.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(client
                      ${gRPC_LIBRARIES}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}

                     )

add_executable(server server.cpp ${proto_srcs} ${grpc_srcs})
target_link_libraries(server
                      calculator
                      ${GINAC_LIBRARIES}
                      ${gRPC_LIBRARIES}
                     )
