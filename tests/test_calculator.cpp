#if !defined(WIN32)
#  define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE GiNaC_Test
#include <boost/test/unit_test.hpp>

#include <string>
#include <sstream>

#include "calculator.hpp"


BOOST_AUTO_TEST_SUITE(CalculatorTestSuite)

BOOST_AUTO_TEST_CASE(test_initial) {
    Calculator calc;
    BOOST_CHECK_EQUAL(calc.is_valid(), true);
    BOOST_CHECK_EQUAL(calc.get_result(), "1");
}

BOOST_AUTO_TEST_CASE(test_assignment) {
    Calculator calc("12321");
    BOOST_CHECK_EQUAL(calc.is_valid(), true);
    BOOST_CHECK_EQUAL(calc.get_result(), "12321");
}

BOOST_AUTO_TEST_CASE(test_invalid_assignment) {
    Calculator calc("bzzz!");
    BOOST_CHECK_EQUAL(calc.is_valid(), false);
    BOOST_CHECK_EQUAL(calc.get_result(), "NaN");
}

BOOST_AUTO_TEST_CASE(test_invalid_signed_assignment_minus) {
    Calculator calc("-12");
    BOOST_CHECK_EQUAL(calc.is_valid(), false);
    BOOST_CHECK_EQUAL(calc.get_result(), "NaN");
}

BOOST_AUTO_TEST_CASE(test_invalid_signed_assignment_plus) {
    Calculator calc("+12");
    BOOST_CHECK_EQUAL(calc.is_valid(), false);
    BOOST_CHECK_EQUAL(calc.get_result(), "NaN");
}

BOOST_AUTO_TEST_CASE(test_invalid_non_integer_assignment) {
    Calculator calc("12.34");
    BOOST_CHECK_EQUAL(calc.is_valid(), false);
    BOOST_CHECK_EQUAL(calc.get_result(), "NaN");
}

BOOST_AUTO_TEST_SUITE_END()
