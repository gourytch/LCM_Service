#include <array>
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


const auto kNaN = "NaN"; // "Not a Number" value

const auto DEFAULT_SERVER_HOST = "localhost";
const auto DEFAULT_SERVER_PORT = 50005;

const auto HEADER_HOST = "Host";
const auto HEADER_SESSION = "Session";

const auto INITIAL_SESSION_VALUE = "";


std::string readline(evbuffer* evbuf) {
    size_t count;
    auto ptr = evbuffer_readln(evbuf, &count, EVBUFFER_EOL_ANY);
    if (ptr == nullptr) return {};
    return std::string(ptr, ptr + count);
}


std::vector<std::string> getUserInput() {
    std::cout << "enter some natural numbers delimited by space. empty input means end of work" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::regex rx("\\s+");
    return {
        std::sregex_token_iterator(input.begin(), input.end(), rx, -1), {}
    };
}

std::vector<std::string> getRobotInput() {
    std::srand(std::time(nullptr));
    std::vector<std::string> ret;
    for (auto numNumbers = std::rand() % 10; numNumbers > 0; --numNumbers) {
        std::string n;
        for (auto numDigits = std::rand() % 10 + 1; numDigits > 0; --numDigits) {
            n += '0' + (std::rand() % 10);
        }
        ret.emplace_back(std::move(n));
    }
    return ret;  // empty ::= end of session
}

void onSuccess(evhttp_request* req, void* arg) {
    auto evbuf = evhttp_request_get_input_buffer(req);
    std::clog << "got response " << evhttp_request_get_response_code(req)
              << ": " << evhttp_request_get_response_code_line(req) << std::endl;
    auto answer = readline(evbuf);
    std::cout << "ANSWER IS: " << answer << std::endl;
    if (answer == kNaN) {
        std::clog << "break the loop" << std::endl;
        event_base_loopbreak(reinterpret_cast<event_base*>(arg));
        return;
    }
    auto headers = evhttp_request_get_input_headers(req);
    auto sessionHeader = evhttp_find_header(headers, HEADER_SESSION);
    auto sessionData = std::string(sessionHeader ? sessionHeader : "");
    std::clog << "sessiondata <" << sessionData << ">" << std::endl;
    // break for now
    std::clog << "break the loop" << std::endl;
    event_base_loopbreak(reinterpret_cast<event_base*>(arg));
}


void usage(const char* exename) {
    std::cout << "usage: " << exename << " hostname[:port]" << std::endl;
    exit(1);
}


#define __NEW_MAIN__
#ifdef __NEW_MAIN__
int main(int argc, const char **argv) {

    // if (argc != 2) usage(argv[0]);
    const char* arg = (argc == 2) ? argv[1] : "http://localhost:50005/lcm";
    // parse URI
    std::unique_ptr<evhttp_uri, decltype(&evhttp_uri_free)>
            uri(evhttp_uri_parse(arg), &evhttp_uri_free);
    if (!uri) throw std::runtime_error("uri");
    auto serverHost = std::string(evhttp_uri_get_host(uri.get()));
    auto serverPort = evhttp_uri_get_port(uri.get());
    if (serverHost.empty()) serverHost = DEFAULT_SERVER_HOST;
    if (serverPort == -1) serverPort = DEFAULT_SERVER_PORT;

    // create event_base
    std::unique_ptr<event_base, decltype(&event_base_free)>
            EventBase(event_base_new(), &event_base_free);
    if (!EventBase) throw std::runtime_error("EventBase");
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
    // create evhttp_request
    auto Request = evhttp_request_new(onSuccess, EventBase.get());
    if (!Request) throw std::runtime_error("Request");
    // fill request
    evhttp_add_header(Request->output_headers, HEADER_HOST, serverHost.c_str());
    evhttp_add_header(Request->output_headers, HEADER_SESSION, INITIAL_SESSION_VALUE);
    //    auto input = getUserInput();
    auto input = getRobotInput();
    auto evbuf = evhttp_request_get_output_buffer(Request);
    for (auto& it: input) {
        std::cout << "add " << it << std::endl;
        evbuffer_add_printf(evbuf, "%s\n", it.c_str());
    }
    evhttp_make_request(Connection.get(), Request, EVHTTP_REQ_POST, evhttp_uri_get_path(uri.get()));
    // dispatch
    event_base_dispatch(EventBase.get());
    std::cout << "done.";
    return 0;
}
#else
int main(int, const char**) {
    const char* arg = "http://localhost:50005/lcm";
    std::cout << "allocate" << std::endl;
    auto* uri = evhttp_uri_parse(arg);
    auto* base = event_base_new();
    auto* evdns = evdns_base_new(base, 1);
    auto* conn = evhttp_connection_base_new(base, NULL,
                                            evhttp_uri_get_host(uri),
                                            evhttp_uri_get_port(uri));
    auto* rq = evhttp_request_new(onSuccess, base); // released in the depths of libevent
    evhttp_add_header(rq->output_headers, "Host", evhttp_uri_get_host(uri));
    evhttp_make_request(conn, rq, EVHTTP_REQ_POST, evhttp_uri_get_path(uri));
    std::cout << "dispatch" << std::endl;
    event_base_dispatch(base);
    evhttp_connection_free(conn);
    evdns_base_free(evdns, 1);
    event_base_free(base);
    evhttp_uri_free(uri);
    return 0;
}

#endif
