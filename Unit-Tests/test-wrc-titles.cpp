// Unit-Tests/test-wrc-titles.cpp
// Started 7 Apr 2019
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Operations/Program.hpp"
#include "Simple-XML-Parser/Worksheet-Row-Column-Titles.hpp"
namespace utf = boost::unit_test;
namespace a = boost::algorithm;

/*
 * Unit tests to verify the correct operation of WRC Titles.
 */

BOOST_AUTO_TEST_SUITE(test_wrc_titles_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(wrc_ctor)
{
	excel_xml_parser::Worksheet_Row_Column_Titles t;
	t.add_worksheet(1, "one");
	BOOST_TEST(t.size() == 1);
}

BOOST_AUTO_TEST_CASE(wrc_add_row)
{
	excel_xml_parser::Worksheet_Row_Column_Titles t;
	t.add_worksheet(1, "one wkt");
	t.add_worksheet(2, "two wkt");
	t.add_worksheet(3, "three wkt");
	for (const int wkt_idx : t.wkt_indices())
		BOOST_TEST(t.row_count(wkt_idx) == 0);
	t.add_row(1, 1, "one row");
	t.add_row(1, 2, "two row");
	t.add_row(1, 3, "three row");
	BOOST_TEST(t.row_count(1) == 3);
	BOOST_TEST(t.row_count(2) == 0);
	BOOST_TEST(t.row_count(3) == 0);
	const char* expected[] = { "one row", "two row", "three row" };
	for (const int row_idx : t.row_indices(1))
		BOOST_TEST(t.row_title(1, row_idx).value() == expected[row_idx - 1]);
}

BOOST_AUTO_TEST_CASE(wrc_add_col)
{
	excel_xml_parser::Worksheet_Row_Column_Titles t;
	t.add_worksheet(1, "one wkt");
	t.add_worksheet(2, "two wkt");
	t.add_worksheet(3, "three wkt");
	for (const int wkt_idx : t.wkt_indices())
		BOOST_TEST(t.row_count(wkt_idx) == 0);
	t.add_col(1, 1, "one col");
	t.add_col(1, 2, "two col");
	t.add_col(1, 3, "three col");
	BOOST_TEST(t.col_count(1) == 3);
	BOOST_TEST(t.col_count(2) == 0);
	BOOST_TEST(t.col_count(3) == 0);
	const char* expected[] = { "one col", "two col", "three col" };
	for (const int col_idx : t.col_indices(1))
		BOOST_TEST(t.col_title(1, col_idx).value() == expected[col_idx - 1]);
}

BOOST_AUTO_TEST_CASE(wrc_add_row_col)
{
	using std::string;
	using std::vector;
	vector<string> prefixes = { "one", "two", "three", "four", "five", "six" };
	const string wkt_suffix{ " wkt" };
	const string row_suffix{ " row" };
	const string col_suffix{ " col" };
	excel_xml_parser::Worksheet_Row_Column_Titles t;

	for (auto const& p : prefixes)
		t.add_worksheet(p + wkt_suffix);

	for (const int wkt_idx : t.wkt_indices()) {
		const string expected_title = prefixes[wkt_idx - 1] + wkt_suffix;
		BOOST_TEST(t.wkt_title(wkt_idx).value() == expected_title);
	}

	for (const int wkt_idx : t.wkt_indices()) {
		for (auto const& p : prefixes) {
			t.add_row(wkt_idx, t.wkt_title(wkt_idx).value() + " " + p + row_suffix);
			t.add_col(wkt_idx, t.wkt_title(wkt_idx).value() + " " + p + col_suffix);
		}
	}

	for (const int wkt_idx : t.wkt_indices()) {
		BOOST_TEST(t.row_count(wkt_idx) == prefixes.size());
		for (const int row_idx : t.row_indices(wkt_idx)) {
			const string expected_row_title =
			  t.wkt_title(wkt_idx).value() + " " + prefixes[row_idx - 1] + row_suffix;
			BOOST_TEST(t.row_title(wkt_idx, row_idx).value() == expected_row_title);
		}
		BOOST_TEST(t.col_count(wkt_idx) == prefixes.size());
		for (const int col_idx : t.col_indices(wkt_idx)) {
			const string expected_col_title =
			  t.wkt_title(wkt_idx).value() + " " + prefixes[col_idx - 1] + col_suffix;
			BOOST_TEST(t.col_title(wkt_idx, col_idx).value() == expected_col_title);
		}
	}
}

BOOST_AUTO_TEST_CASE(wrc_bad_wkt)
{
	using std::runtime_error;
	excel_xml_parser::Worksheet_Row_Column_Titles t;
	BOOST_CHECK_THROW(t.add_row(1, "Bad row"), runtime_error);
	BOOST_CHECK_THROW(t.add_row(1, 1, "Bad row"), runtime_error);
	BOOST_CHECK_THROW(t.add_col(1, "Bad col"), runtime_error);
	BOOST_CHECK_THROW(t.add_col(1, 1, "Bad col"), runtime_error);
	BOOST_CHECK_THROW(t.col_count(1), runtime_error);
	BOOST_CHECK_THROW(t.row_indices(1), runtime_error);
	BOOST_CHECK_THROW(t.col_indices(1), runtime_error);
#pragma warning(push)
#pragma warning(disable : 4834) // discard return of function with 'nodiscard' attribute
	BOOST_CHECK_THROW(t.col_title(1, 1).value(), std::bad_optional_access);
	BOOST_CHECK_THROW(t.row_title(1, 1).value(), std::bad_optional_access);
	BOOST_CHECK_THROW(t.wkt_title(1).value(), std::bad_optional_access);
#pragma warning(pop)
}

BOOST_AUTO_TEST_SUITE_END()
