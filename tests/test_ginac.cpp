#if !defined(WIN32)
#  define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE GiNaC_Test
#include <boost/test/unit_test.hpp>

#include <string>
#include <sstream>
#include <ginac/ginac.h>


BOOST_AUTO_TEST_SUITE(GiNaCTestSuite)

BOOST_AUTO_TEST_CASE(test_GCD) {
    const GiNaC::numeric a   = 2 * 2 * 2  * 3 * 3  * 5               * 11;
    const GiNaC::numeric b   = 2          * 3      * 5 * 5 * 5  * 7;
    const GiNaC::numeric gcd = 2          * 3      * 5;
    const GiNaC::numeric c_gcd = GiNaC::gcd(a, b);
    BOOST_CHECK_EQUAL(c_gcd, gcd);
}

BOOST_AUTO_TEST_CASE(test_LCM) {
    const GiNaC::numeric a   = 2 * 2 * 2  * 3 * 3  * 5               * 11;
    const GiNaC::numeric b   = 2          * 3      * 5 * 5 * 5  * 7;
    const GiNaC::numeric lcm = 2          * 3      * 5  * 2*2*3*5*5*7*11;
    const GiNaC::numeric c_lcm = GiNaC::lcm(a, b);

    BOOST_CHECK_EQUAL(c_lcm, lcm);
}

BOOST_AUTO_TEST_CASE(test_ConvertFromStringNormal) {
    const char* value = "12345";  // not a string
    const GiNaC::numeric number(value);  // const char*
    BOOST_CHECK_EQUAL(number, GiNaC::numeric(12345));
}

BOOST_AUTO_TEST_CASE(test_ConvertFromStringExponent) {
    const char* value = "1E6";
    const GiNaC::numeric number(value);
    BOOST_CHECK_EQUAL(number, GiNaC::numeric(1000000));
}

BOOST_AUTO_TEST_CASE(test_ConvertBigNumbers) {
    const char* value = "1000000000000000000000";
    const GiNaC::numeric number(value);
    BOOST_CHECK_EQUAL(number, GiNaC::numeric("1E21"));
}

BOOST_AUTO_TEST_CASE(test_ConvertToString) {
    const GiNaC::numeric number(1234567890UL);
    // quick-and-dirty numeric-to-sting converter
    std::stringstream ss;
    ss << number;
    const std::string str = ss.str();
    BOOST_CHECK_EQUAL(str, std::string("1234567890"));
}

BOOST_AUTO_TEST_SUITE_END()
