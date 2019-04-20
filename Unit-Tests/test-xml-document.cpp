/**
 * @file   Unit-Tests/test-xml-document.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "Simple-XML/Document.hpp"
namespace a = boost::algorithm;
namespace f = boost::filesystem;
namespace tt = boost::test_tools;
namespace utf = boost::unit_test;
using std::runtime_error;
using std::string;
using std::vector;
using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;
using simple_xml::Document;
using simple_xml::Element;
using simple_xml::Worksheet_Row_Column_Titles;

/*
 * Unit tests to verify the correct operation of simple_xml::Document.
 */

BOOST_AUTO_TEST_SUITE(test_xml_document_suite, *utf::enabled())
namespace {

	const f::path samples_dir{
		"C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml"
	};
}


BOOST_AUTO_TEST_CASE(document_ctor)
{
	const std::string row_titles_column = "1";
	const int column_titles_row = 1;
	const size_t column_title_span = 1;
	Document d{ row_titles_column, column_titles_row, column_title_span };
	BOOST_TEST(d.elements().empty());
}


BOOST_AUTO_TEST_CASE(document_load_single_value_worksheet)
{
	const std::string row_titles_column = "1";
	const int column_titles_row = 1;
	const size_t column_title_span = 1;
	Document d{ row_titles_column, column_titles_row, column_title_span };
	d.load_xml_file(samples_dir / "single-value-worksheet.xml");
	BOOST_TEST(d.elements().size() == 40);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	for (auto const& e : d.elements()) {
		if (e.name() == "Data")
			BOOST_TEST(e.text() == "3.14");
	}
}


BOOST_AUTO_TEST_CASE(document_load_single_column_of_values_worksheet)
{
	const std::string row_titles_column = "1";
	const int column_titles_row = 1;
	const size_t column_title_span = 1;
	Document d{ row_titles_column, column_titles_row, column_title_span };
	d.load_xml_file(samples_dir / "single-column-of-values-worksheet.xml");
	BOOST_TEST(d.elements().size() == 52);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	const vector<string> expected_values = { "2", "4", "6", "8", "10" };
	vector<string> parsed_values;
	for (auto const& e : d.elements()) {
		if (e.name() == "Data")
			parsed_values.push_back(e.text());
	}
	BOOST_TEST(parsed_values == expected_values);
}


BOOST_AUTO_TEST_CASE(document_worksheet_names_1)
{
	const std::string row_titles_column = "1";
	const int column_titles_row = 1;
	const size_t column_title_span = 1;
	Document d{ row_titles_column, column_titles_row, column_title_span };
	d.load_xml_file(samples_dir / "small.xml");
	d.extract_worksheet_titles();
	BOOST_TEST(d.elements().size() == 154);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	const vector<string> expected_names = { "Profit and Loss", "Balance Sheet" };
	BOOST_TEST(d.titles().wkt_count() == 2);
	BOOST_TEST(d.titles().wkt_titles() == expected_names);
}

BOOST_AUTO_TEST_SUITE_END()
