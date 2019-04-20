/**
 * @file   Unit-Tests/test-element-filter.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Pseudo-XPath-Parser/Grade.hpp"
#include "Pseudo-XPath-Parser/mini-grammar.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "Simple-XML/Element-Filter.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;
namespace fsn = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace phx = boost::phoenix;
using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;
using simple_xml::Element;
using simple_xml::Element_Visitor;
using simple_xml::Element_Filter;
using simple_xml::Worksheet_Row_Column_Titles;
using pseudo_xpath_parser::Grade;
using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
using XPath_Grammar = pseudo_xpath_parser::mini_grammar<Stream_Iterator>;

/*
 * Unit tests to verify the correct operation of simple_xml::Element_Filter.
 */

BOOST_AUTO_TEST_SUITE(test_element_filter_suite, *utf::enabled())


BOOST_AUTO_TEST_CASE(element_filter_ctor_1)
{
	vector<Element> elements;
	Worksheet_Row_Column_Titles t;
	Element_Filter ef{ elements, t };
	BOOST_TEST(ef.depth() == 0);
	BOOST_TEST(ef.path_to_string() == "");
	int counter = 0;
	ef.visit_all_depth_first([&](Element_Visitor&) -> bool {
		++counter;
		return true;
	});
	BOOST_TEST(counter == 0);
}


static Element::Index
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


static Grade::SP parse_pseudo_xpath(const char* const xpath_text)
{
	XPath_Grammar g;
	std::istringstream xpath_stream{ xpath_text };
	xpath_stream.unsetf(std::ios::skipws);
	xpath_stream >> qi::phrase_match(g, ascii::space);
	if (!(xpath_stream.good() || xpath_stream.eof()))
		throw runtime_error{ "Failed to parse pseudo XPath." };
	return g.result;
}


BOOST_AUTO_TEST_CASE(element_filter_depth_first_1)
{
	vector<Element> elements;
	const Element::Index e1 = new_element(elements, "Workbook");
	const Element::Index e2 = new_element(elements, "Worksheet", e1);
	const Element::Index e3 = new_element(elements, "Row", e2);
	const Element::Index e4 = new_element(elements, "Data", e3);
	const Element::Index e5 = new_element(elements, "Data", e3);

	Worksheet_Row_Column_Titles t;
	Element_Filter ef{ elements, t };
	ef.set_filter_path(parse_pseudo_xpath("Workbook,Worksheet,Row,Data"));

	int visit_count = 0;
	ef.visit_all_depth_first(
	  [&](Element_Visitor& v) -> bool //
	  {
		  BOOST_TEST(v.current().name() == "Data");
		  ++visit_count;
		  return true;
	  });
	BOOST_TEST(visit_count == 2);
}

bool contains(list<string> l, string s)
{
	if (std::find(l.begin(), l.end(), s) == l.end())
		return false;
	else
		return true;
}

BOOST_AUTO_TEST_CASE(element_filter_depth_first_2)
{
	vector<Element> elements;
	const Element::Index e1 = new_element(elements, "Workbook");
	const Element::Index e2 = new_element(elements, "Worksheet", e1);
	const Element::Index e3 = new_element(elements, "Row", e2);
	const Element::Index e4 = new_element(elements, "Data", e3);
	const Element::Index e5 = new_element(elements, "Data", e3);
	const Element::Index e6 = new_element(elements, "Row", e2);
	const Element::Index e7 = new_element(elements, "Data", e6);
	const Element::Index e8 = new_element(elements, "Data", e6);
	const Element::Index e9 = new_element(elements, "Data", e6);

	Worksheet_Row_Column_Titles t;
	Element_Filter ef{ elements, t };
	ef.set_filter_path(parse_pseudo_xpath("Workbook,Worksheet,Row"));

	list<string> expected_names = { "Row", "Data", "Data", "Row", "Data", "Data", "Data" };
	list<string> filtered_names;
	ef.visit_all_depth_first(
	  [&](Element_Visitor& v) -> bool //
	  {
		  filtered_names.push_back(v.current().name());
		  return true;
	  });
	BOOST_TEST(filtered_names == expected_names);
}


BOOST_AUTO_TEST_SUITE_END()
