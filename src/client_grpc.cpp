#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>

namespace opts = boost::program_options;


const char *DEFAULT_HOST = "127.0.0.1";
const int DEFAULT_PORT = 7777;

class Client {
public:
    Client(){}
};


static void show_usage_and_exit(const char* appname) {
    std::cerr << "usage: "
            << appname 
            << " server_host [server_port]"
            << std::endl;
    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv) {
    std::string server_host = DEFAULT_HOST;
    int server_port = DEFAULT_PORT;
    if (argc < 2 || argc > 3) show_usage_and_exit(argv[0]);  

    exit(EXIT_SUCCESS);
}
