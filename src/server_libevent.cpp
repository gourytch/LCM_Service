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

#include "calculator.hpp"

#include "shared.hpp"


const auto DEFAULT_NUM_THREADS = 4;
const auto REASON_OK = "OK";


class Server final {
public:
    explicit Server(const std::string& address=DEFAULT_SERVER_HOST,
                    int port=DEFAULT_SERVER_PORT);
    void serve(int numThreads=DEFAULT_NUM_THREADS);

protected:
    std::string address;
    int port;

    void handleLCM(evhttp_request*);
};

Server::Server(const std::string& address, int port) {
    this->address = address;
    this->port = port;
}

void Server::serve(int numThreads) {
    try {
        std::exception_ptr InitExcept; // quick and dirty.
        volatile bool IsRun = true;
        evutil_socket_t Socket = -1;
        auto ThreadFunc = [&] () {
            try {
                std::unique_ptr<event_base, decltype(&event_base_free)>
                        EventBase(event_base_new(), &event_base_free);
                if (!EventBase) throw std::runtime_error("EventBase");
                std::unique_ptr<evhttp, decltype(&evhttp_free)>
                        EvHttp(evhttp_new(EventBase.get()), &evhttp_free);
                if (!EvHttp) throw std::runtime_error("evhttp_new");
                auto CallbackLCM = [](evhttp_request* req, void* ptr) {
                    reinterpret_cast<Server*>(ptr)->handleLCM(req);
                };
                evhttp_set_cb(EvHttp.get(), PATH_LCM, CallbackLCM, this);
                if (Socket == -1) {
                    auto *BoundSock = evhttp_bind_socket_with_handle(
                                EvHttp.get(), address.c_str(), port);
                    if (!BoundSock)
                        throw std::runtime_error("evhttp_bind_socket_with_handle");
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
        for (auto i = 0; i < numThreads; ++i) {
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
}

/*** LCM WORKLOAD ***/
void Server::handleLCM(evhttp_request* req) {
    auto sessionData = getSessionData(req);
    auto workload = getWorkload(req);
    auto calc = Calculator(sessionData);

    for (auto& it: workload) {
        calc.add_value(it);
    }
    setSessionData(req, calc.get_result()); // just keep as a session data too
    auto evbuf = setWorkload(req, calc.get_result());
    evhttp_send_reply(req, HTTP_OK, REASON_OK, evbuf);
}

int main(int, char**) {
    Server().serve();
    std::cout << "Done." << std::endl;
    return 0;
}
