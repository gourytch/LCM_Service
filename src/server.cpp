#include <iostream>
#include <string>

#include <grpc++/grpc++.h>

#include "lcm_calculator.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ClientContext;
using grpc::Status;
using lcm_calculator::SessionRequest;
using lcm_calculator::CalculateRequest;
using lcm_calculator::Response;
using lcm_calculator::Calculator;

class ServiceImpl final: public Calculator::Service {
    Status BeginSession(ServerContext* context,
                        const SessionRequest* request,
                        Response* response) override {
        return Status::OK;
    }
    Status Calculate(ServerContext* context,
                     const CalculateRequest* request,
                     Response* response) override {
        return Status::OK;
    }
    Status EndSession(ServerContext* context,
                     const SessionRequest* request,
                     Response* response) override {
        return Status::OK;
    }
};

void RunServer() {
  std::string server_address("0.0.0.0:50005");
  ServiceImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}


int main(int, char**) {
    RunServer();
    return 0;
}
