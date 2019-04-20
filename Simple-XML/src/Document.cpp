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
#include "Pseudo-XPath-Parser/mini-grammar.hpp"

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
	using XPath_Grammar = pseudo_xpath_parser::mini_grammar<String_Iterator>;

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


	void Document::extract_worksheet_titles()
	{
		const string xpath_text{ "Workbook, Worksheet" };
		XPath_Grammar xpath_parser;
		String_Iterator sitr = xpath_text.begin();
		String_Iterator const send = xpath_text.end();
		if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
			throw runtime_error{ "Failed to parse this XPath for worksheet titles: " +
								 xpath_text };
		Element_Filter ef{ m_elements, m_titles };
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

	
	void Document::extract_column_titles()
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
		Element_Filter ef{ m_elements, m_titles };
		ef.visit_all_depth_first(
		  [&](Element_Visitor& visitor) -> bool //
		  {
			  Element& ele = visitor.current();
				  assert(ele.wkt_idx == wkt_idx);
				  assert(ele.row_idx == gColumnTitlesRow);
				  assert(ele.name() == "Data");
				  if (!ele.text().empty()) {
					  m_titles->add_col(ele.wkt_idx, ele.col_idx, ele.text());
				  }
				  return true;
			  });
		}
	}

} // namespace simple_xml
