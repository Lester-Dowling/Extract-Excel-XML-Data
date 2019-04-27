/**
 * @file   Unit-Practices/practice/fusion.cpp
 * @date   Started 2018-08-5
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include "testing-factor/mini-xml-tag.hpp"
#include "testing-factor/opening-tag.hpp"
#include "testing-factor/tag-attributes.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

/*
 * Practise and develop a correct parser for an XML tag.
 */

BOOST_AUTO_TEST_SUITE(practice_fusion_suite, *utf::enabled())
namespace {
	practice::mini_xml_tag trial_mini_xml_tag(const std::string sample_input)
	{
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		typedef std::string::const_iterator Iterator;
		practice::opening_tag<Iterator> g; // Our grammar
		practice::mini_xml_tag result;
		BOOST_REQUIRE(qi::phrase_parse(
		  sample_input.begin(), sample_input.end(), g, ascii::space, result));
		// std::ostringstream oss;
		// oss << "Result: " << std::endl;
		// oss << "    Name       = " << result.name << std::endl;
		// oss << "    Attributes = " << result.attributes << std::endl;
		// BOOST_TEST_MESSAGE(oss.str());
		return result;
	}
} // namespace


BOOST_AUTO_TEST_CASE(verify_mini_xml_tag_1) //
{
	using boost::algorithm::contains;
	auto sample2 = "<bar>";
	auto opening2 = trial_mini_xml_tag(sample2);
	BOOST_REQUIRE_EQUAL(std::string{ "bar" }, opening2.name);
	BOOST_REQUIRE(opening2.attributes == "");
}


BOOST_AUTO_TEST_CASE(verify_mini_xml_tag_2) //
{
	using boost::algorithm::contains;
	auto sample1 = "<bar color=\"white\">";
	auto opening1 = trial_mini_xml_tag(sample1);
	BOOST_REQUIRE_EQUAL(std::string{ "bar" }, opening1.name);
	BOOST_REQUIRE(contains(opening1.attributes, "color"));
	BOOST_REQUIRE(contains(opening1.attributes, "white"));
}


BOOST_AUTO_TEST_CASE(verify_mini_xml_tag_3) //
{
	using boost::algorithm::contains;
	auto sample3 = "<bar color=\"white\"  style=\"bold; italic; underline;\" >";
	auto opening3 = trial_mini_xml_tag(sample3);
	BOOST_REQUIRE_EQUAL(std::string{ "bar" }, opening3.name);
	BOOST_REQUIRE(contains(opening3.attributes, "color"));
	BOOST_REQUIRE(contains(opening3.attributes, "white"));
	BOOST_REQUIRE(contains(opening3.attributes, "style"));
	BOOST_REQUIRE(contains(opening3.attributes, "bold"));
	BOOST_REQUIRE(contains(opening3.attributes, "italic"));
	BOOST_REQUIRE(contains(opening3.attributes, "underline"));
}

BOOST_AUTO_TEST_SUITE_END()
