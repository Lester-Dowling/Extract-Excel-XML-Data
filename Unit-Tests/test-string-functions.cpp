// Unit-Tests/test-string-functions.cpp
// Started 10 Apr 2019
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Operations/string-functions.hpp"
namespace utf = boost::unit_test;
namespace a = boost::algorithm;
using std::setw;

/*
 * Unit tests to verify the correct operation of Program.
 */

BOOST_AUTO_TEST_SUITE(test_string_functions_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(is_comma_1)
{
	BOOST_TEST(operations::is_comma(','));
	std::string all_commas{ ",,,," };
	BOOST_TEST(std::all_of(all_commas.cbegin(), all_commas.cend(), &operations::is_comma));
	std::string any_commas{ "a,bcdef" };
	BOOST_TEST(std::any_of(all_commas.cbegin(), all_commas.cend(), &operations::is_comma));
}

BOOST_AUTO_TEST_CASE(remove_commas_1)
{
	BOOST_TEST(operations::erase_commas("a,b,c") == "abc");
	BOOST_TEST(operations::erase_commas("abc") == "abc");
	BOOST_TEST(operations::erase_commas(",abc,") == "abc");
	BOOST_TEST(operations::erase_commas(",") == "");
	BOOST_TEST(operations::erase_commas(",,,,,,,,,,,") == "");
	BOOST_TEST(operations::erase_commas(",,,,,,,,,,,z") == "z");
	BOOST_TEST(operations::erase_commas("a,,,,,,,,,,,") == "a");
}

BOOST_AUTO_TEST_CASE(is_quoted_1)
{
	BOOST_TEST(operations::is_quoted("'abc'"));
	BOOST_TEST(operations::is_quoted("''"));
	BOOST_TEST(!operations::is_quoted("123"));
	BOOST_TEST(!operations::is_quoted(""));
}

BOOST_AUTO_TEST_CASE(append_quoted_1)
{
	std::string bdr;
	operations::append_quoted_if_not_number(bdr, "abc", "unexpected");
	BOOST_TEST(bdr == "'abc'");
	operations::append_quoted_if_not_number(bdr, "123", "unexpected");
	BOOST_TEST(bdr == "'abc'123");
	BOOST_REQUIRE_THROW(
	  operations::append_quoted_if_not_number(bdr, "", "expected"), std::runtime_error);
	operations::append_quoted_if_not_number(bdr, "Z", "unexpected");
	BOOST_TEST(bdr == "'abc'123'Z'");
}

BOOST_AUTO_TEST_SUITE_END()
