/**
 * @file   Unit-Practices/practice/regex.cpp
 * @date   Started 2019-04-12
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
#include "Operations/string-functions.hpp"
namespace utf = boost::unit_test;
namespace a = boost::algorithm;
using boost::regex;
using boost::regex_match;
using boost::smatch;
using std::string;

/*
 * Practise and verify the correct performance of regex expressions.
 */

BOOST_AUTO_TEST_SUITE(test_regex_suite, *utf::enabled())

namespace {

inline bool starts_with(std::string_view big, std::string_view x)
{
	return big.size() >= x.size() && big.compare(0, x.size(), x) == 0;
}

}

BOOST_AUTO_TEST_CASE(starts_with_workbook)
{
	const regex workbook_regex{ "[[:space:]]*Workbook\\b.*" };
	const char* should_pass[] = //
	  { " Workbook, Worksheet, Table ", "   Workbook", "Workbook[something]" };
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		const string str{ should_pass[idx] };
		smatch workbook_match;
		BOOST_TEST(regex_match(str, workbook_match, workbook_regex));
		BOOST_TEST(workbook_match[0] == str);
	}
}

BOOST_AUTO_TEST_CASE(workbook_with_filter)
{
	const regex filter_regex{ "[[:space:]]*Workbook[[:space:]]*\\[([^\\]]*)\\].*" };
	const char* should_pass[] = //
	  { "Workbook[f]", "  Workbook[ff], Worksheet, Table " };
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		const string str{ should_pass[idx] };
		smatch filter_match;
		BOOST_TEST(regex_match(str, filter_match, filter_regex));
		const auto repeat_count = static_cast<std::string::size_type>(idx + 1);
		const char repeated_char = 'f';
		const std::string expected_filter(repeat_count, repeated_char);
		BOOST_TEST(filter_match[1] == expected_filter);
	}
}

BOOST_AUTO_TEST_CASE(with_worksheet)
{
	const regex worksheet_regex{ ".*\\bWorksheet\\b.*" };
	const char* should_pass[] = //
	  { " Workbook, Worksheet, Table ", "   Worksheet", "Worksheet[something]" };
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		const string str{ should_pass[idx] };
		smatch worksheet_match;
		BOOST_TEST(regex_match(str, worksheet_match, worksheet_regex));
		BOOST_TEST(worksheet_match[0] == str);
	}
}

BOOST_AUTO_TEST_CASE(start_workbook)
{
	const char* should_pass[] = //
	  { " Workbook, Worksheet, Table ",
		"   Workbook",
		" Workbook[some other thing], Worksheet[something]" };
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		string gXPathText{ should_pass[idx] };

		string full_xpath_text;
		const regex workbook_regex{ "[[:space:]]*Workbook\\b(.*)" };
		full_xpath_text = "Workbook";
		boost::smatch workbook_match;
		if (regex_match(gXPathText, workbook_match, workbook_regex)) {
			// Extract an optional workbook filter:
			const regex workbook_filter_regex{
				"[[:space:]]*Workbook[[:space:]]*(\\[[^\\]]*\\])(.*)"
			};
			boost::smatch workbook_filter_match;
			if (regex_match(gXPathText, workbook_filter_match, workbook_filter_regex)) {
				full_xpath_text += workbook_filter_match[1];
				gXPathText = workbook_filter_match[2];
			}
			else
				gXPathText = workbook_match[1];
		}
		BOOST_TEST(a::starts_with(full_xpath_text, "Workbook"));
		BOOST_TEST(!a::starts_with(a::trim_copy(gXPathText), "Workbook"));
	}
}

BOOST_AUTO_TEST_CASE(continue_worksheet)
{
	const char* should_pass[] = {
		", Worksheet, Table ",	  //
		",   Worksheet",		  //
		", Worksheet[something]", //
		", Table"				  //
	};
	const std::string gDefaultWorksheet = "1";
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		string gXPathText{ should_pass[idx] };
		string full_xpath_text;

		full_xpath_text += ",Worksheet";
		const regex worksheet_regex{ ".*\\bWorksheet\\b(.*)" };
		boost::smatch worksheet_match;
		if (regex_match(gXPathText, worksheet_match, worksheet_regex)) {
			// Extract an optional worksheet filter:
			const regex worksheet_filter_regex{
				".*\\bWorksheet[[:space:]]*(\\[[^\\]]*\\])(.*)"
			};
			boost::smatch worksheet_filter_match;
			if (regex_match(gXPathText, worksheet_filter_match, worksheet_filter_regex)) {
				full_xpath_text += worksheet_filter_match[1];
				gXPathText = worksheet_filter_match[2];
			}
			else {
				full_xpath_text += '[';
				operations::append_quoted_if_not_number(
				  full_xpath_text, gDefaultWorksheet, "Default worksheet ref is missing.");
				full_xpath_text += ']';
				gXPathText = worksheet_match[1];
			}
		}
		else {
			full_xpath_text += '[';
			operations::append_quoted_if_not_number(
			  full_xpath_text, gDefaultWorksheet, "Default worksheet ref is missing.");
			full_xpath_text += ']';
		}
		full_xpath_text += gXPathText;
		// BOOST_TEST_MESSAGE('#' << idx << ' ' << full_xpath_text);
		BOOST_TEST(
		  (a::starts_with(full_xpath_text, ",Worksheet[1]") ||
		   a::starts_with(full_xpath_text, ",Worksheet[something]")));
	}
}

