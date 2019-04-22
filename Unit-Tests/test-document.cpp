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
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(test_document_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of simple_xml::Document.
 */

namespace a = boost::algorithm;
namespace f = boost::filesystem;
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
namespace {

	const f::path samples_dir{
		"C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml"
	};
}


BOOST_AUTO_TEST_CASE(document_ctor)
{
	Document d;
	BOOST_TEST(d.elements().empty());
}


BOOST_AUTO_TEST_CASE(document_load_single_value_worksheet)
{
	Document d{ samples_dir / "single-value-worksheet.xml" };
	BOOST_TEST(d.elements().size() == 40);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	for (auto const& e : d.elements()) {
		if (e.name() == "Data")
			BOOST_TEST(e.text() == "3.14");
	}
}


BOOST_AUTO_TEST_CASE(document_load_single_column_of_values_worksheet)
{
	Document d{ samples_dir / "single-column-of-values-worksheet.xml" };
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


BOOST_AUTO_TEST_CASE(document_worksheet_titles_1)
{
	Document d;
	d.load_xml_file(samples_dir / "small.xml");
	BOOST_TEST(d.elements().size() == 153);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	BOOST_TEST(d.titles().wkt_count() == 2);
	const vector<string> expected_names = { "Profit and Loss", "Balance Sheet" };
	BOOST_TEST(d.titles().wkt_titles() == expected_names);
}


BOOST_AUTO_TEST_CASE(document_column_titles_1)
{
	Document d;
	d.load_xml_file(samples_dir / "small.xml");
	BOOST_TEST(d.elements().size() == 153);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	BOOST_TEST(d.titles().wkt_count() == 2);
	d.extract_column_titles();
	const vector<string> expected_names_wkt_1 = //
	  { "Profit and Loss Item", "06/16", "06/17" };
	BOOST_TEST(d.titles().col_count(1) == 3);
	BOOST_TEST(d.titles().col_titles(1) == expected_names_wkt_1);
	const vector<string> expected_names_wkt_2 = //
	  { "Item", "06/16", "06/17" };
	BOOST_TEST(d.titles().col_count(2) == 3);
	BOOST_TEST(d.titles().col_titles(2) == expected_names_wkt_2);
}


BOOST_AUTO_TEST_CASE(document_row_titles_1)
{
	Document d;
	d.load_xml_file(samples_dir / "small.xml");
	BOOST_TEST(d.elements().size() == 153);
	BOOST_TEST(d.elements().front().name() == "Workbook");
	BOOST_TEST(d.titles().wkt_count() == 2);
	d.extract_column_titles();
	d.extract_row_titles();
	const vector<string> expected_rows_wkt_1 = //
	  { "Operating Revenue", "Other Revenue", "Total Revenue Excluding Interest" };
	BOOST_TEST(d.titles().row_count(1) == 3);
	BOOST_TEST(d.titles().row_titles(1) == expected_rows_wkt_1);
	const vector<string> expected_rows_wkt_2 = //
	  { "CA - Cash",		"CA - Receivables",	   "CA - Prepaid Expenses",
		"CA - Inventories", "CA - Investments",	   "CA - NCA Held Sale",
		"CA - Other",		"Total Current Assets" };
	BOOST_TEST(d.titles().row_count(2) == 8);
	BOOST_TEST(d.titles().row_titles(2) == expected_rows_wkt_2);
}

BOOST_AUTO_TEST_SUITE_END()
