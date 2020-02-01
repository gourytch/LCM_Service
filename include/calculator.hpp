#pragma once

#include <string>

#include <ginac/numeric.h>

bool is_valid_number(const std::string& value);
 
class Calculator {
public:
    explicit Calculator(const std::string& initial_value = std::string());
    void add_value(const std::string& value);
    std::string get_result() const;
    void reset();
    bool is_valid() const {return _valid;};
private:
    void invalidate();
    GiNaC::numeric _accumulator;
    bool _valid;
};
