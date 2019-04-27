/**
 * @file   Unit-Tests/test-calculator.cpp
 * @date   Started 2019-04-7
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Pseudo-XPath/forming.hpp"
#include "Operations/Program.hpp"
#include "Operations/Calculator.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(test_calculator_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of Calculator.
 */

namespace a = boost::algorithm;
using operations::Program;
using std::cout;
using std::endl;
using std::setw;
using Grade = pseudo_xpath::Grade;
namespace {
	char* const small_xml_file =
	  "C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml/small.xml";

	char* const program_name = "Unit testing of Extract-Excel-XML-Data";
} // namespace

BOOST_AUTO_TEST_CASE(calculator_default_ctor)
{
	operations::Calculator c;
	BOOST_TEST(c.get_symbol("pi") == 3.1415926535897932385);
	BOOST_TEST(c.get_symbol("e") == 2.7182818284590452354);
}

BOOST_AUTO_TEST_CASE(arithmetic_expression_1)
{
	operations::Calculator c;
	BOOST_TEST(c.evaluate("k = 1; k = k + 1; k * 2") == 4);
}

BOOST_AUTO_TEST_CASE(set_symbol_1)
{
	operations::Calculator c;
	c.set_symbol("k", 1);
	BOOST_TEST(c.evaluate("k = k + 1; k * 2") == 4);
}

BOOST_AUTO_TEST_CASE(function_1)
{
	operations::Calculator c;
	const double v = -123.456;
	c.set_symbol("DATA", v);
	BOOST_TEST(c.evaluate("( abs(DATA) * 2 + 1 ) / 2") == (std::abs(v) * 2 + 1) / 2);
}

BOOST_AUTO_TEST_CASE(regex_iterator_replace_into_calc_2, *utf::enabled())
{
	char* argv[] = { program_name };
	constexpr int argc = static_cast<int>(std::size(argv));
	std::ostringstream oss;
	std::ostringstream ess;
	Program program{ argc, argv, oss, ess };
	program.load_xml_file(small_xml_file);
	program.write_worksheet_titles();
	program.write_column_titles();
	program.write_row_titles();
	std::ostringstream xoss;
	xoss << "Worksheet #  1 --> Profit and Loss" << endl;
	xoss << "Worksheet #  2 --> Balance Sheet" << endl;
	xoss << "Worksheet #1 Profit and Loss Column #  1 --> Profit and Loss Item" << endl;
	xoss << "Worksheet #1 Profit and Loss Column #  2 --> 06/16" << endl;
	xoss << "Worksheet #1 Profit and Loss Column #  3 --> 06/17" << endl;
	xoss << "Worksheet #2 Balance Sheet Column #  1 --> Item" << endl;
	xoss << "Worksheet #2 Balance Sheet Column #  2 --> 06/16" << endl;
	xoss << "Worksheet #2 Balance Sheet Column #  3 --> 06/17" << endl;
	xoss << "Worksheet #1 Profit and Loss Row #  2 --> Operating Revenue" << endl;
	xoss << "Worksheet #1 Profit and Loss Row #  3 --> Other Revenue" << endl;
	xoss << "Worksheet #1 Profit and Loss Row #  4 --> Total Revenue Excluding Interest"
		 << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  2 --> CA - Cash" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  3 --> CA - Receivables" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  4 --> CA - Prepaid Expenses" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  5 --> CA - Inventories" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  6 --> CA - Investments" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  7 --> CA - NCA Held Sale" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  8 --> CA - Other" << endl;
	xoss << "Worksheet #2 Balance Sheet Row #  9 --> Total Current Assets" << endl;
	BOOST_TEST(oss.str() == xoss.str());

	const boost::regex cell_ref_regex{ "(\\[[^\\]]+\\]){2,3}" };
	const std::string calc{ "1 + ['Total Revenue Excluding Interest']['06/17'] * 6" };
	std::string calc_interpolated;
	std::string::const_iterator postmatched_begin;
	std::for_each(
	  boost::sregex_iterator{ calc.begin(), calc.end(), cell_ref_regex },
	  boost::sregex_iterator{},
	  [&](boost::smatch const& what) //
	  {
		  calc_interpolated.append(what.prefix().first, what.prefix().second);
		  postmatched_begin = what.suffix().first;

		  // what[0] contains the whole matched string:
		  const std::string cell_ref = what[0];
		  std::list<std::string> cell_ref_ords;
		  a::split(cell_ref_ords, cell_ref, a::is_any_of("[]"), a::token_compress_on);
		  BOOST_TEST(cell_ref_ords.size() >= 2);
		  if (cell_ref_ords.front().empty())
			  cell_ref_ords.pop_front();
		  std::string cell_ref_xpath_text = "Row[";
		  std::string const& row_ref = cell_ref_ords.front();
		  cell_ref_xpath_text += row_ref;
		  cell_ref_xpath_text += ']';
		  cell_ref_ords.pop_front();

		  cell_ref_xpath_text += ',';
		  cell_ref_xpath_text += "Cell[";
		  std::string const& col_ref = cell_ref_ords.front();
		  cell_ref_xpath_text += col_ref;
		  cell_ref_xpath_text += ']';
		  cell_ref_ords.pop_front();
		  cell_ref_xpath_text += ',';
		  cell_ref_xpath_text += "Data";
		  Grade::SP xpath_root =
			program.parse_xpath_text(pseudo_xpath::prefix(1) + cell_ref_xpath_text);
		  const char* const expected_xpath = //
			"Workbook --> Worksheet[Worksheet=1] --> Table --> "
			"Row[Row='Total Revenue Excluding Interest'] --> Cell[Cell='06/17'] --> Data";
		  BOOST_TEST(Grade::path_to_string(xpath_root) == expected_xpath);
		  const std::string cell_text = program.extract_single_text(xpath_root);

		  calc_interpolated += ' ';
		  calc_interpolated.append(cell_text);
		  calc_interpolated += ' ';
	  });
	calc_interpolated.append(postmatched_begin, calc.cend());
	BOOST_TEST(calc_interpolated == "1 +  227904000  * 6");
	BOOST_TEST(program.gCalculator.evaluate(calc_interpolated) == 1 + 227904000 * 6);
}

