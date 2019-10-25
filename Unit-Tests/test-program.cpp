// Unit-Tests/test-program.cpp
// Started 7 Apr 2019
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Pseudo-XPath/forming.hpp"
#include "Operations/Program.hpp"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_program_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of Program.
 */

namespace a = boost::algorithm;
namespace f = boost::filesystem;
using operations::Program;
using pseudo_xpath::Grade;
using std::cout;
using std::endl;
using std::list;
using std::ostringstream;
using std::setw;
using std::string;
using std::vector;
namespace {

	const f::path samples_dir{
		"C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml"
	};

	char* const sample_xml_file =
	  "C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml/SHV.xml";

	char* const small_xml_file =
	  "C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml/small.xml";

	char* const price_history_xml_file =
	  "C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml/"
	  "price-history.xml";

	char* const program_name = "Unit tests of Extract-Excel-XML-Data";
} // namespace

BOOST_AUTO_TEST_CASE(program_create)
{
	char* argv[] = { program_name };
	constexpr int argc = static_cast<int>(std::size(argv));
	std::ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	BOOST_TEST(program.argc == 1);
	BOOST_TEST(oss.str().empty());
	BOOST_TEST(ess.str().empty());
}

BOOST_AUTO_TEST_CASE(single_cell_extraction_1)
{
	char* argv[] = //
	  { program_name,
		"--row_titles=Item",
		"--enumerate_rows",
		"--xpath=Row['Net Profit after Tax Before Abnormals'] , Cell['06/16'] , Data",
		sample_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(!oss.str().empty());
	BOOST_TEST(ess.str().empty());
	list<string> expected_output_pieces = {
		"16",	  // Expected row number.
		"34177000" // Expected value.
	};
	const string expected_answer = a::join(expected_output_pieces, " \t ");
	BOOST_TEST(a::trim_copy(oss.str()) == expected_answer);
}

BOOST_AUTO_TEST_CASE(single_cell_extraction_2)
{
	char* argv[] = //
	  { program_name,
		"--row_titles=Item",
		"--xpath=Row['Operating Revenue'] , Cell['06/17'] , Data",
		sample_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(ess.str().empty());
	list<string> expected_output_pieces = {
		"239981000" // Expected value.
	};
	const string expected_answer = a::join(expected_output_pieces, "\t");
	BOOST_TEST(a::trim_copy(oss.str()) == expected_answer);
}

BOOST_AUTO_TEST_CASE(single_cell_extraction_3)
{
	char* argv[] = //
	  { program_name,
		"--row_titles=Item",
		"--xpath=Row[22] , Cell[5] , Data",
		// Row['Shares Outstanding at Period End']  Cell['06/17']
		sample_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(ess.str().empty());
	const double expected_number = 73'606'835.00; // Shares outstanding at 06/2017
	std::istringstream program_answer{ oss.str() };
	double program_number;
	program_answer >> program_number;
	BOOST_TEST(program_number == expected_number);
}

BOOST_AUTO_TEST_CASE(calculator_on_single_cell_extraction_1)
{
	char* argv[] = //
	  { program_name,
		"--row_titles=Item",
		"--xpath=Row[22] , Cell[5] , Data",
		"--each=DATA * 3 + 2",
		sample_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(ess.str().empty());
	const double extracted_number = 73'606'835.00; // Shares outstanding at 06/2017
	std::istringstream program_answer{ oss.str() };
	double program_number;
	program_answer >> program_number;
	BOOST_TEST(program_number == extracted_number * 3 + 2);
}

BOOST_AUTO_TEST_CASE(all_data_extraction_1)
{
	char* argv[] = { program_name, "--enumerate_rows", "--xpath=Data", small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(!oss.str().empty());
	BOOST_TEST(ess.str().empty());
	typedef list<string> pieces_t;
	pieces_t expected_output_pieces = //
	  { "1",		 "Profit and Loss Item",
		"06/16",	 "06/17",
		"2",		 "Operating Revenue",
		"285917000", "239981000",
		"3",		 "Other Revenue",
		"-34138000", "-12077000",
		"4",		 "Total Revenue Excluding Interest",
		"251779000", "227904000",
		"1",		 "Item",
		"06/16",	 "06/17",
		"2",		 "CA - Cash",
		"1435000",   "1060000",
		"3",		 "CA - Receivables",
		"44888000",  "41131000",
		"4",		 "CA - Prepaid Expenses",
		"3589000",   "5675000",
		"5",		 "CA - Inventories",
		"104316000", "87474000",
		"6",		 "CA - Investments",
		"1293000",   "1270000",
		"7",		 "CA - NCA Held Sale",
		"0",		 "0",
		"8",		 "CA - Other",
		"0",		 "0",
		"9",		 "Total Current Assets",
		"155521000", "136610000" };
	pieces_t oss_pieces;
	a::split(oss_pieces, oss.str(), a::is_any_of("\t\n\r"), a::token_compress_on);
	// Remove spurious left-over after split:
	for (auto& piece : oss_pieces)
		a::trim(piece);
	oss_pieces.remove_if([](string const& s) { return s.empty(); });
	// typedef pieces_t::const_iterator Iter;
	// for (Iter eitr = expected_output_pieces.begin(), oitr = oss_pieces.begin();
	//	eitr != expected_output_pieces.end() && oitr != oss_pieces.end(); ++eitr, ++oitr)
	//{
	//	BOOST_TEST_MESSAGE(setw(36) << *eitr << setw(36) << *oitr);
	//}
	BOOST_TEST(oss_pieces.size() == expected_output_pieces.size());
	BOOST_TEST(oss_pieces == expected_output_pieces);
}

BOOST_AUTO_TEST_CASE(single_cell_data_extraction_1)
{
	char* argv[] = { program_name };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	const char* const one_data_xpath_text = //
	  " Row['Total Revenue Excluding Interest'], "
	  "Cell['06/17'], "
	  " Data ";
	program.load_xml_file(small_xml_file);
	const string xpath_text = pseudo_xpath::prefix(1) + one_data_xpath_text;
	Grade::SP xpath_root = pseudo_xpath::Grade::parse(xpath_text);
	const char* const expected_xpath = //
	  "Workbook --> Worksheet[Worksheet=1] --> Table --> "
	  "Row[Row='Total Revenue Excluding Interest'] --> "
	  "Cell[Cell='06/17'] --> Data";
	BOOST_TEST(Grade::path_to_string(xpath_root) == expected_xpath);
	const double extracted_number = program.extract_single_number(xpath_root);
	BOOST_TEST(extracted_number == 227904000);
	const string extracted_text = program.extract_single_text(xpath_root);
	BOOST_TEST(extracted_text == "227904000");
}

BOOST_AUTO_TEST_CASE(calc_expression_1)
{
	char* argv[] = { program_name,
					 "-c 1 + ['Total Revenue Excluding Interest']['06/17'] * 6",
					 small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(!oss.str().empty());
	BOOST_TEST(ess.str().empty());
	const double expected_result = 1 + 227904000 * 6;
	ostringstream expected_output;
	expected_output << std::fixed << std::setprecision(program.precision())
					<< expected_result << std::endl;
	BOOST_TEST(oss.str() == expected_output.str());
}

BOOST_AUTO_TEST_CASE(calc_expression_with_worksheet_title)
{
	char* argv[] = {
		program_name,
		"-c 1 + [Profit and Loss][Total Revenue Excluding Interest][06/17] * 6",
		small_xml_file
	};
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(!oss.str().empty());
	BOOST_TEST(ess.str().empty());
	const double expected_result = 1 + 227904000 * 6;
	ostringstream expected_output;
	expected_output << std::fixed << std::setprecision(program.precision())
					<< expected_result << std::endl;
	BOOST_TEST(oss.str() == expected_output.str());
}

BOOST_AUTO_TEST_CASE(calc_expression_across_two_worksheet_titles)
{
	char* argv[] = { program_name,
					 "-c "
					 "2 * [Profit and Loss][Total Revenue Excluding Interest][06/17]"
					 " + "
					 "[Balance Sheet][Total Current Assets][06/17] * 8"
					 " + 42",
					 small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(!oss.str().empty());
	BOOST_TEST(ess.str().empty());
	const double expected_result = 2 * 227'904'000.00 + 136'610'000.00 * 8 + 42;
	ostringstream expected_output;
	expected_output << std::fixed << std::setprecision(program.precision())
					<< expected_result << std::endl;
	BOOST_TEST(oss.str() == expected_output.str());
}

BOOST_AUTO_TEST_CASE(calc_expression_bad_worksheet_title)
{
	char* argv[] = { program_name,
					 "-c "
					 "[Bad worksheet title][Total Revenue Excluding Interest][06/17]",
					 small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	BOOST_CHECK_THROW(program.run(), std::runtime_error);
	BOOST_TEST(oss.str().empty());
}

BOOST_AUTO_TEST_CASE(calc_expression_bad_row_title)
{
	char* argv[] = { program_name,
					 "-c "
					 "[Profit and Loss][Bad row title][06/17]",
					 small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	BOOST_CHECK_THROW(program.run(), std::runtime_error);
	BOOST_TEST(oss.str().empty());
}

BOOST_AUTO_TEST_CASE(calc_expression_bad_column_title)
{
	char* argv[] = {
		program_name,
		"-c "
		"[Profit and Loss][Total Revenue Excluding Interest][Bad column title]",
		small_xml_file
	};
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	BOOST_CHECK_THROW(program.run(), std::runtime_error);
	BOOST_TEST(oss.str().empty());
}

BOOST_AUTO_TEST_CASE(parse_number_1)
{
	const char* const input = "123";
	double parsed_number;
	std::istringstream iss{ input };
	iss >> parsed_number;
	BOOST_TEST(parsed_number == 123);
	BOOST_TEST(iss.eof());
}

BOOST_AUTO_TEST_CASE(parse_non_number_1)
{
	const char* const input = "06/17";
	double parsed_number;
	std::istringstream iss{ input };
	iss >> parsed_number;
	BOOST_TEST(!iss.eof());
}

BOOST_AUTO_TEST_CASE(calc_each_constraint_1)
{
	using boost::regex;
	using boost::regex_match;
	using boost::smatch;
	const string calc_each_text{ " [Row=1] Data * 2" };
	const regex constraint_regex{ "[[:space:]]*\\[([^\\]]+)\\][[:space:]]*(.*)" };
	smatch constraint_match;
	if (regex_match(calc_each_text, constraint_match, constraint_regex)) {
		// Extract an optional constraint:
		const string constraint = constraint_match[1];
		BOOST_TEST(constraint == "Row=1");
		const regex attr_regex{ "[[:space:]]*(.*)=(.*)[[:space:]]*" };
		smatch attr_match;
		if (regex_match(constraint, attr_match, attr_regex)) {
			const string row_or_column = attr_match[1];
			const string value = attr_match[2];
			BOOST_TEST(row_or_column == "Row");
			BOOST_TEST(value == "1");
		}
	}
}

BOOST_AUTO_TEST_CASE(calc_each_constraint_2)
{
	char* argv[] = //
	  { program_name,
		"--row_titles=Item",
		"--xpath=Row[Row>1], Cell[Column>1], Data",
		"--each=[Column=2] DATA * 3",
		price_history_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(ess.str().empty());
	typedef list<string> pieces_t;
	pieces_t expected_output_pieces = //
	  { "17.7300000000000004", "89177",  "18.0000000000000000", "108948",
		"17.7899999999999991", "100885", "18.4199999999999982", "149858",
		"18.4800000000000004", "84893" };
	pieces_t oss_pieces;
	a::split(oss_pieces, oss.str(), a::is_any_of("\t\n\r"), a::token_compress_on);
	// Remove spurious left-over after split:
	for (auto& piece : oss_pieces)
		a::trim(piece);
	oss_pieces.remove_if([](string const& s) { return s.empty(); });
	BOOST_TEST(oss_pieces.size() == expected_output_pieces.size());
	BOOST_TEST(oss_pieces == expected_output_pieces);
}

BOOST_AUTO_TEST_CASE(implicit_alternation)
{
	char* argv[] = //
	  { program_name, "--xpath=Row[Row>1], Cell[2|3], Data", small_xml_file };
	constexpr int argc = static_cast<int>(std::size(argv));
	ostringstream oss;
	ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.run();
	BOOST_TEST(ess.str().empty());
	typedef list<string> pieces_t;
	pieces_t expected_output_pieces = //
	  { "285917000", "239981000", "-34138000", "-12077000", "251779000", "227904000" };
	pieces_t oss_pieces;
	a::split(oss_pieces, oss.str(), a::is_any_of("\t\n\r"), a::token_compress_on);
	// Remove spurious left-over after split:
	for (auto& piece : oss_pieces)
		a::trim(piece);
	oss_pieces.remove_if([](string const& s) { return s.empty(); });
	BOOST_TEST(oss_pieces.size() == expected_output_pieces.size());
	BOOST_TEST(oss_pieces == expected_output_pieces);
	// BOOST_TEST_MESSAGE(oss.str());
}

BOOST_AUTO_TEST_SUITE_END()