BOOST_AUTO_TEST_CASE(continue_table)
{
	const char* should_pass[] = {
		", Table ",			  //
		"",					  //
		", Table[something]", //
		", Table, Row"		  //
	};
	const size_t should_pass_size = std::size(should_pass);
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		string gXPathText{ should_pass[idx] };
		string full_xpath_text;

		full_xpath_text += ",Table";

		const regex table_regex{ ".*\\bTable\\b(.*)" };
		boost::smatch table_match;
		if (regex_match(gXPathText, table_match, table_regex)) {
			// Extract an optional table filter:
			const regex table_filter_regex{ ".*\\bTable[[:space:]]*(\\[[^\\]]*\\])(.*)" };
			boost::smatch table_filter_match;
			if (regex_match(gXPathText, table_filter_match, table_filter_regex)) {
				full_xpath_text += table_filter_match[1];
				gXPathText = table_filter_match[2];
			}
			else
				gXPathText = table_match[1];
		}
		full_xpath_text += gXPathText;
		// BOOST_TEST_MESSAGE('#' << idx << ' ' << full_xpath_text);
		BOOST_TEST(a::starts_with(full_xpath_text, ",Table"));
	}
}

BOOST_AUTO_TEST_CASE(workbook_worksheet_table)
{
	using boost::regex;
	using boost::regex_match;
	using std::string;

	const char* should_pass[] = {
		" ",												   //
		"",													   //
		"Workbook, Worksheet, Table, Row, Cell, Data",		   //
		"Worksheet[2], Row[5], Cell[6], Data",				   //
		"Worksheet[Big Time Budget], Row[1], Cell[1000], Data" //
	};
	const size_t should_pass_size = std::size(should_pass);
	const std::string gDefaultWorksheet = "1";
	for (size_t idx = 0; idx < should_pass_size; ++idx) {
		string gXPathText{ should_pass[idx] };
		string full_xpath_text;

		full_xpath_text = "Workbook";
		const regex workbook_regex{ "[[:space:]]*Workbook\\b(.*)" };
		boost::smatch workbook_match;
		if (regex_match(gXPathText, workbook_match, workbook_regex)) {
			// Extract an optional workbook filter:
			const regex workbook_filter_regex{
				"[[:space:]]*Workbook[[:space:]]*(\\[[^\\]]*\\])(.*)"
			};
			boost::smatch workbook_filter_match;
			if (regex_match(gXPathText, workbook_filter_match, workbook_filter_regex)) {
				full_xpath_text += workbook_filter_match[1];
				gXPathText = workbook_filter_match[2];
			}
			else
				gXPathText = workbook_match[1];
		}

		full_xpath_text += ",Worksheet";
		const regex worksheet_regex{ ".*\\bWorksheet\\b(.*)" };
		boost::smatch worksheet_match;
		if (regex_match(gXPathText, worksheet_match, worksheet_regex)) {
			// Extract an optional worksheet filter:
			const regex worksheet_filter_regex{
				".*\\bWorksheet[[:space:]]*(\\[[^\\]]*\\])(.*)"
			};
			boost::smatch worksheet_filter_match;
			if (regex_match(gXPathText, worksheet_filter_match, worksheet_filter_regex)) {
				full_xpath_text += worksheet_filter_match[1];
				gXPathText = worksheet_filter_match[2];
			}
			else {
				full_xpath_text += '[';
				operations::append_quoted_if_not_number(
				  full_xpath_text, gDefaultWorksheet, "Default worksheet ref is missing.");
				full_xpath_text += ']';
				gXPathText = worksheet_match[1];
			}
		}
		else {
			full_xpath_text += '[';
			operations::append_quoted_if_not_number(
			  full_xpath_text, gDefaultWorksheet, "Default worksheet ref is missing.");
			full_xpath_text += ']';
		}

		full_xpath_text += ",Table";
		const regex table_regex{ ".*\\bTable\\b(.*)" };
		boost::smatch table_match;
		if (regex_match(gXPathText, table_match, table_regex)) {
			// Extract an optional table filter:
			const regex table_filter_regex{ ".*\\bTable[[:space:]]*(\\[[^\\]]*\\])(.*)" };
			boost::smatch table_filter_match;
			if (regex_match(gXPathText, table_filter_match, table_filter_regex)) {
				full_xpath_text += table_filter_match[1];
				gXPathText = table_filter_match[2];
			}
			else
				gXPathText = table_match[1];
		}
		full_xpath_text += gXPathText;
		const regex expected_regex{ "Workbook,Worksheet\\[[^\\]]+\\],Table\\b.*" };
		BOOST_TEST(regex_match(full_xpath_text, expected_regex));
		// BOOST_TEST_MESSAGE('#' << idx << ' ' << full_xpath_text);
	}
}

BOOST_AUTO_TEST_SUITE_END()
