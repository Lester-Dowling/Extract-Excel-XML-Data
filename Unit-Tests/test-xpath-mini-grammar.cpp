/**
 * @file   Unit-Tests/test-xpath-mini-grammar.cpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Pseudo-XPath/mini-grammar.hpp"
#include "Pseudo-XPath/Grade.hpp"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_xpath_mini_grammar_suite, *utf::disabled())

/*
 * Unit tests to verify the correct operation of the pseudo XPath grammar.
 */

namespace fsn = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace phx = boost::phoenix;
using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
using Grade = pseudo_xpath::Grade;
using XPath_Grammar = pseudo_xpath::mini_grammar<Stream_Iterator>;


BOOST_AUTO_TEST_CASE(single_attribute_filter)
{
	const char* const xpath_text = "Cell[ Column=1 ]   ";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Cell");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Column");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_number == 1);
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(Grade::path_to_string(g.result) == "Cell[Column=1]");
}

BOOST_AUTO_TEST_CASE(implicit_attribute_filter_named)
{
	const char* const xpath_text = "Worksheet['Profit and Loss']";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Worksheet");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "Profit and Loss");
	BOOST_TEST(Grade::path_to_string(g.result) == "Worksheet[Worksheet='Profit and Loss']");
}

BOOST_AUTO_TEST_CASE(implicit_attribute_filter_numbered)
{
	const char* const xpath_text = "Worksheet[1]";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Worksheet");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(v->filters().front().good_filter_number);
	BOOST_TEST(Grade::path_to_string(g.result) == "Worksheet[Worksheet=1]");
}

BOOST_AUTO_TEST_CASE(column_path_1)
{
	const char* const xpath_text =
	  "Workbook, Worksheet[1], Table, Row[1],Cell,Data[ss:Type=String]";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);
}

BOOST_AUTO_TEST_CASE(grammar_xml_attribute_filter)
{
	const char* const xpath_text = "  Worksheet   ,   Table   ,   Row[ Row=12 ] , "
								   "Cell[ Column=1  ss::ValueType=Number ]   ";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().empty());

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 2);
	BOOST_TEST(v->name() == "Table");
	BOOST_TEST(v->filters().empty());

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 3);
	BOOST_TEST(v->name() == "Row");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Row");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "12");
	BOOST_TEST(v->filters().front().good_filter_number);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 4);
	BOOST_TEST(v->name() == "Cell");
	BOOST_TEST(v->filters().size() == 2);
	BOOST_TEST(v->filters().front().attribute_name == "Column");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(v->filters().front().good_filter_number);
	BOOST_TEST(v->filters().back().attribute_name == "ss::ValueType");
	BOOST_TEST(v->filters().back().filter_operator == '=');
	BOOST_TEST(v->filters().back().filter_value == "Number");
	BOOST_TEST(!v->filters().back().good_filter_number);

	BOOST_TEST(
	  Grade::path_to_string(g.result) ==
	  "Worksheet --> Table --> Row[Row=12] --> Cell[Column=1 ss::ValueType='Number']");
}

BOOST_AUTO_TEST_SUITE_END()
