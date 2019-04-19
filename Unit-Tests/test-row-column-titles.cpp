/**
 * @file   Unit-Tests/test-row-column-titles.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Row-Column-Titles.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;
using std::vector;

/*
 * Unit tests to verify the correct operation of simple_xml::Row_Column_Titles.
 */

BOOST_AUTO_TEST_SUITE(test_row_column_titles_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(rct_default_ctor)
{
	simple_xml::Row_Column_Titles rct;
	BOOST_TEST(rct.row_count() == 0);
	BOOST_TEST(rct.col_count() == 0);
	BOOST_TEST(rct.row_indices().empty());
	BOOST_TEST(rct.col_indices().empty());
	BOOST_TEST(rct.wkt_title().empty());
	BOOST_TEST(!rct.row_title(1).has_value());
	BOOST_TEST(!rct.col_title(1).has_value());
}

BOOST_AUTO_TEST_CASE(rct_name_ctor)
{
	simple_xml::Row_Column_Titles rct{ "worksheet name" };
	BOOST_TEST(rct.wkt_title() == "worksheet name");
}

BOOST_AUTO_TEST_CASE(rct_add)
{
	simple_xml::Row_Column_Titles rct{ "worksheet name" };
	const int row_1_idx = rct.add_row("row title #1");
	const int row_2_idx = rct.add_row("row title #2");
	const int col_1_idx = rct.add_col("col title #1");
	const int col_2_idx = rct.add_col("col title #2");
	const int col_3_idx = rct.add_col("col title #3");
	BOOST_TEST(rct.row_count() == 2);
	BOOST_TEST(rct.col_count() == 3);
	vector<int> expected_row_indices = { row_1_idx, row_2_idx };
	vector<int> expected_col_indices = { col_1_idx, col_2_idx, col_3_idx };
	BOOST_TEST(rct.row_indices() == expected_row_indices);
	BOOST_TEST(rct.col_indices() == expected_col_indices);
	BOOST_TEST(rct.row_title(row_1_idx).value() == "row title #1");
	BOOST_TEST(rct.row_title(row_2_idx).value() == "row title #2");
	BOOST_TEST(rct.col_title(col_1_idx).value() == "col title #1");
	BOOST_TEST(rct.col_title(col_2_idx).value() == "col title #2");
	BOOST_TEST(rct.col_title(col_3_idx).value() == "col title #3");
}


BOOST_AUTO_TEST_SUITE_END()