namespace {
	int run_int_calculator(std::string expr)
	{
		std::ostringstream calc_clo_oss;
		calc_clo_oss << "--calc=" << expr;
		const std::string calc_clo = calc_clo_oss.str();
		std::ostringstream prec_clo_oss;
		const int precision = 0;
		prec_clo_oss << "--precision=" << precision;
		const std::string prec_clo = prec_clo_oss.str();
		std::vector<char*> args;
		args.push_back(program_name);
		args.push_back(const_cast<char*>(prec_clo.c_str()));
		args.push_back(const_cast<char*>(calc_clo.c_str()));
		args.push_back(small_xml_file);
		char** const argv = &args.front();
		const int argc = static_cast<int>(args.size());
		std::ostringstream oss;
		std::ostringstream ess;
		Program program{ argc, argv, oss, ess };
		program.run();
		if (!ess.str().empty())
			throw std::runtime_error{ ess.str() };
		else
			return std::stoi(oss.str());
	}

	double run_double_calculator(std::string expr, const int precision = -1)
	{
		std::ostringstream calc_clo_oss;
		calc_clo_oss << "--calc=" << expr;
		const std::string calc_clo = calc_clo_oss.str();
		std::ostringstream prec_clo_oss;
		prec_clo_oss << "--precision=" << precision;
		const std::string prec_clo = prec_clo_oss.str();
		std::vector<char*> args;
		args.push_back(program_name);
		args.push_back(const_cast<char*>(prec_clo.c_str()));
		args.push_back(const_cast<char*>(calc_clo.c_str()));
		args.push_back(small_xml_file);
		char** const argv = &args.front();
		const int argc = static_cast<int>(args.size());
		std::ostringstream oss;
		std::ostringstream ess;
		Program program{ argc, argv, oss, ess };
		program.run();
		if (!ess.str().empty())
			throw std::runtime_error{ ess.str() };
		else
			return std::stod(oss.str());
	}

	/*
	 * Test utility function to limit precision of doubles.
	 */
	double lp(double v, const int precision = -1)
	{
		std::ostringstream oss;
		oss << std::setprecision(
				 (precision < 0) ? std::numeric_limits<double>::digits10 + 1 : precision)
			<< v;
		// const double scaler = std::pow(10, precision);
		// return std::floor(v * scaler) / scaler;
		return std::stod(oss.str());
	}
} // namespace

BOOST_AUTO_TEST_CASE(calc_int_arithmetic)
{
	BOOST_TEST(run_int_calculator("1 + 1") == 1 + 1);
	BOOST_TEST(run_int_calculator("2 + 3 * 6") == 2 + 3 * 6);
	BOOST_TEST(run_int_calculator("(-2 * 6 + 24) / 3") == (-2 * 6 + 24) / 3);
}

BOOST_AUTO_TEST_CASE(calc_double_arithmetic)
{
	const double kPI = 2 * std::acos(0);
	const double kE = std::exp(1);
	BOOST_TEST(run_double_calculator("pi") == lp(kPI));
	BOOST_TEST(run_double_calculator("e") == lp(kE));
	BOOST_TEST(run_double_calculator("1/2") == lp(1.0 / 2.0));
	BOOST_TEST(
	  run_double_calculator("deltapercent(2,4)") == 100); // 100% increase from 2 to 4.
	BOOST_TEST(
	  run_double_calculator("deltapercent(2,3)") == 50); // 50% increase from 2 to 3.
	BOOST_TEST(run_double_calculator("cos(pi)") == std::cos(kPI));
	const double calc_sin_pi = run_double_calculator("sin(pi)");
	const double this_sin_pi = std::sin(kPI);
	const double diff_sin_pi = std::abs(this_sin_pi - calc_sin_pi);
	BOOST_TEST(diff_sin_pi < 1.0e-16);
}

BOOST_AUTO_TEST_CASE(date_function)
{
	operations::Calculator c;
	BOOST_TEST(c.evaluate("date(09/08/2018)") == 2018.6054794520548512);
	BOOST_TEST(c.evaluate("date(08/08/2018)") == 2018.6027397260274938);
	BOOST_TEST(c.evaluate("date(07/08/2018)") == 2018.5999999999999091);
	BOOST_TEST(c.evaluate("date(06/08/2018)") == 2018.5972602739725517);
	BOOST_TEST(c.evaluate("date(03/08/2018)") == 2018.5890410958904795);

	BOOST_TEST(c.evaluate("date(09/08/2018) - 2018") == 0.6054794520548512);
	BOOST_TEST(c.evaluate("date(08/08/2018) - 2010") == 8.6027397260274938);
	BOOST_TEST(c.evaluate("date(07/08/2018)-2000") == 18.5999999999999091);
	BOOST_TEST(c.evaluate("date(06/08/2018)-1000*2") == 18.5972602739725517);
	BOOST_TEST(c.evaluate("-2018 + date(03/08/2018)") == 0.5890410958904795);
}

BOOST_AUTO_TEST_SUITE_END()
