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
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"

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
		 * The column which contains the titles for each row.
		 */
		std::string m_row_titles_column; // gRowTitlesColumn

		/**
		 * The row which contains the titles for each column.
		 */
		int m_column_titles_row; // gColumnTitlesRow

		/**
		 * How many rows do the column titles occupy.  Usually just one, though
		 * sometimes lengthy column titles span two or three rows.
		 */
		size_t m_column_title_span; // gColumnTitleSpan

		friend class Element_Filter;

	public:
		Document(
		  std::string row_titles_column,
		  int column_titles_row,
		  size_t column_title_span)
		  : m_row_titles_column{ row_titles_column }
		  , m_column_titles_row{ column_titles_row }
		  , m_column_title_span{ column_title_span }
		{
		}

		/**
		 * Load and parse the given XML file into this document.
		 */
		void load_xml_file(const boost::filesystem::path xml_path);

		/**
		 * Find all the worksheet titles.
		 */
		void extract_worksheet_titles();

		/**
		 * Find the column titles for each worksheet.
		 */
		void extract_column_titles();

		/**
		 * Accessor that returns the Elements in this document.
		 */
		std::vector<Element> const& elements() const { return m_elements; }

		/**
		 * Accessor that returns the titles in this document.
		 */
		Worksheet_Row_Column_Titles const& titles() const { return m_titles; }
	};
} // namespace simple_xml
