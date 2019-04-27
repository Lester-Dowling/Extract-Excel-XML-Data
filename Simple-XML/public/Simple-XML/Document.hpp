/**
 * @file   Simple-XML/Document.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Filter.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "Pseudo-XPath/Grade.hpp"

namespace simple_xml {

	/**
	 * A simple XML file represented in memory as a connected set of nodes.  Each node is
	 * represented by @c Element.
	 */
	class Document {
		/**
		 * The connected set of nodes.
		 */
		std::vector<Element> m_elements;

		/**
		 * Titles of all worksheets, rows and columns within the workbook.
		 */
		Worksheet_Row_Column_Titles m_titles;

		/**
		 * The Element filter over this Document.
		 */
		Element_Filter m_filter{ m_elements, m_titles };

		/**
		 * The column which contains the titles for each row.
		 */
		std::string m_row_titles_column; // gRowTitlesColumn

		/**
		 * The row which contains the titles for each column.
		 */
		int m_column_titles_row = 0; // gColumnTitlesRow

		/**
		 * How many rows do the column titles occupy.  Usually just one, though
		 * sometimes lengthy column titles span two or three rows.
		 */
		int m_column_title_span = 0; // gColumnTitleSpan

		/**
		 * The Excel XML file which this Document represents in memory.
		 */
		boost::filesystem::path m_excel_xml_path;

		/**
		 * Find all the worksheet titles.
		 */
		void extract_worksheet_titles();

		/**
		 * Visitor callback which extracts a single data cell.
		 */
		std::string m_one_data;
		bool one_data_visit(simple_xml::Element_Visitor&);

		/**
		 * Visitor callback to write out all fields of an Element.
		 */
		bool write_all_fields_visit(simple_xml::Element_Visitor&);

	public:
		Document() = default;

		/**
		 * Ctor to make a Document with the contents of the given Excel XML file.
		 */
		Document(const boost::filesystem::path xml_path);

		/**
		 * Load and parse the given Excel XML file into this document.
		 */
		void load_xml_file(const boost::filesystem::path xml_path);

		/**
		 * Find the column titles in each worksheet.
		 */
		void extract_column_titles(int column_titles_row = 1, int column_title_span = 1);

		/**
		 * Find the row titles in each worksheet.
		 */
		void extract_row_titles(std::string row_titles_column = "1");

		/**
		 * Accessor for the index of the first row where the data of the worksheet starts.
		 */
		int row_idx_start_of_data() const;

		/**
		 * Accessor that returns the Elements in this document.
		 */
		std::vector<Element> const& elements() const { return m_elements; }

		/**
		 * Accessor that returns the titles in this document.
		 */
		Worksheet_Row_Column_Titles const& titles() const { return m_titles; }

		/**
		 * Accessor for the pathname to the original Excel file.
		 */
		boost::filesystem::path const& path() const { return m_excel_xml_path; }

		/**
		 * Accessor to get an Element filter ready to query document.
		 */
		Element_Filter const& filter() const { return m_filter; }
		Element_Filter& filter() { return m_filter; }

		/**
		 * Query Document and return a single node's text as specified by the precise pseudo
		 * XPath.
		 */
		std::string extract_single_text(Grade::SP xpath_root);

		/**
		 * Query Document and return the number in a single node whose address is specified
		 * precisely from the @c xpath_root.
		 */
		double extract_single_number(Grade::SP xpath_root);

		/**
		 * Diagnostic utility function to write out all fields of every Element.
		 */
		void write_all_fields();
	};
} // namespace simple_xml
