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
using std::string;
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
	BOOST_TEST(v.depth() == 1);
	BOOST_TEST(v.path_to_string() == "root");
	BOOST_TEST(v.current().name() == "root");
	BOOST_TEST(v.current().text() == "some text");
	BOOST_TEST(v.current().children.empty());
	BOOST_TEST(v.current().attributes.empty());
	BOOST_TEST(v.current().col_idx <= 0);
	BOOST_TEST(v.current().row_idx <= 0);
	BOOST_TEST(v.current().wkt_idx <= 0);
	BOOST_TEST(!v.current().attribute("Not there").has_value());
}

inline Element::Index
new_element(vector<Element>& e, string const& n, const Element::Index parent_idx = 0)
{
	// New child:
	const bool new_child_is_root = e.empty();
	e.emplace_back(n);
	Element::Index new_element_index = e.size() - 1;
	if (!new_child_is_root)
		e.at(parent_idx).children.push_back(new_element_index);
	return new_element_index;
}

BOOST_AUTO_TEST_CASE(element_visitor_children_1)
{
	vector<Element> sample;
	const Element::Index e1 = new_element(sample, "e1  ");
	const Element::Index e2 = new_element(sample, "  e2  ", e1);

	Element_Visitor v{ sample };
	BOOST_TEST(v.path_to_string() == "e1");
	BOOST_TEST(v.depth() == 1);
	BOOST_TEST(v.current().name() == "e1");
	BOOST_TEST(!v.current().children.empty());
	BOOST_TEST(v.visit_first_child());
	BOOST_TEST(v.current().name() == "e2");
	BOOST_TEST(v.current().children.empty());
	BOOST_TEST(!v.visit_first_child());
	BOOST_TEST(v.path_to_string() == "e1 --> e2");
	BOOST_TEST(v.depth() == 2);
}


BOOST_AUTO_TEST_CASE(element_visitor_children_2)
{
	vector<Element> sample;
	const Element::Index e1 = new_element(sample, "e1  ");
	const Element::Index e2 = new_element(sample, "  e2  ", e1);
	const Element::Index e3 = new_element(sample, "  e3  ", e1);

	Element_Visitor v{ sample };
	BOOST_TEST(v.depth() == 1);
	BOOST_TEST(v.current().name() == "e1");
	BOOST_TEST(!v.current().children.empty());
	BOOST_TEST(v.visit_first_child());
	BOOST_TEST(v.current().name() == "e2");
	BOOST_TEST(v.visit_next_sibling());
	BOOST_TEST(v.current().name() == "e3");
	BOOST_TEST(v.path_to_string() == "e1 --> e3");
	BOOST_TEST(v.depth() == 2);
}


BOOST_AUTO_TEST_CASE(element_visitor_depth_first_1)
{
	vector<Element> sample;
	const Element::Index e1 = new_element(sample, "e1  ");
	const Element::Index e2 = new_element(sample, "  e2  ", e1);
	const Element::Index e3 = new_element(sample, "  e3  ", e1);
	const Element::Index e4 = new_element(sample, "    e4  ", e3);
	const Element::Index e5 = new_element(sample, "    e5  ", e2);

	Element_Visitor v{ sample };
	int visit_count = 0;
	Element_Visitor::depth_first(
	  sample,
	  [&](Element_Visitor& v) -> bool //
	  {
		  std::cout << v.current().name() << std::endl;
		  ++visit_count;
		  return true;
	  });
	BOOST_TEST(visit_count == 5);
}


BOOST_AUTO_TEST_SUITE_END()
