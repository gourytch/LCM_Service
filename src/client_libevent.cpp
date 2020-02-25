#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include <event2/event.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>

#include "shared.hpp"


const auto DEFAULT_SERVER_URI = "http://localhost:50005/lcm";

#define USE_ROBOT
#define USE_NEVER_ENDING_ROBOT_INPUT
#define USE_NON_ZERO_ROBOT_INPUT

#ifdef USE_ROBOT
std::vector<std::string> getInput() {
    std::vector<std::string> ret;
#ifdef USE_NEVER_ENDING_ROBOT_INPUT
    auto minNums = 1;
#else //USE_NEVER_ENDING_ROBOT_INPUT
    auto minNums = 0;
#endif //USE_NEVER_ENDING_ROBOT_INPUT
    for (auto numNums = minNums + std::rand() % 10; numNums > 0; --numNums) {
        std::string n;
        for (auto numDigits = std::rand() % 10 + 1; numDigits > 0; --numDigits) {
            n += '0' + (std::rand() % 10);
        }
        bool zero = true;
        for (auto a: n) {
            if (a != '0') {
                zero = false;
                break;
            }
        }
        if (zero) {
            n = "1";
        }
        ret.emplace_back(std::move(n));
    }
    return ret;  // empty ::= end of session
}
#else //USE_ROBOT

std::vector<std::string> getInput() {
    std::cout << "Enter some natural numbers delimited by space." << std::endl;
    std::cout << "Empty input means end of work." << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::regex rx("\\s+");
    return {
        std::sregex_token_iterator(input.begin(), input.end(), rx, -1), {}
    };
}
#endif //USE_ROBOT


class Client final {
public:
    explicit Client(const std::string& uri=DEFAULT_SERVER_URI);
    void run();
protected:
    int serverPort;
    std::string serverHost;
    std::string serverPath;

    event_base* base;  // value is valid only when run() is active
    evhttp_connection* connection; // valid only when run() is active
    std::string sessionData;

    void breakTheLoop();
    bool handleInput();
    void performRequest(const Strings& workload);
    void processResult(evhttp_request*);
};

Client::Client(const std::string& uri) {
    std::unique_ptr<evhttp_uri, decltype(&evhttp_uri_free)>
            EvURI(evhttp_uri_parse(uri.c_str()), &evhttp_uri_free);
    if (!EvURI) throw std::runtime_error("uri");
    serverPort = evhttp_uri_get_port(EvURI.get());
    serverHost = std::string(evhttp_uri_get_host(EvURI.get()));
    serverPath = evhttp_uri_get_path(EvURI.get());
    if (serverPort == -1) serverPort = DEFAULT_SERVER_PORT;
    if (serverHost.empty()) serverHost = DEFAULT_SERVER_HOST;
    if (serverPath.empty()) serverPath = PATH_LCM;
    base = nullptr;
    connection = nullptr;
    sessionData = INITIAL_SESSION_VALUE;
}

void Client::run() {
    // create event_base
    std::unique_ptr<event_base, decltype(&event_base_free)>
            EventBase(event_base_new(), &event_base_free);
    if (!EventBase) throw std::runtime_error("EventBase");
    base = EventBase.get();
    // create evdns_base
    auto EvDNSDeleter = [](evdns_base* p) { evdns_base_free(p, 0); };
    std::unique_ptr<evdns_base, decltype(EvDNSDeleter)>
            EvDNSBase(evdns_base_new(EventBase.get(), 1), EvDNSDeleter);
    if (!EvDNSBase) throw std::runtime_error("EvDNSBase");
    // create evhttp_connection
    std::unique_ptr<evhttp_connection, decltype(&evhttp_connection_free)>
            Connection(evhttp_connection_base_new(EventBase.get(),
                                                  EvDNSBase.get(),
                                                  serverHost.c_str(),
                                                  serverPort),
                       &evhttp_connection_free);
    if (!Connection) throw std::runtime_error("Connection");
    connection = Connection.get();
    if (handleInput()) {
        // dispatch and enter the loop
        event_base_dispatch(EventBase.get());
    } // else just quit
}

void Client::breakTheLoop() {
    std::clog << "break the loop" << std::endl;
    event_base_loopbreak(base);
}

void Client::processResult(evhttp_request* req) {
    auto workload = getWorkload(req);
    if (evhttp_request_get_response_code(req) != 200 || workload.empty()) {
        std::cerr << "bad server response." << std::endl;
        breakTheLoop();
        return;
    }
    auto answer = workload.front();
    if (answer == kNaN) {
        std::clog << "bad input sequence." << std::endl;
        breakTheLoop();
        return;
    }
    std::cout << "least common multiplier is: " << answer << std::endl;
    sessionData = getSessionData(req);

    if (!handleInput()) {
        breakTheLoop();
    }
}


bool Client::handleInput() {
    auto input = getInput();
    if (input.empty()) {
        std::cout << "end of sesson requested" << std::endl;
        return false;
    }
    performRequest(input);
    return true;
}

void Client::performRequest(const Strings &workload) {
    auto callback = [](evhttp_request* req, void* arg){
        auto client = reinterpret_cast<Client*>(arg);
        client->processResult(req);
    };
    // create evhttp_request
    auto Request = evhttp_request_new(callback, this);
    if (!Request) throw std::runtime_error("Request");
    // fill request
    evhttp_add_header(Request->output_headers, HEADER_HOST, serverHost.c_str());
    evhttp_add_header(Request->output_headers, HEADER_SESSION, sessionData.c_str());
    auto evbuf = evhttp_request_get_output_buffer(Request);
    for (auto& it: workload) {
        std::cout << "add " << it << std::endl;
        evbuffer_add_printf(evbuf, "%s\n", it.c_str());
    }
    evhttp_make_request(connection, Request, EVHTTP_REQ_POST, serverPath.c_str());
}


int main(int argc, const char **argv) {
    std::srand(std::time(nullptr)); // for roboinput
    auto uri = (argc == 2) ? argv[1] : DEFAULT_SERVER_URI;
    Client(uri).run();
    return 0;
}
