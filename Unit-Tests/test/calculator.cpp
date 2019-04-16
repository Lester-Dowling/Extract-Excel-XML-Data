// Unit-Tests/test/calculator.cpp
// Started 7 Apr 2019
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Operations/Program.hpp"
#include "Operations/Calculator.hpp"
namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
namespace a = boost::algorithm;
using std::setw;
using std::shared_ptr;
using std::make_shared;
using operations::Program;
using Grade = pseudo_xpath_parser::Grade;

/*
 * Unit tests to verify the correct operation of Calculator.
 */

BOOST_AUTO_TEST_SUITE(test_calculator_suite, *utf::disabled())
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

BOOST_AUTO_TEST_CASE(regex_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){3}.*" };
	const std::string calc{ "[1][2][3] * 6" };
	BOOST_TEST(boost::regex_match(calc, e));
}

BOOST_AUTO_TEST_CASE(regex_iterator_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){2,3}" };
	const std::string calc{ "1 + [1][2][3] * 6" };
	std::string prematched, matched;
	std::string::const_iterator postmatched_begin;
	std::string::const_iterator postmatched_end;
	std::for_each(
	  boost::sregex_iterator{ calc.begin(), calc.end(), e },
	  boost::sregex_iterator{},
	  [&](boost::smatch const& what) //
	  {
		  matched = what[0]; // what[0] contains the whole matched string.
		  prematched = std::string{ what.prefix().first, what.prefix().second };
		  postmatched_begin = what.suffix().first;
		  postmatched_end = what.suffix().second;
	  });
	BOOST_TEST(matched == "[1][2][3]");
	BOOST_TEST(prematched == "1 + ");
	const std::string postmatched{ postmatched_begin, postmatched_end };
	BOOST_TEST(postmatched == " * 6");
}

BOOST_AUTO_TEST_CASE(regex_iterator_replace_into_calc_1)
{
	const boost::regex e{ "(\\[[^\\]]+\\]){2,3}" };
	const std::string calc{ "1 + [1][2][3] * 6" };
	std::map<std::string, std::string> values;
	values["[1][2][3]"] = "123";
	std::string replaced;
	std::string::const_iterator postmatched_begin;
	std::for_each(
	  boost::sregex_iterator{ calc.begin(), calc.end(), e },
	  boost::sregex_iterator{},
	  [&](boost::smatch const& what) //
	  {
		  std::string matched = what[0]; // what[0] contains the whole matched string.
		  replaced.append(what.prefix().first, what.prefix().second);
		  replaced.append(values[matched]);
		  postmatched_begin = what.suffix().first;
	  });
	replaced.append(postmatched_begin, calc.cend());
	BOOST_TEST(replaced == "1 + 123 * 6");
}

BOOST_AUTO_TEST_CASE(split_worksheet_cell_ref)
{
	const std::string cell_ref{ "[1][2][3]" };
	std::vector<std::string> cell_ref_ords;
	a::split(cell_ref_ords, cell_ref, a::is_any_of("[]"), a::token_compress_on);
	BOOST_TEST(cell_ref_ords.size() >= 3);
	BOOST_TEST(cell_ref_ords[1] == "1");
	BOOST_TEST(cell_ref_ords[2] == "2");
	BOOST_TEST(cell_ref_ords[3] == "3");
}

BOOST_AUTO_TEST_CASE(regex_iterator_replace_into_calc_2)
{
	char* argv[] = { program_name };
	constexpr int argc = static_cast<int>(std::size(argv));
	std::ostringstream oss;
	std::ostringstream ess;
	auto program = make_shared<Program>(argc, argv, oss, ess);
	excel_xml_parser::Node::SP xml_root = program->load_xml_file(small_xml_file);

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
		  Grade::SP xpath_root = program->parse_xpath_text(cell_ref_xpath_text);
		  const char* const expected_xpath = //
			"Workbook --> Worksheet[Worksheet=1] --> Table --> Row[Row=Total Revenue "
			"Excluding Interest] --> Cell[Cell=06/17] --> Data";
		  BOOST_TEST(Grade::path_to_string(xpath_root) == expected_xpath);
		  const std::string cell_text = program->extract_single_text(xml_root, xpath_root);

		  calc_interpolated += ' ';
		  calc_interpolated.append(cell_text);
		  calc_interpolated += ' ';
	  });
	calc_interpolated.append(postmatched_begin, calc.cend());
	BOOST_TEST(calc_interpolated == "1 +  227904000  * 6");
	BOOST_TEST(program->gCalculator.evaluate(calc_interpolated) == 1 + 227904000 * 6);
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
		auto program = make_shared<Program>(argc, argv, oss, ess);
		program->run();
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
		auto program = make_shared<Program>(argc, argv, oss, ess);
		program->run();
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

BOOST_AUTO_TEST_SUITE_END()
