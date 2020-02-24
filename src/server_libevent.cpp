#include <stdexcept>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstdint>
#include <vector>
#include <array>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <event2/bufferevent.h>

#include <calculator.hpp>


const auto SERVER_ADDRESS = "0.0.0.0";
const auto SERVER_PORT = 50005;
const auto NUM_THREADS = 4;

const auto HEADER_SESSION = "Session";

const auto PATH_LCM = "/lcm";
const auto REASON_OK = "OK";


std::string readline(evbuffer* evbuf) {
    size_t count;
    auto ptr = evbuffer_readln(evbuf, &count, EVBUFFER_EOL_ANY);
    if (ptr == nullptr) return {};
    return std::string(ptr, ptr + count);
}

 void LCMCallback(evhttp_request* req, void*) {
    std::clog << "Request LCM from " << evhttp_request_get_host(req) << std::endl;
    auto headers = evhttp_request_get_input_headers(req);
    auto sessionHeader = evhttp_find_header(headers, HEADER_SESSION);
    auto sessionData = std::string(sessionHeader ? sessionHeader : "");
    std::clog << "sessiondata <" << sessionData << ">" << std::endl;
    auto calc = Calculator(sessionData);
    auto* evbuf = evhttp_request_get_input_buffer(req);
    for (;;) {
        auto value = readline(evbuf);
        if (value.empty()) break;
        std::clog << "add value [" << value << "]" << std::endl;
        calc.add_value(value);
    }
    if (calc.is_valid()) {
        std::clog << "valid result: " << calc.get_result() << std::endl;
    } else {
        std::clog << "invalid sequence received" << std::endl;
    }
    headers = evhttp_request_get_output_headers(req);
    evhttp_add_header(headers, HEADER_SESSION, calc.get_result().c_str());
    evbuf = evhttp_request_get_output_buffer(req);
    evbuffer_add_printf(evbuf, "%s\n", calc.get_result().c_str());
    evhttp_send_reply(req, HTTP_OK, REASON_OK, evbuf);
}

int main(int, char**) {
    try {
        std::exception_ptr InitExcept; // quick and dirty.
        volatile bool IsRun = true;
        evutil_socket_t Socket = -1;
        auto ThreadFunc = [&] () {
            try {
                std::unique_ptr<event_base, decltype(&event_base_free)> EventBase(event_base_new(), &event_base_free);
                if (!EventBase) throw std::runtime_error("EventBase");
                std::unique_ptr<evhttp, decltype(&evhttp_free)> EvHttp(evhttp_new(EventBase.get()), &evhttp_free);
                if (!EvHttp) throw std::runtime_error("evhttp_new");
                evhttp_set_cb(EvHttp.get(), PATH_LCM, LCMCallback, nullptr);
                if (Socket == -1) {
                    auto *BoundSock = evhttp_bind_socket_with_handle(EvHttp.get(), SERVER_ADDRESS, SERVER_PORT);
                    if (!BoundSock) throw std::runtime_error("evhttp_bind_socket_with_handle");
                    if ((Socket = evhttp_bound_socket_get_fd(BoundSock)) == -1)
                        throw std::runtime_error("evhttp_bound_socket_get_fd");
                } else {
                    if (evhttp_accept_socket(EvHttp.get(), Socket) == -1)
                        throw std::runtime_error("evhttp_accept_socket");
                }
                while (IsRun) { // event loop
                    event_base_loop(EventBase.get(), EVLOOP_NONBLOCK);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // yield
                }
            }
            catch (...) { // catch anything, save it and then exit silently.
                InitExcept = std::current_exception();
            }
        }; /* ThreadFunc */
        auto ThreadDeleter = [&](std::thread *t) {
            IsRun = false;
            t->join();
            delete t;
        }; /* ThreadDeleter */

        using ThreadPtr = std::unique_ptr<std::thread, decltype(ThreadDeleter)>;
        using ThreadPool = std::vector<ThreadPtr>;
        ThreadPool Threads;
        for (auto i = 0; i < NUM_THREADS; ++i) {
            std::cout << "start thread " << i << " ..." << std::endl;
            ThreadPtr Thread(new std::thread(ThreadFunc), ThreadDeleter);
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // time to warm up
            if (InitExcept != std::exception_ptr()) {  // something nasty happened
                IsRun = false;
                std::rethrow_exception(InitExcept);
            }
            Threads.push_back(std::move(Thread));
        }
        std::cout << "Server ready. Press [Enter] for quit." << std::endl;
        (void)std::cin.get();
        IsRun = false;
    }
    catch (const std::exception &exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
    std::cout << "Done." << std::endl;
    return 0;
}
