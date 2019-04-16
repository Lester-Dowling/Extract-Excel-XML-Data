/**
 * @file   Operations/src/Program.cpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#include "pch-operations.hpp"
#include "Operations/Program.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Simple-XML-Parser/mini-grammar.hpp"
#include "Simple-XML-Parser/Node-Filter.hpp"
#include "Pseudo-XPath-Parser/mini-grammar.hpp"
#include "Operations/string-functions.hpp"

namespace operations {
	using std::endl;
	using std::setw;
	using std::string;
	using std::vector;
	using std::runtime_error;
	namespace ascii = boost::spirit::ascii;
	namespace qi = boost::spirit::qi;
	using file_in_memory_t = Program::file_in_memory_t;
	using Memory_Iterator = Program::Memory_Iterator;
	using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
	using Node = excel_xml_parser::Node;
	using Grade = pseudo_xpath_parser::Grade;
	using String_Iterator = string::const_iterator;
	using XPath_Grammar = pseudo_xpath_parser::mini_grammar<String_Iterator>;
	using XML_Grammar = excel_xml_parser::mini_grammar<Memory_Iterator>;

	const char* const all_data_xpath_text = //
	  " Row "
	  " , "
	  " Cell "
	  " , "
	  " Data ";

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

	std::string Program::xpath_prefix()
	{
		std::ostringstream xp;
		xp << "Workbook, Worksheet, Table, ";
		return xp.str();
	}

	void Program::extract_worksheet_titles(Node::SP xml_root)
	{
		using std::runtime_error;
		namespace ascii = boost::spirit::ascii;
		namespace qi = boost::spirit::qi;
		const string xpath_text{ "Workbook, Worksheet" };
		XPath_Grammar xpath_parser;
		String_Iterator sitr = xpath_text.begin();
		String_Iterator const send = xpath_text.end();
		if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
			throw runtime_error{ "Failed to parse this XPath for worksheet titles: " +
								 xpath_text };
		excel_xml_parser::Node_Filter::all_siblings(
		  xml_root,
		  xpath_parser.result,
		  m_titles,
		  [&](excel_xml_parser::Node_Visitor& visitor) -> bool //
		  {
			  if (visitor.name() == "Worksheet") {
				  if (visitor.attribute("ss:Name").has_value()) {
					  m_titles->add_worksheet(visitor.wkt(), *visitor.attribute("ss:Name"));
				  }
				  else {
					  m_titles->add_worksheet(visitor.wkt(), std::to_string(visitor.wkt()));
				  }
			  }
			  return true;
		  });
	}

	void Program::write_worksheet_titles()
	{
		for (const int wkt_idx : m_titles->wkt_indices()) {
			*gOut << "Worksheet #" << setw(3) << wkt_idx << " --> "
				  << *m_titles->wkt_title(wkt_idx) << endl;
		}
	}

	void Program::extract_column_titles(Node::SP xml_root)
	{
		std::ostringstream titles_xpath_oss;
		titles_xpath_oss << "Row[" << gColumnTitlesRow << "],Cell,Data[ss:Type=String]";
		for (const int wkt_idx : m_titles->wkt_indices()) {
			const string xpath_text{ this->xpath_prefix(wkt_idx) + titles_xpath_oss.str() };
			XPath_Grammar xpath_parser;
			String_Iterator sitr = xpath_text.begin();
			String_Iterator const send = xpath_text.end();
			if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
				throw runtime_error{ "Failed to parse this XPath for column titles: " +
									 xpath_text };
			excel_xml_parser::Node_Filter::all_siblings(
			  xml_root,
			  xpath_parser.result,
			  m_titles,
			  [&](excel_xml_parser::Node_Visitor& visitor) -> bool //
			  {
				  assert(visitor.wkt == wkt_idx);
				  assert(visitor.row() == gColumnTitlesRow);
				  assert(visitor.name() == "Data");
				  if (!visitor.text().empty()) {
					  m_titles->add_col(visitor.wkt(), visitor.col(), visitor.text());
				  }
				  return true;
			  });
		}
	}

	void Program::write_column_titles()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_titles->wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> cols{ m_titles->col_indices(*wkt_idx) };
			for (Iterator col_idx = cols.begin(); col_idx != cols.end(); col_idx++) {
				*gOut << "Worksheet #" << *wkt_idx << ' ' << *m_titles->wkt_title(*wkt_idx)
					  << " Column #" << setw(3) << *col_idx << " --> "
					  << *m_titles->col_title(*wkt_idx, *col_idx) << endl;
			}
		}
	}

	void Program::extract_row_titles(Node::SP xml_root)
	{
		if (gRowTitlesColumn.empty())
			gRowTitlesColumn = "1";
		else {
			// Translate a single capital letter to a column number.  For example, given "-m
			// C" on the command line, translate that to "-m 3".
			if (gRowTitlesColumn.size() == 1) {
				if (std::isupper(gRowTitlesColumn.front())) {
					const int corresponding_column_number =
					  1 + static_cast<int>(gRowTitlesColumn.front()) -
					  static_cast<int>('A');
					gRowTitlesColumn = std::to_string(corresponding_column_number);
				}
			}
		}
		bool good_column_number = true;
		int column_number;
		try {
			column_number = std::stoi(gRowTitlesColumn);
		}
		catch (std::invalid_argument const&) {
			good_column_number = false;
		}

		const string col_name_filter{ "[Column=\"" + gRowTitlesColumn + "\"]" };
		const string col_number_filter{ "[Column=" + gRowTitlesColumn + "]" };
		const string col_filter = good_column_number ? col_number_filter : col_name_filter;
		const string titles_xpath_text{ "Row,Cell" + col_filter + ",Data[ss:Type=String]" };
		for (const int wkt_idx : m_titles->wkt_indices()) {
			const string xpath_text{ this->xpath_prefix() + titles_xpath_text };
			XPath_Grammar xpath_parser;
			String_Iterator sitr = xpath_text.begin();
			String_Iterator const send = xpath_text.end();
			if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
				throw runtime_error{ "Failed to parse this XPath for column titles: " +
									 xpath_text };
			excel_xml_parser::Node_Filter::all_siblings(
			  xml_root,
			  xpath_parser.result,
			  m_titles,
			  [&](excel_xml_parser::Node_Visitor& visitor) -> bool //
			  {
				  if (good_column_number)
					  assert(visitor.col() == column_number);
				  assert(visitor.wkt() == wkt_idx);
				  assert(visitor.name() == "Data");
				  m_titles->add_row(visitor.wkt(), visitor.row(), visitor.text());
				  return true;
			  });
		}
	}

	void Program::write_row_titles()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_titles->wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> rows{ m_titles->row_indices(*wkt_idx) };
			for (Iterator row_idx = rows.begin(); row_idx != rows.end(); row_idx++) {
				*gOut << "Worksheet #" << *wkt_idx << ' ' << *m_titles->wkt_title(*wkt_idx)
					  << " Row #" << setw(3) << *row_idx << " --> "
					  << *m_titles->row_title(*wkt_idx, *row_idx) << endl;
			}
		}
	}

	void Program::write_cell_refs()
	{
		typedef vector<int>::const_iterator Iterator;
		const vector<int> wkts{ m_titles->wkt_indices() };
		for (Iterator wkt_idx = wkts.begin(); wkt_idx != wkts.end(); wkt_idx++) {
			const vector<int> rows{ m_titles->row_indices(*wkt_idx) };
			const vector<int> cols{ m_titles->col_indices(*wkt_idx) };
			for (Iterator col_idx = cols.begin(); col_idx != cols.end(); col_idx++) {
				for (Iterator row_idx = rows.begin(); row_idx != rows.end(); row_idx++) {
					*gOut << '[' << *m_titles->wkt_title(*wkt_idx) << ']'			//
						  << '[' << *m_titles->row_title(*wkt_idx, *row_idx) << ']' //
						  << '[' << *m_titles->col_title(*wkt_idx, *col_idx) << ']' //
						  << endl;
				}
			}
		}
	}

	bool Program::write_all_fields_visit(excel_xml_parser::Node_Visitor& visitor)
	{
		if (!visitor.name().empty()) {
			*gOut << '(' << visitor.depth() << ')' << "Tag:    " << visitor.name() << endl;
			*gOut << "     Path:    " << visitor.path_to_string() << endl;
		}
		if (!visitor.attributes().empty()) {
			*gOut << "     Attr: ";
			for (auto attr : visitor.attributes()) {
				*gOut << attr.first << '=' << attr.second << "   ";
			}
			*gOut << endl;
		}
		if (!visitor.text().empty()) {
			*gOut << "     Text: " << visitor.text() << endl;
		}
		*gOut << "     Row: " << visitor.row() << endl;
		*gOut << "     Col: " << visitor.col() << endl;
		return true;
	}

	bool Program::write_text_visit(excel_xml_parser::Node_Visitor& visitor)
	{
		assert(visitor.wkt() == 1);
		if (m_visited_row == 0)
			m_visited_row = visitor.row();

		if (m_visited_row != visitor.row()) {
			m_visited_row = visitor.row();
			m_visited_col = 0;
			*gOut << endl;
		}

		if (gEnumerateRows && m_visited_col == 0)
			*gOut << std::setw(4) << visitor.row() << " \t ";
		else if (0 < m_visited_col)
			*gOut << " \t ";

		if (gEachArithmeticExpression.empty()) {
			// No arithmetic expression to evaluate.  Print raw text, only:
			*gOut << visitor.text();
		}
		else { // Else, parse arithmetic expression for each visited:
			try {
				gCalculator.set_symbol("DATA", std::stod(visitor.text()));
				*gOut << std::fixed << std::setprecision(this->precision())
					  << gCalculator.evaluate(gEachArithmeticExpression);
			}
			catch (std::invalid_argument const&) {
				// visitor.text() is not a number, so ignore arithmetic expression:
				*gOut << visitor.text();
			}
		}
		m_visited_col = visitor.col();
		return true;
	}

	bool Program::one_data_visit(excel_xml_parser::Node_Visitor& visitor)
	{
		m_one_data = visitor.text();
		return false;
	}

	string Program::extract_single_text(Node::SP xml_root, Grade::SP xpath_root)
	{
		m_one_data.clear();
		excel_xml_parser::Node_Filter::all_siblings(
		  xml_root, xpath_root, m_titles, boost::bind(&Program::one_data_visit, this, _1));
		return m_one_data;
	}

	double Program::extract_single_number(Node::SP xml_root, Grade::SP xpath_root)
	{
		m_one_data.clear();
		excel_xml_parser::Node_Filter::all_siblings(
		  xml_root, xpath_root, m_titles, boost::bind(&Program::one_data_visit, this, _1));
		return std::stod(erase_commas(m_one_data));
	}

	Node::SP Program::load_xml_file(const f::path xml_path)
	{
		if (!f::exists(xml_path))
			throw runtime_error{ "No such file: " + xml_path.string() };
		f::fstream xml_stream{ xml_path };
		if (!xml_stream)
			throw runtime_error{ "Could not open file: " + xml_path.string() };
		boost::system::error_code ec;
		const boost::uintmax_t xml_file_size = f::file_size(xml_path, ec);
		if (ec || xml_file_size == static_cast<boost::uintmax_t>(-1))
			throw runtime_error{ "Failed to get XML file size." };
		if (gVerbose)
			*gErr << "Parsing " << xml_path.string() << endl;

		file_in_memory_t file_in_memory;
		file_in_memory.reserve(xml_file_size);
		xml_stream.unsetf(std::ios::skipws); // No white space skipping!
		std::copy(
		  std::istream_iterator<char>(xml_stream),
		  std::istream_iterator<char>(),
		  std::back_inserter(file_in_memory));

		Memory_Iterator fitr = file_in_memory.begin();
		Memory_Iterator const fend = file_in_memory.end();
		XML_Grammar xml_parser;
		if (!phrase_parse(fitr, fend, xml_parser, ascii::space))
			throw runtime_error{ "Parsing failed." };
		extract_worksheet_titles(xml_parser.result);
		if (gWriteWorksheetTitles) {
			write_worksheet_titles();
			throw No_Op{};
		}
		extract_column_titles(xml_parser.result);
		if (gWriteColumnTitles) {
			write_column_titles();
			throw No_Op{};
		}
		extract_row_titles(xml_parser.result);
		if (gWriteRowTitles) {
			write_row_titles();
			throw No_Op{};
		}
		if (gWriteCellRefs) {
			write_cell_refs();
			throw No_Op{};
		}
		return xml_parser.result;
	}

	Grade::SP Program::parse_xpath_text(const string xpath_text, const int wkt_idx)
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

	void Program::compute_xpath_and_write_results(Node::SP xml_root)
	{
		using std::string;
		using boost::regex;
		using boost::regex_match;
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

		excel_xml_parser::Node_Filter::all_siblings(
		  xml_root,
		  parse_xpath_text(full_xpath_text),
		  m_titles,
		  boost::bind(&Program::write_text_visit, this, _1));
		*gOut << endl;
	}

	void Program::compute_calc_and_write_results(Node::SP xml_root)
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
				  const string cell_text = extract_single_text(xml_root, xpath_root);
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
		   "Arithmetic expression to be worked out by the calculator.  Give either a "
		   "file "
		   "name for a calc script file; or, give a calc expression.")
		  // --------------------------------------------------------------
		  ("each,e",
		   po::value<string>(&gEachArithmeticExpression),
		   "Arithmetic expression to be worked out by the calculator.  The symbol DATA "
		   "in "
		   "the expression will be replaced by its value from the worksheet.  For "
		   "example, "
		   "to triple every extracted data value: --each='DATA * 3'")
		  // --------------------------------------------------------------
		  ("precision,p",
		   po::value<int>(&gPrecision)->default_value(-1),
		   "Number of decimal places for output from the calculator.  Default is "
		   "maximum "
		   "precision.")
		  // --------------------------------------------------------------
		  ("default_worksheet,d",
		   po::value<string>(&gDefaultWorksheet)->default_value("1"),
		   "The worksheet to operate on when no worksheet ref is otherwise specified.")
		  // -----------------------------------------------------------------
		  ("titles_of_worksheets,k",
		   po::bool_switch(&gWriteWorksheetTitles),
		   "Write out only the titles of each worksheet, nothing else.")
		  // -----------------------------------------------------------------
		  ("titles_of_columns,t",
		   po::bool_switch(&gWriteColumnTitles),
		   "Write out only the titles of each column, nothing else.")
		  // -----------------------------------------------------------------
		  ("titles_of_rows,y",
		   po::bool_switch(&gWriteRowTitles),
		   "Write out only the titles of each row, nothing else.")
		  // -----------------------------------------------------------------
		  ("cell_refs,r",
		   po::bool_switch(&gWriteCellRefs),
		   "Write out all the valid cell refs that are possible in calc expressions.")
		  // -----------------------------------------------------------------
		  ("enumerate_rows,n",
		   po::bool_switch(&gEnumerateRows),
		   "Prefix the row number to each line of output.")
		  // -----------------------------------------------------------------
		  ("row_titles_column,m",
		   po::value<string>(&gRowTitlesColumn),
		   "The column which contains the titles for each row.  Default is the first "
		   "column.")
		  // -----------------------------------------------------------------
		  ("column_titles_row,w",
		   po::value<int>(&gColumnTitlesRow)->default_value(1),
		   "The row which contains the titles for each column.  Default is the first "
		   "row.")
		  // -----------------------------------------------------------------
		  ("column_titles_span,s",
		   po::value<size_t>(&gColumnTitleSpan),
		   "The number of rows that the column titles span over.  Defaults to one row.")
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
			throw std::runtime_error{ "No Excel files were given." };
		VectorString const& in_files = gVM["in-file"].as<VectorString>();
		for (auto const& file_path : in_files) {
			m_titles->clear();
			m_visited_row = 0;
			m_visited_col = 0;
			Node::SP xml_root = load_xml_file(file_path);
			if (!gCalcText.empty()) {
				// Try gCalcText first as a filename:
				f::path calc_script_path{ gCalcText };
				if (f::exists(calc_script_path)) {

					f::fstream calc_script_stream{ calc_script_path };
					if (!calc_script_stream)
						throw runtime_error{ "Could not open file: " +
											 calc_script_path.string() };
					boost::system::error_code ec;
					const boost::uintmax_t file_size = f::file_size(calc_script_path, ec);
					if (ec || file_size == static_cast<boost::uintmax_t>(-1))
						throw runtime_error{ "Failed to get calc script file size." };
					if (gVerbose)
						*gErr << "Loading calc script " << calc_script_path.string()
							  << endl;

					// Load the calc script into gCalcText:
					gCalcText.clear();
					gCalcText.reserve(file_size);
					calc_script_stream.unsetf(std::ios::skipws); // No white space skipping!
					std::copy(
					  std::istream_iterator<char>(calc_script_stream),
					  std::istream_iterator<char>(),
					  std::back_inserter(gCalcText));
				}
				// Compute the calc expression in gCalcText:
				compute_calc_and_write_results(xml_root);
			}
			if (!gXPathText.empty())
				compute_xpath_and_write_results(xml_root);
		}
	}
} // namespace operations
