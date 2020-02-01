#if !defined(WIN32)
#  define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE Sample

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(SampleTestSuite)

BOOST_AUTO_TEST_CASE(test_Addition) {
    BOOST_CHECK_EQUAL(2 + 2, 4);
}


BOOST_AUTO_TEST_CASE(test_Multiplication) {
    BOOST_CHECK_EQUAL(2 * 2, 4);
}

BOOST_AUTO_TEST_SUITE_END()
