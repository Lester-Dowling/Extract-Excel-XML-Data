/**
 * @file   Unit-Practices/practice/boost-string.cpp
 * @date   Started 2019-04-30
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;

/*
 * Practise and learn how about boost::string.
 */

BOOST_AUTO_TEST_SUITE(practice_boost_string_suite, *utf::disabled())

BOOST_AUTO_TEST_CASE(split_columns_list)
{
	const string row_titles_column{ "1,  2,  [3]  3" };
	list<string> columns_list;
	a::split(columns_list, row_titles_column, a::is_any_of(", "), a::token_compress_on);
	io::write_sequence(cout, columns_list, "columns_list");
	BOOST_TEST(columns_list.size() == 4);
}

BOOST_AUTO_TEST_SUITE_END()
