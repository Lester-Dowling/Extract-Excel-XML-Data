#undef TRACE_VISITOR

#ifdef NDEBUG
#undef TRACE_VISITOR
#endif



typedef boost::spirit::basic_istream_iterator<char> Stream_Iterator;

typedef client::mini_xpath_grammar<Stream_Iterator> XPath_Grammar;
XPath_Grammar xpath_grammar;

std::istringstream sample_stream{ xpath_text };
sample_stream.unsetf(std::ios::skipws);
sample_stream >> qi::phrase_match(xpath_grammar, ascii::space);
if (!(sample_stream.good() || sample_stream.eof()))
	throw runtime_error{ "Failed to parse XPath" };
cout << "XPath == " << XPath_Grade::path(xpath_grammar.result) << endl;

typedef fusion::vector<CharSeq, char, CharSeq> filter_attribute;

qi::rule<Iterator, char(), ascii::space_type> filter_op;
qi::rule<Iterator, filter_attribute(), ascii::space_type> filter;
qi::rule<Iterator, void(), ascii::space_type> attributes;
start_type start;

, filter_op{ qi::char_("=<>"), "filter_op" }, filter
{
	attribute_name >> filter_op >
	  (attribute_value_dq | attribute_value_sq | attribute_value_nq),
	  "filter"
}

// << phoenix::val(" here: \"")
// << phoenix::construct<std::string>(qi::_3, qi::_2)   //
// iterators to error-pos, end
// << phoenix::val("\"")


// qi::rule<Iterator, filter_attribute(), ascii::space_type> filter_dq;
// qi::rule<Iterator, filter_attribute(), ascii::space_type> filter_sq;
// qi::rule<Iterator, filter_attribute(), ascii::space_type> filter_nq;

//, filter_dq{ attribute_name >> filter_op >> attribute_value_dq, "filter_dq" }
//, filter_sq{ attribute_name >> filter_op >> attribute_value_sq, "filter_sq" }
//, filter_nq{ attribute_name >> filter_op >> attribute_value_nq, "filter_nq" }
//, filter{ filter_dq | filter_sq | filter_nq, "filter" }


// << phx::val(" here: \"")
// << phx::construct<std::string>(qi::_3, qi::_2)   //
// iterators to error-pos, end
// << phx::val("\"")


try {
	std::stod(row_ref);
	cell_ref_xpath_text += row_ref;
}
catch (std::invalid_argument const&) {
	cell_ref_xpath_text += '\'';
	cell_ref_xpath_text += row_ref;
	cell_ref_xpath_text += '\'';
}

try {
	std::stod(col_ref);
	cell_ref_xpath_text += col_ref;
}
catch (std::invalid_argument const&) {
	cell_ref_xpath_text += '\'';
	cell_ref_xpath_text += col_ref;
	cell_ref_xpath_text += '\'';
}


// -----------------------------------------------------------------
("columns_skip,k",
 po::value<size_t>(&gSkipCols),
 "The number of columns to ignore at the left of the worksheet.")



  /**
   * The number of columns to skip from the left of the Excel worksheet.
   */
  size_t gSkipCols = 0;

// -----------------------------------------------------------------
("rows_skip,r",
 po::value<size_t>(&gSkipRows),
 "The number of rows to ignore at the top of the worksheet.")


  /**
   * The number of rows to skip from the top of the Excel worksheet.
   */
  size_t gSkipRows = 0;



BOOST_AUTO_TEST_CASE(wrc_add_row_col)
{
	operations::Worksheet_Column_Row_Titles t;
	t.add_worksheet(1, "one wkt");
	t.add_worksheet(2, "two wkt");
	t.add_worksheet(3, "three wkt");
	t.add_worksheet(4, "four wkt");
	t.add_worksheet(5, "five wkt");
	t.add_worksheet(6, "six wkt");
	BOOST_TEST(t.row_count(1) == 0);
	BOOST_TEST(t.row_count(2) == 0);
	BOOST_TEST(t.row_count(3) == 0);
	t.add_row(1, 1, "one row");
	t.add_row(1, 2, "two row");
	t.add_row(1, 3, "three row");
	BOOST_TEST(t.row_count(1) == 3);
	BOOST_TEST(t.row_count(2) == 0);
	BOOST_TEST(t.row_count(3) == 0);
	BOOST_TEST(t.row_title(1, 1).value() == "one row");
	BOOST_TEST(t.row_title(1, 2).value() == "two row");
	BOOST_TEST(t.row_title(1, 3).value() == "three row");
	BOOST_TEST(t.col_count(1) == 0);
	BOOST_TEST(t.col_count(2) == 0);
	BOOST_TEST(t.col_count(3) == 0);
	t.add_col(1, 1, "one col");
	t.add_col(1, 2, "two col");
	t.add_col(1, 3, "three col");
	BOOST_TEST(t.col_count(1) == 3);
	BOOST_TEST(t.col_count(2) == 0);
	BOOST_TEST(t.col_count(3) == 0);
	BOOST_TEST(t.col_title(1, 1).value() == "one col");
	BOOST_TEST(t.col_title(1, 2).value() == "two col");
	BOOST_TEST(t.col_title(1, 3).value() == "three col");
}



// string const& name() const { return current().name(); }

// string const& text() const { return current().text(); }

// map<string, string> const& attributes() const { return current().attributes; };

// vector<Element::Index> const& children() const { return current().children; }

// int col_idx() const { return current().col_idx; }

// int row_idx() const { return current().row_idx; }

// int wkt_idx() const { return current().wkt_idx; }

// optional<string> attribute(string attribute_name) const
//{
//	return current().attribute(attribute_name);
//}



/**
 * Find and put into @c m_titles all the worksheet names in the workbook.
 */
void extract_worksheet_titles(Node::SP xml_root);

/**
 * Find and put into m_titles all the column titles in the workbook.
 */
void extract_column_titles(Node::SP);

/**
 * Find and put into m_titles all the row titles in the workbook.
 */
void extract_row_titles(Node::SP);

/**
 * Visit all XML nodes and print all fields of the nodes.
 */
bool write_all_fields_visit(excel_xml_parser::Node_Visitor&);



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
				  1 + static_cast<int>(gRowTitlesColumn.front()) - static_cast<int>('A');
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
