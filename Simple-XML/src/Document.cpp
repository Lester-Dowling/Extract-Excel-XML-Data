/**
 * @file   Simple-XML/Document.cpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Document.hpp"
#include "Simple-XML/Element-Creator.hpp"
#include "Simple-XML/Element-Filter.hpp"
#include "Simple-XML/mini-grammar.hpp"
#include "Pseudo-XPath/Grade.hpp"
#include "Pseudo-XPath/mini-grammar.hpp"

namespace simple_xml {
	using std::runtime_error;
	using std::string;
	using std::vector;
	namespace ascii = boost::spirit::ascii;
	namespace qi = boost::spirit::qi;
	namespace f = boost::filesystem;
	using file_in_memory_t = std::vector<char>;
	using Memory_Iterator = file_in_memory_t::const_iterator;
	using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
	using String_Iterator = string::const_iterator;
	using XML_Grammar = mini_grammar<Memory_Iterator>;
	using XPath_Grammar = pseudo_xpath::mini_grammar<String_Iterator>;
	using Grade = pseudo_xpath::Grade;


	void Document::load_xml_file(const f::path xml_path)
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

		file_in_memory_t file_in_memory;
		file_in_memory.reserve(xml_file_size);
		xml_stream.unsetf(std::ios::skipws); // No white space skipping!
		std::copy(
		  std::istream_iterator<char>(xml_stream),
		  std::istream_iterator<char>(),
		  std::back_inserter(file_in_memory));

		Memory_Iterator fitr = file_in_memory.begin();
		Memory_Iterator const fend = file_in_memory.end();
		Element_Creator element_creator{ this->m_elements };
		XML_Grammar xml_parser{ &element_creator };
		if (!phrase_parse(fitr, fend, xml_parser, ascii::space))
			throw runtime_error{ "Parsing failed." };
		// extract_worksheet_titles(xml_parser.result);
		// if (gWriteWorksheetTitles) {
		//	write_worksheet_titles();
		//	throw No_Op{};
		//}
		// extract_column_titles(xml_parser.result);
		// if (gWriteColumnTitles) {
		//	write_column_titles();
		//	throw No_Op{};
		//}
		// extract_row_titles(xml_parser.result);
		// if (gWriteRowTitles) {
		//	write_row_titles();
		//	throw No_Op{};
		//}
		// if (gWriteCellRefs) {
		//	write_cell_refs();
		//	throw No_Op{};
		//}
		// return xml_parser.result;
	}


	static Grade::SP parse_xpath(const string xpath_text)
	{
		XPath_Grammar xpath_parser;
		String_Iterator sitr = xpath_text.begin();
		String_Iterator const send = xpath_text.end();
		if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
			throw runtime_error{ "Failed to parse this XPath: " + xpath_text };
		return xpath_parser.result;
	}


	void Document::extract_worksheet_titles()
	{
		Element_Filter ef{ m_elements, m_titles };
		ef.set_filter_path(parse_xpath("Workbook, Worksheet"));
		ef.visit_all_depth_first(
		  [&](Element_Visitor& visitor) -> bool //
		  {
			  Element& ele = visitor.current();
			  if (ele.name() == "Worksheet") {
				  if (ele.attribute("ss:Name").has_value()) {
					  m_titles.add_worksheet(ele.wkt_idx, *ele.attribute("ss:Name"));
				  }
				  else {
					  m_titles.add_worksheet(ele.wkt_idx, std::to_string(ele.wkt_idx));
				  }
			  }
			  return true;
		  });
	}


	void Document::extract_column_titles(int column_titles_row, int column_title_span)
	{
		m_column_titles_row = column_titles_row;
		m_column_title_span = column_title_span;
		for (const int wkt_idx : m_titles.wkt_indices()) {
			std::ostringstream titles_xpath_oss;
			titles_xpath_oss << "Workbook, Worksheet[" << wkt_idx << "], Table, "
							 << "Row[" << m_column_titles_row
							 << "],Cell,Data[ss:Type=String]";
			Element_Filter ef{ m_elements, m_titles };
			ef.set_filter_path(parse_xpath(titles_xpath_oss.str()));
			ef.visit_all_depth_first(
			  [&](Element_Visitor& visitor) -> bool //
			  {
				  Element& ele = visitor.current();
				  BOOST_ASSERT(ele.wkt_idx == wkt_idx);
				  BOOST_ASSERT(ele.row_idx == m_column_titles_row);
				  BOOST_ASSERT(ele.name() == "Data");
				  if (!ele.text().empty()) {
					  m_titles.add_col(wkt_idx, ele.col_idx, ele.text());
				  }
				  else {
					  m_titles.add_col(
						ele.wkt_idx, ele.col_idx, std::to_string(ele.col_idx));
				  }
				  return true;
			  });
		}
	}


	void Document::extract_row_titles(const std::string row_titles_column)
	{
		m_row_titles_column = row_titles_column;
		if (m_row_titles_column.empty())
			m_row_titles_column = "1";
		else {
			// Translate a single capital letter to a column number.  For example, given "-m
			// C" on the command line, translate that to "-m 3".
			if (m_row_titles_column.size() == 1) {
				if (std::isupper(m_row_titles_column.front())) {
					const int corresponding_column_number =
					  1 + static_cast<int>(m_row_titles_column.front()) -
					  static_cast<int>('A');
					m_row_titles_column = std::to_string(corresponding_column_number);
				}
			}
		}
		bool good_column_number = true;
		int column_number;
		try {
			column_number = std::stoi(m_row_titles_column);
		}
		catch (std::invalid_argument const&) {
			good_column_number = false;
		}

		const string col_name_filter{ "[Column=\"" + m_row_titles_column + "\"]" };
		const string col_number_filter{ "[Column=" + m_row_titles_column + "]" };
		const string& col_filter = good_column_number ? col_number_filter : col_name_filter;
		for (const int wkt_idx : m_titles.wkt_indices()) {
			std::ostringstream titles_xpath_oss;
			titles_xpath_oss << "Workbook,"									   //
							 << "Worksheet[" << wkt_idx << "], Table, "		   //
							 << "Row[Row>" << row_idx_start_of_data() << "], " //
							 << "Cell" << col_filter << ", Data[ss:Type=String]";
			Element_Filter ef{ m_elements, m_titles };
			ef.set_filter_path(parse_xpath(titles_xpath_oss.str()));
			ef.visit_all_depth_first(
			  [&](Element_Visitor& visitor) -> bool //
			  {
				  Element& ele = visitor.current();
				  BOOST_ASSERT(ele.wkt_idx == wkt_idx);
				  BOOST_ASSERT(ele.name() == "Data");
				  if (good_column_number) {
					  BOOST_ASSERT(ele.col_idx == column_number);
				  }
				  if (!ele.text().empty()) {
					  m_titles.add_row(wkt_idx, ele.row_idx, ele.text());
				  }
				  else {
					  m_titles.add_row(
						ele.wkt_idx, ele.row_idx, std::to_string(ele.row_idx));
				  }
				  return true;
			  });
		}
	}

	int Document::row_idx_start_of_data() const
	{
		BOOST_ASSERT(m_column_titles_row > 0);
		BOOST_ASSERT(m_column_title_span > 0);
		return m_column_titles_row + m_column_title_span - 1;
	}

} // namespace simple_xml
