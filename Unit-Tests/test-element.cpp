/**
 * @file   Unit-Tests/test-element.cpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Element.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;
using std::vector;

/*
 * Unit tests to verify the correct operation of simple_xml::Element.
 */

BOOST_AUTO_TEST_SUITE(test_element_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(element_default_ctor)
{
	simple_xml::Element e;
	BOOST_TEST(e.name().empty());
	BOOST_TEST(e.attributes.empty());
	BOOST_TEST(e.text().empty());
	BOOST_TEST(e.children.empty());
	BOOST_TEST(e.row_idx == -1);
	BOOST_TEST(e.col_idx == -1);
	BOOST_TEST(e.wkt_idx == -1);
}

inline vector<char> to_vector(const char* const sample)
{
	return vector<char>{ sample, sample + strlen(sample) };
}

BOOST_AUTO_TEST_CASE(element_name_ctor)
{
	const char* const sample = "element_name";
	simple_xml::Element e{ to_vector(sample) };
	BOOST_TEST(e.name() == sample);
	BOOST_TEST(e.text().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_name_ctor_1)
{
	const char* const sample = "     element_name";
	const char* const trimmed = "element_name";
	simple_xml::Element e{ to_vector(sample) };
	BOOST_TEST(e.name() == trimmed);
	BOOST_TEST(e.text().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_name_ctor_2)
{
	const char* const sample = "element_name         ";
	const char* const trimmed = "element_name";
	simple_xml::Element e{ to_vector(sample) };
	BOOST_TEST(e.name() == trimmed);
	BOOST_TEST(e.text().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_name_ctor_3)
{
	const char* const sample = "     element_name         ";
	const char* const trimmed = "element_name";
	simple_xml::Element e{ to_vector(sample) };
	BOOST_TEST(e.name() == trimmed);
	BOOST_TEST(e.text().empty());
}

BOOST_AUTO_TEST_CASE(element_text_1)
{
	const char* const sample = "Some element text.";
	simple_xml::Element e;
	e.set_text(to_vector(sample));
	BOOST_TEST(e.text() == sample);
	BOOST_TEST(e.name().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_text_1)
{
	const char* const sample = "     Some element text.";
	const char* const trimmed = "Some element text.";
	simple_xml::Element e;
	e.set_text(to_vector(sample));
	BOOST_TEST(e.text() == trimmed);
	BOOST_TEST(e.name().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_text_2)
{
	const char* const sample = "Some element text.         ";
	const char* const trimmed = "Some element text.";
	simple_xml::Element e;
	e.set_text(to_vector(sample));
	BOOST_TEST(e.text() == trimmed);
	BOOST_TEST(e.name().empty());
}

BOOST_AUTO_TEST_CASE(trimmed_element_text_3)
{
	const char* const sample = "     Some element text.         ";
	const char* const trimmed = "Some element text.";
	simple_xml::Element e;
	e.set_text(to_vector(sample));
	BOOST_TEST(e.text() == trimmed);
	BOOST_TEST(e.name().empty());
}

BOOST_AUTO_TEST_SUITE_END()
