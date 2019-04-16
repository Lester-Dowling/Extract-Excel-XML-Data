#include "pch-unit-tests.hpp"
#define BOOST_TEST_MODULE Excel XML Data Extractor Test Suite
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(main_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(ensure_64_bit) //
{
	BOOST_REQUIRE_EQUAL(sizeof(void*), 8);
#ifndef WIN64
	BOOST_FAIL("x64 is required");
#endif
}

BOOST_AUTO_TEST_SUITE_END()
