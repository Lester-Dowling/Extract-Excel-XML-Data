/**
 * @file   Simple-XML/Document.cpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Document.hpp"
#include "Simple-XML/Element-Creator.hpp"
#include "Simple-XML/Element-Visitor.hpp"
#include "Simple-XML/Element-Filter.hpp"
#include "Simple-XML/mini-grammar.hpp"
#include "Pseudo-XPath/Grade.hpp"
#include "Pseudo-XPath/mini-grammar.hpp"
#include "IO-Extra/sequence.hpp"
#include "Strings-Extra/predicates.hpp"
#include "Strings-Extra/convert-and-translate.hpp"
#include "Strings-Extra/forming.hpp"
namespace simple_xml {
	using std::cout;
	using std::endl;
	using std::list;
	using std::runtime_error;
	using std::string;
	using std::vector;
	using std::optional;
	namespace ascii = boost::spirit::ascii;
	namespace qi = boost::spirit::qi;
	namespace f = boost::filesystem;
	namespace a = boost::algorithm;
	using file_in_memory_t = std::vector<char>;
	using Memory_Iterator = file_in_memory_t::const_iterator;
	using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
	using String_Iterator = string::const_iterator;
	using XML_Grammar = mini_grammar<Memory_Iterator>;
	using XPath_Grammar = pseudo_xpath::mini_grammar<String_Iterator>;
	using Grade = pseudo_xpath::Grade;

	Document::Document(const boost::filesystem::path xml_path)
	{
		this->load_xml_file(xml_path);
	}

	void Document::load_xml_file(const f::path xml_path)
	{
		m_excel_xml_path = xml_path;
		if (!f::exists(m_excel_xml_path))
			throw runtime_error{ "No such file: " + m_excel_xml_path.string() };
		f::ifstream xml_stream{ m_excel_xml_path };
		if (!xml_stream)
			throw runtime_error{ "Could not open file: " + m_excel_xml_path.string() };
		boost::system::error_code ec;
		const boost::uintmax_t xml_file_size = f::file_size(m_excel_xml_path, ec);
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

		extract_worksheet_titles();
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
		m_filter.set_filter_path(parse_xpath("Workbook, Worksheet"));
		m_filter.visit_all_depth_first(
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
			m_filter.set_filter_path(parse_xpath(titles_xpath_oss.str()));
			m_filter.visit_all_depth_first(
			  [&](Element_Visitor& visitor) -> bool //
			  {
				  Element& ele = visitor.current();
				  BOOST_ASSERT(ele.wkt_idx == wkt_idx);
				  BOOST_ASSERT(ele.row_idx == m_column_titles_row);
				  BOOST_ASSERT(ele.name() == "Data");
				  if (!ele.text().empty()) {
					  m_titles(wkt_idx).add_col(ele.col_idx, ele.text());
				  }
				  else {
					  m_titles(ele.wkt_idx)
						.add_col(ele.col_idx, std::to_string(ele.col_idx));
				  }
				  return true;
			  });
		}
	}


	/*static*/
	string Document::row_filter_columns(
	  const int wkt_idx,
	  const std::string row_titles_column_spec)
	{
		list<string> row_titles_column_list;
		a::split(
		  row_titles_column_list,
		  row_titles_column_spec,
		  a::is_any_of(" ,"),
		  a::token_compress_on);
		row_titles_column_list.remove_if([](string const& spec) { return spec.empty(); });

		if (row_titles_column_list.empty())
			throw std::runtime_error{
				"No column given to specify which column has the row titles."
			};
		using Iterator = list<string>::const_iterator;
		Iterator litr = row_titles_column_list.begin();
		Iterator const lend = row_titles_column_list.end();

		// See if there is a worksheet guard for this worksheet:
		std::ostringstream wkt_guard_oss;
		wkt_guard_oss << '[' << wkt_idx << ']';
		const string wkt_guard{ wkt_guard_oss.str() };
		Iterator gitr = row_titles_column_list.begin();
		for (; gitr != lend; ++gitr) {
			if (*gitr == wkt_guard) {
				litr = gitr;
				++litr;
				break;
			}
		}

		string column_filter;
		bool good_column_number = true;
		for (; litr != lend; ++litr) {
			std::string row_titles_column = *litr;
			if ( // Stop traversing the list if this item is a wkt guard:
			  row_titles_column.size() == 3		  //
			  && row_titles_column.front() == '[' //
			  && row_titles_column.back() == ']')
				break;
			strings::translate_column_title_uppercase_letter(row_titles_column);
			int column_number;
			try {
				column_number = std::stoi(row_titles_column);
				if (!column_filter.empty())
					column_filter += '|';
				column_filter += row_titles_column;
			}
			catch (std::invalid_argument const&) {
				good_column_number = false;
				if (column_filter.empty()) {
					return { "[Column=\"" + row_titles_column + "\"]" };
				}
				else {
					throw runtime_error{
						"Named columns not yet implemented for multi-column row titles."
					};
				}
			}
		}
		BOOST_ASSERT(good_column_number);
		if (column_filter.empty())
			column_filter = "1"; // Default is first column.
		return { "[" + column_filter + "]" };
	}


	void Document::extract_row_titles(std::string row_titles_column_spec)
	{
		BOOST_ASSERT(!row_titles_column_spec.empty());
		for (const int wkt_idx : m_titles.wkt_indices()) {
			std::ostringstream titles_xpath_oss;
			titles_xpath_oss << "Workbook,"									   //
							 << "Worksheet[" << wkt_idx << "], Table, "		   //
							 << "Row[Row>" << row_idx_start_of_data() << "], " //
							 << "Cell"
							 << row_filter_columns(wkt_idx, row_titles_column_spec)
							 << ", Data[ss:Type=String]";
			// cout << titles_xpath_oss.str() << endl;
			m_filter.set_filter_path(parse_xpath(titles_xpath_oss.str()));

			m_filter.visit_all_depth_first(
			  [&](Element_Visitor& visitor) -> bool //
			  {
				  Element& ele = visitor.current();
				  BOOST_ASSERT(ele.wkt_idx == wkt_idx);
				  BOOST_ASSERT(ele.name() == "Data");
				  if (!ele.text().empty()) {
					  m_titles(wkt_idx).add_row(ele.row_idx, ele.text());
				  }
				  else {
					  m_titles(ele.wkt_idx)
						.add_row(ele.row_idx, std::to_string(ele.row_idx));
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

	bool Document::one_data_visit(simple_xml::Element_Visitor& visitor)
	{
		m_one_data = visitor.current().text();
		return false;
	}

	std::string Document::extract_single_text(Grade::SP xpath_root)
	{
		m_one_data.clear();
		m_filter.set_filter_path(xpath_root)
		  .visit_all_depth_first(boost::bind(&Document::one_data_visit, this, _1));
		return m_one_data;
	}

	double Document::extract_single_number(Grade::SP xpath_root)
	{
		return std::stod(strings::erase_commas(extract_single_text(xpath_root)));
	}

	bool Document::write_all_fields_visit(simple_xml::Element_Visitor& visitor)
	{
		using std::cout;
		using std::endl;
		using std::setw;
		if (visitor.current().name() != "Data")
			return true;
		if (!visitor.current().name().empty()) {
			cout << setw(-12) << "Tag:" << ' ' << visitor.current().name() << endl;
			cout << setw(12) << "Depth:" << ' ' << visitor.depth() << endl;
			cout << setw(12) << "Path:" << ' ' << visitor.path_to_string() << endl;
		}
		if (!visitor.current().attributes.empty()) {
			cout << setw(12) << "Attr:" << ' ';
			for (auto attr : visitor.current().attributes) {
				cout << attr.first << '=' << attr.second << ' ' << ' ' << ' ';
			}
			cout << endl;
		}
		if (!visitor.current().text().empty()) {
			cout << setw(12) << "Text:" << ' ' << visitor.current().text() << endl;
		}
		cout << setw(12) << "Row:" << ' ' << visitor.current().row_idx << endl;
		cout << setw(12) << "Col:" << ' ' << visitor.current().col_idx << endl;
		if (const auto row_title =
			  m_titles(visitor.current().wkt_idx).row_title(visitor.current().row_idx);
			row_title.has_value()) //
		{
			cout << setw(12) << "Row Title:" << ' ' << *row_title << endl;
		}
		if (const auto col_title =
			  m_titles(visitor.current().wkt_idx).col_title(visitor.current().col_idx);
			col_title.has_value()) //
		{
			cout << setw(12) << "Col Title:" << ' ' << *col_title << endl;
		}
		return true;
	}

	void Document::write_all_fields()
	{
		Element_Visitor v{ m_elements };
		v.visit_all_depth_first(boost::bind(&Document::write_all_fields_visit, this, _1));
	}

} // namespace simple_xml
