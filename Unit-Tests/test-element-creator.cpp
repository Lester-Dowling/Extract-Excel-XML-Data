/**
 * @file   Unit-Tests/test-element-creator.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Element-Creator.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(test_element_creator_suite, *utf::disabled())

/*
 * Unit tests to verify the correct operation of simple_xml::Element_Creator.
 */

namespace a = boost::algorithm;
using std::vector;
using std::string;
using simple_xml::Element;
using simple_xml::Element_Creator;

inline vector<char> to_vector(const char* const sample)
{
	return vector<char>{ sample, sample + strlen(sample) };
}


BOOST_AUTO_TEST_CASE(element_creator_ctor)
{
	vector<Element> elements;
	BOOST_TEST(elements.empty());
	Element_Creator creator{ elements };
	BOOST_TEST(creator.depth() == 0);
	BOOST_TEST(creator.path_to_string() == "");
}


BOOST_AUTO_TEST_CASE(new_element_1)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	BOOST_TEST(creator.depth() == 1);
	BOOST_TEST(creator.path_to_string() == "e1");
}


BOOST_AUTO_TEST_CASE(new_element_2)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	const Element::Index e2 = creator.new_element(to_vector("e2"));
	const Element::Index e3 = creator.new_element(to_vector("e3"));
	BOOST_TEST(creator.depth() == 3);
	BOOST_TEST(creator.path_to_string() == "e1 --> e2 --> e3");
}


BOOST_AUTO_TEST_CASE(closing_tag_1)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	const Element::Index e2 = creator.new_element(to_vector("e2"));
	BOOST_TEST(creator.verify_closing_tag(to_vector("e2")));
	const Element::Index e3 = creator.new_element(to_vector("e3"));
	BOOST_TEST(creator.depth() == 2);
	BOOST_TEST(creator.path_to_string() == "e1 --> e3");
	BOOST_TEST(creator.verify_closing_tag(to_vector("e3")));
	BOOST_TEST(creator.path_to_string() == "e1");
}


BOOST_AUTO_TEST_CASE(attribute_1)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	const Element::Index e2 = creator.new_element(to_vector("e2"));
	BOOST_TEST(creator.depth() == 2);
	BOOST_TEST(creator.path_to_string() == "e1 --> e2");
	creator.set_attribute("ss::Type", "Number");
	creator.set_attribute("ss::Format", "Right");
	const Element::Index e3 = creator.new_element(to_vector("e3"));
	BOOST_TEST(creator.depth() == 3);
	BOOST_TEST(creator.path_to_string() == "e1 --> e2 --> e3");
	BOOST_TEST(creator.verify_closing_tag(to_vector("e3")));
	BOOST_TEST(creator.path_to_string() == "e1 --> e2");
	BOOST_TEST(creator.current().attribute("ss::Type").value() == "Number");
	BOOST_TEST(creator.current().attribute("ss::Format").value() == "Right");
}


BOOST_AUTO_TEST_CASE(singleton_1)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	const Element::Index e2 = creator.new_element(to_vector("e2"));
	BOOST_TEST(creator.depth() == 2);
	BOOST_TEST(creator.path_to_string() == "e1 --> e2");
	creator.set_attribute("ss::Type", "Number");
	creator.set_attribute("ss::Format", "Right");
	BOOST_TEST(creator.current().attribute("ss::Type").value() == "Number");
	BOOST_TEST(creator.current().attribute("ss::Format").value() == "Right");
	BOOST_TEST(creator.close_singleton());
	BOOST_TEST(creator.depth() == 1);
	BOOST_TEST(creator.path_to_string() == "e1");
}


BOOST_AUTO_TEST_CASE(text_1)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	const Element::Index e1 = creator.new_element(to_vector("e1"));
	const Element::Index e2 = creator.new_element(to_vector("e2"));
	BOOST_TEST(creator.depth() == 2);
	BOOST_TEST(creator.path_to_string() == "e1 --> e2");
	creator.set_text(to_vector("  some  text   "));
	BOOST_TEST(creator.current().text() == "some  text");
	BOOST_TEST(creator.verify_closing_tag(to_vector("e2")));
	BOOST_TEST(creator.depth() == 1);
	BOOST_TEST(creator.path_to_string() == "e1");
	BOOST_TEST(creator.current().text().empty());
}

BOOST_AUTO_TEST_SUITE_END()
