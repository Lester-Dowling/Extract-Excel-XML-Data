/**
 * @file   Unit-Tests/test-string-extra.cpp
 * @date   Started 2019-04-10
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "String-Extra/predicates.hpp"
#include "String-Extra/forming.hpp"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_string_extra_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of Program.
 */

namespace a = boost::algorithm;
using std::setw;

BOOST_AUTO_TEST_CASE(is_comma_1)
{
	BOOST_TEST(string_extra::is_comma(','));
	std::string all_commas{ ",,,," };
	BOOST_TEST(std::all_of(all_commas.cbegin(), all_commas.cend(), &string_extra::is_comma));
	std::string any_commas{ "a,bcdef" };
	BOOST_TEST(std::any_of(all_commas.cbegin(), all_commas.cend(), &string_extra::is_comma));
}

BOOST_AUTO_TEST_CASE(remove_commas_1)
{
	BOOST_TEST(string_extra::erase_commas("a,b,c") == "abc");
	BOOST_TEST(string_extra::erase_commas("abc") == "abc");
	BOOST_TEST(string_extra::erase_commas(",abc,") == "abc");
	BOOST_TEST(string_extra::erase_commas(",") == "");
	BOOST_TEST(string_extra::erase_commas(",,,,,,,,,,,") == "");
	BOOST_TEST(string_extra::erase_commas(",,,,,,,,,,,z") == "z");
	BOOST_TEST(string_extra::erase_commas("a,,,,,,,,,,,") == "a");
}

BOOST_AUTO_TEST_CASE(is_quoted_1)
{
	BOOST_TEST(string_extra::is_quoted("'abc'"));
	BOOST_TEST(string_extra::is_quoted("''"));
	BOOST_TEST(!string_extra::is_quoted("123"));
	BOOST_TEST(!string_extra::is_quoted(""));
}

BOOST_AUTO_TEST_CASE(append_quoted_1)
{
	std::string bdr;
	string_extra::append_quoted_if_not_number(bdr, "abc", "unexpected");
	BOOST_TEST(bdr == "'abc'");
	string_extra::append_quoted_if_not_number(bdr, "123", "unexpected");
	BOOST_TEST(bdr == "'abc'123");
	BOOST_REQUIRE_THROW(
	  string_extra::append_quoted_if_not_number(bdr, "", "expected"), std::runtime_error);
	string_extra::append_quoted_if_not_number(bdr, "Z", "unexpected");
	BOOST_TEST(bdr == "'abc'123'Z'");
}

BOOST_AUTO_TEST_SUITE_END()
