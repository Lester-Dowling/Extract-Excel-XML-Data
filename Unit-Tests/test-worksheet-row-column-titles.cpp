/**
 * @file   Unit-Tests/test-worksheet-row-column-titles.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "IO-Extra/sequence.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(test_worksheet_row_column_titles_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of simple_xml::Worksheet_Row_Column_Titles.
 */

namespace a = boost::algorithm;
using std::cout;
using std::endl;
using std::vector;
using std::list;
using std::string;

BOOST_AUTO_TEST_CASE(wrct_default_ctor)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	BOOST_TEST(t.size() == 0);
	BOOST_REQUIRE_THROW(t(0), std::runtime_error);
	BOOST_REQUIRE_THROW(t(1), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(wrct_add_worksheet_1)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	t.add_worksheet("one");
	t.add_worksheet("two");
	t.add_worksheet("three");
	BOOST_TEST(t.size() == 3);
	BOOST_REQUIRE_THROW(t(0), std::runtime_error);
	BOOST_REQUIRE_NO_THROW(t(1));
	BOOST_REQUIRE_NO_THROW(t(2));
	BOOST_REQUIRE_NO_THROW(t(3));
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

	const int w_1_r_1 = t(1).add_row("w #1 r #1");
	const int w_1_r_2 = t(1).add_row("w #1 r #2");
	const int w_1_r_3 = t(1).add_row("w #1 r #3");

	const int w_2_r_1 = t(2).add_row("w #2 r #1");
	const int w_2_r_2 = t(2).add_row("w #2 r #2");
	const int w_2_r_3 = t(2).add_row("w #2 r #3");
	const int w_2_r_4 = t(2).add_row("w #2 r #4");

	const int w_1_c_1 = t(1).add_col("w #1 c #1");
	const int w_1_c_2 = t(1).add_col("w #1 c #2");
	const int w_1_c_3 = t(1).add_col("w #1 c #3");
	const int w_1_c_4 = t(1).add_col("w #1 c #4");

	const int w_2_c_1 = t(2).add_col("w #2 c #1");
	const int w_2_c_2 = t(2).add_col("w #2 c #2");
	const int w_2_c_3 = t(2).add_col("w #2 c #3");
	const int w_2_c_4 = t(2).add_col("w #2 c #4");
	const int w_2_c_5 = t(2).add_col("w #2 c #5");

	BOOST_TEST(t(1).row_title(1).value() == "w #1 r #1");
	BOOST_TEST(t(1).row_title(2).value() == "w #1 r #2");
	BOOST_TEST(t(1).row_title(3).value() == "w #1 r #3");

	BOOST_TEST(t(2).row_title(1).value() == "w #2 r #1");
	BOOST_TEST(t(2).row_title(2).value() == "w #2 r #2");
	BOOST_TEST(t(2).row_title(3).value() == "w #2 r #3");
	BOOST_TEST(t(2).row_title(4).value() == "w #2 r #4");

	BOOST_TEST(t(1).col_title(1).value() == "w #1 c #1");
	BOOST_TEST(t(1).col_title(2).value() == "w #1 c #2");
	BOOST_TEST(t(1).col_title(3).value() == "w #1 c #3");
	BOOST_TEST(t(1).col_title(4).value() == "w #1 c #4");

	BOOST_TEST(t(2).col_title(1).value() == "w #2 c #1");
	BOOST_TEST(t(2).col_title(2).value() == "w #2 c #2");
	BOOST_TEST(t(2).col_title(3).value() == "w #2 c #3");
	BOOST_TEST(t(2).col_title(4).value() == "w #2 c #4");
	BOOST_TEST(t(2).col_title(5).value() == "w #2 c #5");

	const vector<int> expected_row_indices_w_1 = { w_1_r_1, w_1_r_2, w_1_r_3 };
	const vector<int> expected_row_indices_w_2 = { w_2_r_1, w_2_r_2, w_2_r_3, w_2_r_4 };
	const vector<int> expected_col_indices_w_1 = { w_1_c_1, w_1_c_2, w_1_c_3, w_1_c_4 };
	const vector<int> expected_col_indices_w_2 = {
		w_2_c_1, w_2_c_2, w_2_c_3, w_2_c_4, w_2_c_5
	};

	BOOST_TEST(t(1).row_indices() == expected_row_indices_w_1);
	BOOST_TEST(t(2).row_indices() == expected_row_indices_w_2);
	BOOST_TEST(t(1).col_indices() == expected_col_indices_w_1);
	BOOST_TEST(t(2).col_indices() == expected_col_indices_w_2);

	BOOST_TEST(t(1).row_count() == expected_row_indices_w_1.size());
	BOOST_TEST(t(1).col_count() == expected_col_indices_w_1.size());
	BOOST_TEST(t(2).row_count() == expected_row_indices_w_2.size());
	BOOST_TEST(t(2).col_count() == expected_col_indices_w_2.size());

	t.clear();
	BOOST_TEST(t.wkt_count() == 0);
	BOOST_TEST(t.empty());
}

BOOST_AUTO_TEST_CASE(wrct_add_multi_cell_row_title)
{
	simple_xml::Worksheet_Row_Column_Titles t;
	BOOST_TEST(t.add_worksheet("one") == 1);
	BOOST_TEST(t.add_worksheet("two") == 2);
	constexpr int w_1_idx = 1;
	constexpr int w_2_idx = 2;
	const int w_1_r_1_idx = t(w_1_idx).add_row("title #1 part #1");
	t(w_1_idx).add_row(w_1_r_1_idx, "part #2");
	const int w_2_r_1_idx = t(w_2_idx).add_row("title #2 part #1");
	t(w_2_idx).add_row(w_2_r_1_idx, "part #2");
	t(w_2_idx).add_row(w_2_r_1_idx, "part #3");
	BOOST_TEST(t(w_1_idx).row_count() == 1);
	BOOST_TEST(t(w_2_idx).row_count() == 1);
	vector<int> w_1_expected_row_indices = { w_1_r_1_idx };
	vector<int> w_2_expected_row_indices = { w_2_r_1_idx };
	BOOST_TEST(t(w_1_idx).row_indices() == w_1_expected_row_indices);
	BOOST_TEST(t(w_2_idx).row_indices() == w_2_expected_row_indices);
	BOOST_TEST(t(w_1_idx).row_title(w_1_r_1_idx).value() == "title #1 part #1, part #2");
	BOOST_TEST(
	  t(w_2_idx).row_title(w_2_r_1_idx).value() == "title #2 part #1, part #2, part #3");
}


BOOST_AUTO_TEST_SUITE_END()
