


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
