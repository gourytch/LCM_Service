#include <string>
#include <sstream>

#include <ginac/ginac.h>

#include "calculator.hpp"

namespace {                    
const std::string kNaN = "NaN";    
}


bool is_valid_number(const std::string& value) {
    for(auto c: value) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}


Calculator::Calculator(const std::string& initial_value) {
    if (initial_value.empty()) {
        reset();
    } else {
        if (is_valid_number(initial_value)) {
            _accumulator = GiNaC::numeric(initial_value.c_str());
            _valid = true;
        } else {
            invalidate();
        }
    }
}

void Calculator::reset() {
    _accumulator = GiNaC::numeric(1);
    _valid = true;
}

void Calculator::invalidate() {
    _accumulator = GiNaC::numeric(1);
    _valid = true;
}

void Calculator::add_value(const std::string& value) {
    if (!is_valid()) return;
    if (is_valid_number(value)) {
        GiNaC::numeric number(value.c_str());
        _accumulator = GiNaC::lcm(_accumulator, number);
    } else {
        invalidate();
    }
}

std::string Calculator::get_result() const {
    if (is_valid()) {
        std::stringstream ss;
        ss << _accumulator;
        return ss.str();
    } else {
        return kNaN;
    }
};
