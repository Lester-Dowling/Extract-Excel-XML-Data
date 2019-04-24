/**
 * @file   Operations/src/Program.cpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#include "pch-operations.hpp"
#include "Operations/Program.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Simple-XML/Element-Filter.hpp"
#include "Pseudo-XPath/mini-grammar.hpp"
#include "Operations/string-functions.hpp"

namespace operations {
	using std::endl;
	using std::runtime_error;
	using std::setw;
	using std::string;
	using std::vector;
	using std::list;
	namespace ascii = boost::spirit::ascii;
	namespace qi = boost::spirit::qi;
	using Grade = pseudo_xpath::Grade;
	using String_Iterator = string::const_iterator;
	using XPath_Grammar = pseudo_xpath::mini_grammar<String_Iterator>;

	const char* const all_data_xpath_text = "Row,Cell,Data";


	Program::Program(int argc, char** argv, ostream& out, ostream& err)
	  : Program_Base{ argc, argv, out, err }
	{
		parse_command_line_options();
	}


	std::string Program::xpath_prefix(std::string worksheet_name)
	{
		// Worksheet[ss:Name="Profit Loss"]
		std::string xp = "Workbook, Worksheet[";
		operations::append_quoted_if_not_number(
		  xp, worksheet_name, "Missing worksheet name");
		xp += "], Table, ";
		return xp;
	}


	std::string Program::xpath_prefix(const int worksheet_number)
	{
		std::ostringstream xp;
		xp << "Workbook, Worksheet[";
		xp << worksheet_number;
		xp << "], Table, ";
		return xp.str();
	}


	std::string Program::xpath_prefix() { return "Workbook, Worksheet, Table, "; }


	void Program::write_worksheet_titles()
	{
		for (const int wkt_idx : m_documents.back().titles().wkt_indices()) {
			*gOut << "Worksheet #" << setw(3) << wkt_idx << " --> "
				  << *m_documents.back().titles().wkt_title(wkt_idx) << endl;
		}
	}


	void Program::write_column_titles()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_documents.back().titles().wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> cols{ m_documents.back().titles().col_indices(*wkt_idx) };
			for (Iterator col_idx = cols.begin(); col_idx != cols.end(); col_idx++) {
				*gOut << "Worksheet #" << *wkt_idx << ' '
					  << *m_documents.back().titles().wkt_title(*wkt_idx) << " Column #"
					  << setw(3) << *col_idx << " --> "
					  << *m_documents.back().titles().col_title(*wkt_idx, *col_idx) << endl;
			}
		}
	}


	void Program::write_row_titles()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_documents.back().titles().wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> rows{ m_documents.back().titles().row_indices(*wkt_idx) };
			for (Iterator row_idx = rows.begin(); row_idx != rows.end(); row_idx++) {
				*gOut << "Worksheet #" << *wkt_idx << ' '
					  << *m_documents.back().titles().wkt_title(*wkt_idx) << " Row #"
					  << setw(3) << *row_idx << " --> "
					  << *m_documents.back().titles().row_title(*wkt_idx, *row_idx) << endl;
			}
		}
	}


	void Program::write_cell_refs()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_documents.back().titles().wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> rows{ m_documents.back().titles().row_indices(*wkt_idx) };
			const vector<int> cols{ m_documents.back().titles().col_indices(*wkt_idx) };
			for (Iterator col_idx = cols.begin(); col_idx != cols.end(); col_idx++) {
				for (Iterator row_idx = rows.begin(); row_idx != rows.end(); row_idx++) {
					*gOut << '[' << *m_documents.back().titles().wkt_title(*wkt_idx)
						  << ']' //
						  << '['
						  << *m_documents.back().titles().row_title(*wkt_idx, *row_idx)
						  << ']' //
						  << '['
						  << *m_documents.back().titles().col_title(*wkt_idx, *col_idx)
						  << ']' //
						  << endl;
				}
			}
		}
	}


	bool Program::constrain_each_calc(int current_row_idx, int current_col_idx)
	{
		using boost::regex;
		using boost::smatch;
		using boost::regex_match;
		if (m_each_calc_constraint._is_valid && m_each_calc_constraint._no_constraint)
			return true;
		if (!m_each_calc_constraint._is_valid) {
			const regex constraint_regex{ "[[:space:]]*\\[([^\\]]+)\\][[:space:]]*(.*)" };
			smatch constraint_match;
			if (regex_match(gEachCalc, constraint_match, constraint_regex)) {
				// Extract non-constraint part:
				gEachCalc = constraint_match[2];
				// Extract an optional constraint:
				const string constraint = constraint_match[1];
				const regex col_number_regex{
					"[[:space:]]*Column[[:space:]]*([=<>])[[:space:]]"
					"*([[:digit:]]+)[[:space:]]*",
					boost::regex::icase
				};
				smatch col_match;
				if (regex_match(constraint, col_match, col_number_regex)) {
					m_each_calc_constraint._name = "Column";
					m_each_calc_constraint._operator = string{ col_match[1] }.front();
					m_each_calc_constraint._value = col_match[2];
					m_each_calc_constraint._number =
					  std::stoi(m_each_calc_constraint._value);
					m_each_calc_constraint._good_number = true;
					m_each_calc_constraint._is_valid = true;
				}
			}
			else {
				m_each_calc_constraint._is_valid = true;
				m_each_calc_constraint._no_constraint = true;
				return true;
			}
		}
		if (m_each_calc_constraint._is_valid) {
			if (m_each_calc_constraint._name == "Column")
				switch (m_each_calc_constraint._operator) {
				case '=':
					return current_col_idx == m_each_calc_constraint._number;
				case '<':
					return current_col_idx < m_each_calc_constraint._number;
				case '>':
					return current_col_idx > m_each_calc_constraint._number;
				default:
					throw std::runtime_error{ "Impossible constraint operator: " +
											  string{ m_each_calc_constraint._operator } };
				}
			else if (m_each_calc_constraint._name == "Row")
				switch (m_each_calc_constraint._operator) {
				case '=':
					return current_row_idx == m_each_calc_constraint._number;
				case '<':
					return current_row_idx < m_each_calc_constraint._number;
				case '>':
					return current_row_idx > m_each_calc_constraint._number;
				default:
					throw std::runtime_error{ "Impossible constraint operator: " +
											  string{ m_each_calc_constraint._operator } };
				}
		}
		return true;
	}


	bool Program::write_text_visit(simple_xml::Element_Visitor& visitor)
	{
		if (m_visited_row == 0)
			m_visited_row = visitor.current().row_idx;

		if (m_visited_row != visitor.current().row_idx) //
		{
			m_visited_row = visitor.current().row_idx;
			m_visited_col = 0;
			*gOut << endl;
		}

		if (gEnumerateRows && m_visited_col == 0)
			*gOut << std::setw(4) << visitor.current().row_idx << " \t ";
		else if (0 < m_visited_col)
			*gOut << " \t ";

		if (gEachCalc.empty()) {
			// No arithmetic expression to evaluate.  Print raw text only:
			*gOut << visitor.current().text();
		}
		else // Else, parse arithmetic expression for each visited:
		  if (constrain_each_calc(visitor.current().row_idx, visitor.current().col_idx)) {
			try {
				static const list<string> cap_variations = { "DATA", "Data", "data" };
				for (auto const& cap_var : cap_variations) {
					gCalculator.set_symbol(cap_var, std::stod(visitor.current().text()));
					gCalculator.set_symbol_text(cap_var, visitor.current().text());
				}
				*gOut << std::fixed << std::setprecision(this->precision())
					  << gCalculator.evaluate(gEachCalc);
			}
			catch (std::invalid_argument const&) {
				// visitor.text() is not a number, so ignore arithmetic expression:
				*gOut << visitor.current().text();
			}
		}
		else {
			// Constraint prevents calc evaluation, so ignore arithmetic expression:
			*gOut << visitor.current().text();
		}
		m_visited_col = visitor.current().col_idx;
		return true;
	}


	string Program::extract_single_text(Grade::SP xpath_root)
	{
		return m_documents.back().extract_single_text(xpath_root);
	}


	double Program::extract_single_number(Grade::SP xpath_root)
	{
		return m_documents.back().extract_single_number(xpath_root);
	}


	void Program::load_xml_file(const f::path xml_path)
	{
		m_documents.emplace_back(xml_path);
		if (!gNoColumnTitles)
			m_documents.back().extract_column_titles(
			  gColumnTitlesRow, gColumnTitleRowCount);
		if (!gNoRowTitles)
			m_documents.back().extract_row_titles(gRowTitlesColumn);
	}


	Grade::SP Program::parse_xpath_text(const string xpath_text) // , const int wkt_idx)
	{
		using std::runtime_error;
		namespace ascii = boost::spirit::ascii;
		namespace qi = boost::spirit::qi;

		XPath_Grammar xpath_parser;
		String_Iterator sitr = xpath_text.begin();
		String_Iterator const send = xpath_text.end();
		if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
			throw runtime_error{ "Failed to parse this XPath: " + xpath_text };
		if (gVerbose)
			*gErr << "XPath == " << Grade::path_to_string(xpath_parser.result) << endl;
		return xpath_parser.result;
	}


	void Program::compute_xpath_and_write_results()
	{
		using boost::regex;
		using boost::regex_match;
		using std::string;
		assert(!gXPathText.empty());
		string full_xpath_text;
		if (gXPathText == "Data")
			full_xpath_text = xpath_prefix() + all_data_xpath_text;
		else {
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
				if (regex_match(
					  gXPathText, worksheet_filter_match, worksheet_filter_regex)) {
					full_xpath_text += worksheet_filter_match[1];
					gXPathText = worksheet_filter_match[2];
				}
				else {
					full_xpath_text += '[';
					operations::append_quoted_if_not_number(
					  full_xpath_text,
					  gDefaultWorksheet,
					  "Default worksheet ref is missing.");
					full_xpath_text += ']';
					gXPathText = worksheet_match[1];
				}
			}
			else {
				full_xpath_text += '[';
				append_quoted_if_not_number(
				  full_xpath_text, gDefaultWorksheet, "Default worksheet ref is missing.");
				full_xpath_text += ']';
			}

			full_xpath_text += ",Table";
			const regex table_regex{ ".*\\bTable\\b(.*)" };
			boost::smatch table_match;
			if (regex_match(gXPathText, table_match, table_regex)) {
				// Extract an optional table filter:
				const regex table_filter_regex{
					".*\\bTable[[:space:]]*(\\[[^\\]]*\\])(.*)"
				};
				boost::smatch table_filter_match;
				if (regex_match(gXPathText, table_filter_match, table_filter_regex)) {
					full_xpath_text += table_filter_match[1];
					gXPathText = table_filter_match[2];
				}
				else
					gXPathText = table_match[1];
			}
			a::trim(gXPathText);
			if (!gXPathText.empty() && gXPathText.front() != ',')
				full_xpath_text += ',';
			full_xpath_text += gXPathText;
		}

		m_documents.back()
		  .filter()
		  .set_filter_path(parse_xpath_text(full_xpath_text))
		  .visit_all_depth_first(boost::bind(&Program::write_text_visit, this, _1));
		*gOut << endl;
	}


	void Program::compute_calc_and_write_results()
	{
		namespace a = boost::algorithm;
		assert(!gCalcText.empty());
		string calc_interpolated;
		if (std::any_of(gCalcText.begin(), gCalcText.end(), boost::is_any_of("[]"))) {
			const boost::regex cell_ref_regex{ "(\\[[^\\]]+\\]){2,3}" };
			string::const_iterator postmatched_begin;
			std::for_each(
			  boost::sregex_iterator{ gCalcText.begin(), gCalcText.end(), cell_ref_regex },
			  boost::sregex_iterator{},
			  [&](boost::smatch const& what) //
			  {
				  calc_interpolated.append(what.prefix().first, what.prefix().second);
				  postmatched_begin = what.suffix().first;
				  // what[0] contains the whole matched string:
				  const string cell_ref = what[0];
				  std::list<string> cell_ref_ords;
				  a::split(
					cell_ref_ords, cell_ref, a::is_any_of("[]"), a::token_compress_on);
				  cell_ref_ords.remove_if([](string const& ord) { return ord.empty(); });
				  if (cell_ref_ords.size() < 2)
					  throw std::runtime_error{ "Bad cell ref" };

				  string worksheet_name;
				  if (cell_ref_ords.size() >= 3) {
					  worksheet_name = cell_ref_ords.front();
					  cell_ref_ords.pop_front();
				  }
				  else {
					  worksheet_name = gDefaultWorksheet;
				  }

				  string cell_ref_xpath_text = xpath_prefix(worksheet_name);

				  cell_ref_xpath_text += " Row[";
				  append_quoted_if_not_number(
					cell_ref_xpath_text, cell_ref_ords.front(), "Empty row ref");
				  cell_ref_xpath_text += ']';
				  cell_ref_ords.pop_front(); // Pop the row ref to get to the cell ref.

				  cell_ref_xpath_text += " , ";
				  cell_ref_xpath_text += "Cell[";
				  append_quoted_if_not_number(
					cell_ref_xpath_text, cell_ref_ords.front(), "Empty column ref");
				  cell_ref_xpath_text += ']';
				  cell_ref_xpath_text += " , ";
				  cell_ref_xpath_text += "Data";

				  Grade::SP xpath_root = parse_xpath_text(cell_ref_xpath_text);
				  const string cell_text =
					m_documents.back().extract_single_text(xpath_root);
				  if (cell_text.empty())
					  throw std::runtime_error{ "Ref to non-existent cell: " +
												cell_ref_xpath_text };
				  calc_interpolated += ' ';
				  calc_interpolated.append(cell_text);
				  calc_interpolated += ' ';
			  });
			calc_interpolated.append(postmatched_begin, gCalcText.cend());
		}
		else
			calc_interpolated = gCalcText;
		gCalculator.evaluate(calc_interpolated);
		for (Calculator::History_Item hi : gCalculator.history()) {
			if (std::holds_alternative<double>(hi)) {
				*gOut << std::fixed << std::setprecision(this->precision())
					  << std::get<double>(hi) << endl;
			}
			else {
				const std::string s = std::get<Calculator::Assignment_Pair>(hi).first;
				const double v = std::get<Calculator::Assignment_Pair>(hi).second;
				*gOut << s << " = " << std::fixed << std::setprecision(this->precision())
					  << v << endl;
			}
		}
	}


	void Program::compute_calc_file_and_write_results()
	{
		assert(!gCalcFile.empty());
		f::path calc_script_path{ gCalcFile };
		if (!f::exists(calc_script_path))
			throw runtime_error{ "No such file: " + calc_script_path.string() };
		f::fstream calc_script_stream{ calc_script_path };
		if (!calc_script_stream)
			throw runtime_error{ "Could not open file: " + calc_script_path.string() };
		boost::system::error_code ec;
		const boost::uintmax_t file_size = f::file_size(calc_script_path, ec);
		if (ec || file_size == static_cast<boost::uintmax_t>(-1))
			throw runtime_error{ "Failed to get calc script file size." };
		if (gVerbose)
			*gErr << "Loading calc script " << calc_script_path.string() << endl;

		// Load the calc script into gCalcText:
		const string saved_gCalcText = gCalcText;
		gCalcText.clear();
		gCalcText.reserve(file_size);
		calc_script_stream.unsetf(std::ios::skipws); // No white space skipping!
		std::copy(
		  std::istream_iterator<char>(calc_script_stream),
		  std::istream_iterator<char>(),
		  std::back_inserter(gCalcText));
		compute_calc_and_write_results();
		gCalcText = saved_gCalcText;
	}


	void Program::setup_option_descriptions()
	{
		Program_Base::setup_option_descriptions();

		// Declare a group of options for only the command line:
		gGeneric.add_options()
		  // --------------------------------------------------------------
		  ("xpath,x",
		   po::value<string>(&gXPathText),
		   "Pseudo XPath to the required data in a worksheet.")
		  // --------------------------------------------------------------
		  ("calc,c",
		   po::value<string>(&gCalcText),
		   "Arithmetic expression to be worked out by the calculator.")
		  // --------------------------------------------------------------
		  ("calc_file,f",
		   po::value<string>(&gCalcFile),
		   "File name of a text file containing arithmetic expressions for the calculator "
		   "to work through.")
		  // --------------------------------------------------------------
		  ("each,e",
		   po::value<string>(&gEachCalc),
		   "Arithmetic expression to be worked out by the calculator for each extract "
		   "value given by a pseudo XPath expression (--xpath).  The symbol DATA in the "
		   "arithmetic will be replaced by its value from the worksheet.  For example, to "
		   "extract and triple every data value from column 2: --xpath "
		   "'Worksheet[1],Row,Cell[2],Data'  --each='DATA * 3'")
		  // --------------------------------------------------------------
		  ("precision,p",
		   po::value<int>(&gPrecision)->default_value(-1),
		   "Number of decimal places for output from the calculator.  Default is maximum "
		   "precision.")
		  // --------------------------------------------------------------
		  ("default_worksheet,d",
		   po::value<string>(&gDefaultWorksheet)->default_value("1"),
		   "Worksheet to operate on when no worksheet ref is otherwise specified.  "
		   "Default is the first worksheet.")
		  // -----------------------------------------------------------------
		  ("titles_of_worksheets,b",
		   po::bool_switch(&gWriteWorksheetTitles),
		   "Write out the titles of each worksheet.  Nothing else.")
		  // -----------------------------------------------------------------
		  ("titles_of_columns,t",
		   po::bool_switch(&gWriteColumnTitles),
		   "Write out the titles of each column.  Nothing else.")
		  // -----------------------------------------------------------------
		  ("titles_of_rows,y",
		   po::bool_switch(&gWriteRowTitles),
		   "Write out the titles of each row.  Nothing else.")
		  // -----------------------------------------------------------------
		  ("cell_refs,r",
		   po::bool_switch(&gWriteCellRefs),
		   "Write out all the valid cell refs that are possible in calc expressions.  "
		   "Nothing else.")
		  // -----------------------------------------------------------------
		  ("enumerate_rows,n",
		   po::bool_switch(&gEnumerateRows)->default_value(false),
		   "Prefix the row number to each line of output.  Default is off.")
		  // -----------------------------------------------------------------
		  ("row_titles_column,m",
		   po::value<string>(&gRowTitlesColumn)->default_value("1"),
		   "Column which contains the titles for each row.  Default is the first column.")
		  // -----------------------------------------------------------------
		  ("column_titles_row,w",
		   po::value<int>(&gColumnTitlesRow)->default_value(1),
		   "Row which contains the titles for each column.  Default is the first row.")
		  // -----------------------------------------------------------------
		  ("column_titles_row_count,s",
		   po::value<int>(&gColumnTitleRowCount)->default_value(1),
		   "Number of rows that the column titles span over.  Defaults to one row.")
		  // -----------------------------------------------------------------
		  ("no_row_titles,j",
		   po::bool_switch(&gNoRowTitles)->default_value(false),
		   "Worksheet does not contain titles before the rows.  Default is false which "
		   "means the worksheet _does_ contain titles for each row of data.")
		  // -----------------------------------------------------------------
		  ("no_column_titles,k",
		   po::bool_switch(&gNoColumnTitles)->default_value(false),
		   "Worksheet does not contain titles over the columns.  Default is false which "
		   "means the worksheet _does_ contain titles over the columns.")
		  // -----------------------------------------------------------------
		  ("write_all_fields,a",
		   po::bool_switch(&gWriteAllFields)->default_value(false),
		   "Write out all fields of every Element in the XML file.")
		  // -----------------------------------------------------------------
		  ;

		// Hidden options are allowed both on the command line and in the config
		// file, but wont be shown in the user help.
		gHidden.add_options()
		  // -----------------------------------------------------------------
		  ("in-file", po::value<VectorString>(), "Positional list of files.")
		  // -----------------------------------------------------------------
		  ("stdin", po::value<string>()->default_value("false"), "Take input from stdin.")
		  // -----------------------------------------------------------------
		  ;

		gPositional.add("in-file", -1);
		gCmdLine.add(gGeneric).add(gHidden);
	}


	void Program::perform_requested_operation()
	{
		using namespace std;
		if (gVM.count("in-file") == 0)
			throw std::runtime_error{ "No files were listed on the command line." };
		vector<string> const& in_files = gVM["in-file"].as<vector<string>>();
		for (auto const& xml_filename : in_files) {
			m_visited_row = 0;
			m_visited_col = 0;
			m_each_calc_constraint.clear();
			if (gVerbose)
				*gErr << "Parsing " << xml_filename << endl;
			load_xml_file(xml_filename);

			if (gWriteAllFields) {
				m_documents.back().write_all_fields();
			}

			if (gWriteWorksheetTitles) {
				write_worksheet_titles();
				continue;
			}

			if (gWriteColumnTitles) {
				write_column_titles();
				continue;
			}

			if (gWriteRowTitles) {
				write_row_titles();
				continue;
			}

			if (gWriteCellRefs) {
				write_cell_refs();
				continue;
			}

			if (!gCalcFile.empty())
				compute_calc_file_and_write_results(); // gCalcFile

			if (!gCalcText.empty())
				compute_calc_and_write_results(); // gCalcText

			if (!gXPathText.empty())
				compute_xpath_and_write_results(); // gXPathText
		}
	}
} // namespace operations
