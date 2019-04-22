/**
 * @file   Unit-Tests/test-worksheet-row-column-titles.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(test_worksheet_row_column_titles_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of simple_xml::Worksheet_Row_Column_Titles.
 */

namespace a = boost::algorithm;
using std::vector;

BOOST_AUTO_TEST_CASE(wrct_default_ctor)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	BOOST_TEST(t.size() == 0);
	BOOST_REQUIRE_NO_THROW(t.throw_if_non_existing_wkt(1));
}

BOOST_AUTO_TEST_CASE(wrct_add_worksheet_1)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	t.add_worksheet("one");
	t.add_worksheet("two");
	t.add_worksheet("three");
	BOOST_TEST(t.size() == 3);
	BOOST_REQUIRE_NO_THROW(t.throw_if_existing_wkt(1));
	BOOST_REQUIRE_NO_THROW(t.throw_if_existing_wkt(2));
	BOOST_REQUIRE_NO_THROW(t.throw_if_existing_wkt(3));
	BOOST_TEST(t.wkt_title(1).value() == "one");
	BOOST_TEST(t.wkt_title(2).value() == "two");
	BOOST_TEST(t.wkt_title(3).value() == "three");
	t.clear();
	BOOST_TEST(t.size() == 0);
}

BOOST_AUTO_TEST_CASE(wrct_add_rows_columns_1)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	BOOST_TEST(t.add_worksheet("one") == 1);
	BOOST_TEST(t.add_worksheet("two") == 2);
	BOOST_TEST(t.add_worksheet("three") == 3);

	const int w_1_r_1 = t.add_row(1, "w #1 r #1");
	const int w_1_r_2 = t.add_row(1, "w #1 r #2");
	const int w_1_r_3 = t.add_row(1, "w #1 r #3");

	const int w_2_r_1 = t.add_row(2, "w #2 r #1");
	const int w_2_r_2 = t.add_row(2, "w #2 r #2");
	const int w_2_r_3 = t.add_row(2, "w #2 r #3");
	const int w_2_r_4 = t.add_row(2, "w #2 r #4");

	const int w_1_c_1 = t.add_col(1, "w #1 c #1");
	const int w_1_c_2 = t.add_col(1, "w #1 c #2");
	const int w_1_c_3 = t.add_col(1, "w #1 c #3");
	const int w_1_c_4 = t.add_col(1, "w #1 c #4");

	const int w_2_c_1 = t.add_col(2, "w #2 c #1");
	const int w_2_c_2 = t.add_col(2, "w #2 c #2");
	const int w_2_c_3 = t.add_col(2, "w #2 c #3");
	const int w_2_c_4 = t.add_col(2, "w #2 c #4");
	const int w_2_c_5 = t.add_col(2, "w #2 c #5");

	BOOST_TEST(t.row_title(1, 1).value() == "w #1 r #1");
	BOOST_TEST(t.row_title(1, 2).value() == "w #1 r #2");
	BOOST_TEST(t.row_title(1, 3).value() == "w #1 r #3");

	BOOST_TEST(t.row_title(2, 1).value() == "w #2 r #1");
	BOOST_TEST(t.row_title(2, 2).value() == "w #2 r #2");
	BOOST_TEST(t.row_title(2, 3).value() == "w #2 r #3");
	BOOST_TEST(t.row_title(2, 4).value() == "w #2 r #4");

	BOOST_TEST(t.col_title(1, 1).value() == "w #1 c #1");
	BOOST_TEST(t.col_title(1, 2).value() == "w #1 c #2");
	BOOST_TEST(t.col_title(1, 3).value() == "w #1 c #3");
	BOOST_TEST(t.col_title(1, 4).value() == "w #1 c #4");

	BOOST_TEST(t.col_title(2, 1).value() == "w #2 c #1");
	BOOST_TEST(t.col_title(2, 2).value() == "w #2 c #2");
	BOOST_TEST(t.col_title(2, 3).value() == "w #2 c #3");
	BOOST_TEST(t.col_title(2, 4).value() == "w #2 c #4");
	BOOST_TEST(t.col_title(2, 5).value() == "w #2 c #5");

	const vector<int> expected_row_indices_w_1 = { w_1_r_1, w_1_r_2, w_1_r_3 };
	const vector<int> expected_row_indices_w_2 = { w_2_r_1, w_2_r_2, w_2_r_3, w_2_r_4 };
	const vector<int> expected_col_indices_w_1 = { w_1_c_1, w_1_c_2, w_1_c_3, w_1_c_4 };
	const vector<int> expected_col_indices_w_2 = {
		w_2_c_1, w_2_c_2, w_2_c_3, w_2_c_4, w_2_c_5
	};

	BOOST_TEST(t.row_indices(1) == expected_row_indices_w_1);
	BOOST_TEST(t.row_indices(2) == expected_row_indices_w_2);
	BOOST_TEST(t.col_indices(1) == expected_col_indices_w_1);
	BOOST_TEST(t.col_indices(2) == expected_col_indices_w_2);

	BOOST_TEST(t.row_count(1) == expected_row_indices_w_1.size());
	BOOST_TEST(t.row_count(2) == expected_row_indices_w_2.size());
	BOOST_TEST(t.col_count(1) == expected_col_indices_w_1.size());
	BOOST_TEST(t.col_count(2) == expected_col_indices_w_2.size());

	t.clear();
	BOOST_TEST(t.wkt_count() == 0);
	BOOST_TEST(t.empty());
}


BOOST_AUTO_TEST_SUITE_END()
