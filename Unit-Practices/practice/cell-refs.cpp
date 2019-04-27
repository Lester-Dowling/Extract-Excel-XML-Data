/**
 * @file   Unit-Practices/practice/cell-refs.cpp
 * @date   Started 2019-04-7
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;

/*
 * Practise and learn how to split up cell refs.
 */

BOOST_AUTO_TEST_SUITE(practice_cell_refs_suite, *utf::disabled())


BOOST_AUTO_TEST_CASE(regex_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){3}.*" };
	const std::string calc{ "[1][2][3] * 6" };
	BOOST_TEST(boost::regex_match(calc, e));
}

BOOST_AUTO_TEST_CASE(regex_iterator_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){2,3}" };
	const std::string calc{ "1 + [1][2][3] * 6" };
	std::string prematched, matched;
	std::string::const_iterator postmatched_begin;
	std::string::const_iterator postmatched_end;
	std::for_each(
	  boost::sregex_iterator{ calc.begin(), calc.end(), e },
	  boost::sregex_iterator{},
	  [&](boost::smatch const& what) //
	  {
		  matched = what[0]; // what[0] contains the whole matched string.
		  prematched = std::string{ what.prefix().first, what.prefix().second };
		  postmatched_begin = what.suffix().first;
		  postmatched_end = what.suffix().second;
	  });
	BOOST_TEST(matched == "[1][2][3]");
	BOOST_TEST(prematched == "1 + ");
	const std::string postmatched{ postmatched_begin, postmatched_end };
	BOOST_TEST(postmatched == " * 6");
}

BOOST_AUTO_TEST_CASE(regex_iterator_replace_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){2,3}" };
	const std::string calc{ "1 + [1][2][3] * 6" };
	std::map<std::string, std::string> values;
	values["[1][2][3]"] = "123";
	std::string replaced;
	std::string::const_iterator postmatched_begin;
	std::for_each(
	  boost::sregex_iterator{ calc.begin(), calc.end(), e },
	  boost::sregex_iterator{},
	  [&](boost::smatch const& what) //
	  {
		  std::string matched = what[0]; // what[0] contains the whole matched string.
		  replaced.append(what.prefix().first, what.prefix().second);
		  replaced.append(values[matched]);
		  postmatched_begin = what.suffix().first;
	  });
	replaced.append(postmatched_begin, calc.cend());
	BOOST_TEST(replaced == "1 + 123 * 6");
}

BOOST_AUTO_TEST_CASE(split_worksheet_cell_ref)
{
	const std::string cell_ref{ "[1][2][3]" };
	std::vector<std::string> cell_ref_ords;
	a::split(cell_ref_ords, cell_ref, a::is_any_of("[]"), a::token_compress_on);
	BOOST_TEST(cell_ref_ords.size() >= 3);
	BOOST_TEST(cell_ref_ords[1] == "1");
	BOOST_TEST(cell_ref_ords[2] == "2");
	BOOST_TEST(cell_ref_ords[3] == "3");
}

BOOST_AUTO_TEST_SUITE_END()
