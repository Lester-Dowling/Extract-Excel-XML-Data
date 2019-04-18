/**
 * @file   Simple-XML/Document.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <map>
#include <memory>
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
	public:
		/**
		 * Load and parse the given XML file into this document.
		 */
		void load_xml_file(const boost::filesystem::path xml_path);

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

		/**
		 * The connected set of nodes.
		 */
		std::vector<Element> m_elements;
	};
} // namespace simple_xml
