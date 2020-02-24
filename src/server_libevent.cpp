#include <stdexcept>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstdint>
#include <vector>

#include <event.h>
#include <event2/http.h>

#define SERVER_ADDRESS "0.0.0.0"
#define SERVER_PORT 50005
#define NUM_THREADS 4


int main(int, char**) {
    char const SrvAddress[] = SERVER_ADDRESS;
    std::uint16_t const SrvPort = SERVER_PORT;
    const int SrvThreadCount = NUM_THREADS;
    try {
        void (*OnRequest)(evhttp_request *, void *) = [] (evhttp_request *req, void *) {
            auto *OutBuf = evhttp_request_get_output_buffer(req);
            if (!OutBuf) return;
            evbuffer_add_printf(OutBuf, "<html><body>It works!</body></html>");
            evhttp_send_reply(req, HTTP_OK, "", OutBuf);
        };
        std::exception_ptr InitExcept;
        bool volatile IsRun = true;
        evutil_socket_t Socket = -1;
        auto ThreadFunc = [&] () {
            try {
                std::unique_ptr<event_base, decltype(&event_base_free)> EventBase(event_base_new(), &event_base_free);
                if (!EventBase) throw std::runtime_error("Failed to create new base_event.");
                std::unique_ptr<evhttp, decltype(&evhttp_free)> EvHttp(evhttp_new(EventBase.get()), &evhttp_free);
                if (!EvHttp) throw std::runtime_error("Failed to create new evhttp.");
                evhttp_set_gencb(EvHttp.get(), OnRequest, nullptr);
                if (Socket == -1) {
                    auto *BoundSock = evhttp_bind_socket_with_handle(EvHttp.get(), SrvAddress, SrvPort);
                    if (!BoundSock) throw std::runtime_error("Failed to bind server socket.");
                    if ((Socket = evhttp_bound_socket_get_fd(BoundSock)) == -1)
                        throw std::runtime_error("Failed to get server socket for next instance.");
                } else {
                    if (evhttp_accept_socket(EvHttp.get(), Socket) == -1)
                        throw std::runtime_error("Failed to bind server socket for new instance.");
                }
                while (IsRun) {
                    event_base_loop(EventBase.get(), EVLOOP_NONBLOCK);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            catch (...) {
                InitExcept = std::current_exception();
            }
        }; /* ThreadFunc */
        auto ThreadDeleter = [&] (std::thread *t) {
            IsRun = false;
            t->join();
            delete t;
        };
        typedef std::unique_ptr<std::thread, decltype(ThreadDeleter)> ThreadPtr;
        typedef std::vector<ThreadPtr> ThreadPool;
        ThreadPool Threads;
        for (int i = 0; i < SrvThreadCount; ++i) {
            ThreadPtr Thread(new std::thread(ThreadFunc), ThreadDeleter);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (InitExcept != std::exception_ptr()) {
                IsRun = false;
                std::rethrow_exception(InitExcept);
            }
            Threads.push_back(std::move(Thread));
        }
        std::cout << "Server started... Press [Enter] for quit." << std::endl;
        std::cin.get();
        IsRun = false;
    }
    catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
