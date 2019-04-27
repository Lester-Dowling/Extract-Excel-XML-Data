/**
 * @file   Unit-Practices/practice/parse.cpp
 * @date   Started 2019-04-7
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;
namespace a = boost::algorithm;

/*
 * Practise and check how std can parse.
 */

BOOST_AUTO_TEST_SUITE(practice_parse_suite, *utf::enabled())


BOOST_AUTO_TEST_CASE(parse_number_1)
{
	const char* const input = "123";
	double parsed_number;
	std::istringstream iss{ input };
	iss >> parsed_number;
	BOOST_TEST(parsed_number == 123);
	BOOST_TEST(iss.eof());
}

BOOST_AUTO_TEST_CASE(parse_non_number_1)
{
	const char* const input = "06/17";
	double parsed_number;
	std::istringstream iss{ input };
	iss >> parsed_number;
	BOOST_TEST(!iss.eof());
}

BOOST_AUTO_TEST_CASE(column_name_or_number)
{
	const std::string filter_by_name{ "Name of a column" };
	const std::string filter_by_number{ "6" };
	int filter_col_idx = -1;
	try {
		filter_col_idx = std::stoi(filter_by_name);
		BOOST_FAIL("Unexpectedly parsed text");
	}
	catch (std::invalid_argument const&) {
		BOOST_TEST(filter_col_idx == -1);
	}
}

BOOST_AUTO_TEST_CASE(test_std_parser) // What does std do when parsing a list of doubles?
{
	using namespace std;
	istringstream doubles{ "   9.8   1.9   0.3   8.1  78.99     " };
	doubles.exceptions(ios_base::badbit | ios_base::failbit);
	vector<double> vd;
	vd.reserve(10);
	try {
		vd.insert(
		  vd.end(), istream_iterator<double>{ doubles }, std::istream_iterator<double>{});
	}
	catch (const std::ios_base::failure&) {
		if (doubles.fail()) {
			// BOOST_TEST_MESSAGE("doubles has failbit set");
		}
		if (doubles.bad()) {
			// BOOST_TEST_MESSAGE("doubles has badbit set");
			BOOST_FAIL("Failed to parse doubles");
		}
		if (doubles.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("doubles has eofbit set");
			BOOST_TEST(vd.size() == 5);
			return;
		}
		else
			BOOST_FAIL("Failed to parse doubles");
	}
}

BOOST_AUTO_TEST_SUITE_END()
