/**
 * @file   Unit-Tests/test-element-visitor.cpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Element-Visitor.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;
using std::vector;
using simple_xml::Element;
using simple_xml::Element_Visitor;

/*
 * Unit tests to verify the correct operation of simple_xml::Element_Visitor.
 */

BOOST_AUTO_TEST_SUITE(test_element_visitor_suite, *utf::enabled())


inline vector<char> to_vector(const char* const sample)
{
	return vector<char>{ sample, sample + strlen(sample) };
}


BOOST_AUTO_TEST_CASE(element_visitor_ctor)
{
	vector<Element> sample;
	sample.emplace_back("root", "  some text    ");
	BOOST_TEST(!sample.empty());

	Element_Visitor v{ sample };
	BOOST_TEST(v.depth() == 0);
	BOOST_TEST(v.path_to_string() == "root");
	BOOST_TEST(v.name() == "root");
	BOOST_TEST(v.text() == "some text");
	BOOST_TEST(v.children().empty());
	BOOST_TEST(v.attributes().empty());
	BOOST_TEST(v.col_idx() <= 0);
	BOOST_TEST(v.row_idx() <= 0);
	BOOST_TEST(v.wkt_idx() <= 0);
	BOOST_TEST(!v.attribute("Not there").has_value());
}


BOOST_AUTO_TEST_SUITE_END()
