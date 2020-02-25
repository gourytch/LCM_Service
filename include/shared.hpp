#pragma once
// this is header-only part common for client and server

#include <string>
#include <vector>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/http.h>


const auto DEFAULT_SERVER_HOST = "localhost";
const auto DEFAULT_SERVER_PORT = 50005;

const auto HEADER_HOST = "Host";
const auto HEADER_SESSION = "Session";
const auto PATH_LCM = "/lcm";

const auto INITIAL_SESSION_VALUE = "";
const auto kNaN = "NaN"; // "Not a Number" value


using Strings = std::vector<std::string>;


std::string readline(evbuffer* evbuf) {
    size_t count;
    auto ptr = evbuffer_readln(evbuf, &count, EVBUFFER_EOL_ANY);
    if (ptr == nullptr) return {};
    return std::string(ptr, ptr + count);
}


std::string getSessionData(evhttp_request* req) {
    auto headers = evhttp_request_get_input_headers(req);
    auto sessionHeader = evhttp_find_header(headers, HEADER_SESSION);
    return std::string(sessionHeader ? sessionHeader : "");
}


void setSessionData(evhttp_request* req,
                           const std::string& data) {
    auto headers = evhttp_request_get_output_headers(req);
    evhttp_add_header(headers, HEADER_SESSION, data.c_str());
}


std::vector<std::string> getWorkload(evhttp_request* req) {
    auto* evbuf = evhttp_request_get_input_buffer(req);
    std::vector<std::string> result;
    for (;;) {
        auto value = readline(evbuf);
        if (value.empty()) break;
        result.emplace_back(std::move(value));
    }
    return result;
}

evbuffer* setWorkload(evhttp_request* req, const std::string& value) {
    auto evbuf = evhttp_request_get_output_buffer(req);
    evbuffer_add_printf(evbuf, "%s\n", value.c_str());
    return evbuf;
}

evbuffer* setWorkload(evhttp_request* req,
                      const std::vector<std::string>& values) {
    auto evbuf = evhttp_request_get_output_buffer(req);
    for (auto& it: values) {
        evbuffer_add_printf(evbuf, "%s\n", it.c_str());
    }
    return evbuf;
}
