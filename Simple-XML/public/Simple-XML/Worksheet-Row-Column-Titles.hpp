/**
 * @file   Simple-XML/Worksheet-Row-Column-Titles.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <map>
#include <utility>
#include <optional>
#include "Simple-XML/Row-Column-Titles.hpp"
namespace simple_xml {
	using std::map;
	using std::optional;
	using std::runtime_error;
	using std::string;
	using std::to_string;

	/**
	 * Keep all the titles of worksheets, rows and columns in a workbook.
	 */
	class Worksheet_Row_Column_Titles {
		map<int, Row_Column_Titles> m_wrc_titles;
		int m_previous_wkt_idx = 0;

	public: //~ Ctors -------------------------------------------------
		Worksheet_Row_Column_Titles() = default;

	public: //~ Mutators ----------------------------------------------
		/**
		 * Add a new worksheet title at the given idx to the titles map.
		 *
		 * @param[in] wkt_idx The worksheet index within the XML file.
		 *
		 * @param[in] title The title of the worksheet.
		 */
		void add_worksheet(const int wkt_idx, const string title);

		/**
		 * Add a new worksheet title at the idx of one plus the previous idx.
		 *
		 * @param[in] title The title of the worksheet.
		 *
		 * @return The idx of the added worksheet.
		 */
		int add_worksheet(const string title);

		/**
		 * Remove all titles and restore the titles map to empty.
		 */
		void clear()
		{
			m_wrc_titles.clear();
			m_previous_wkt_idx = 0;
		}

	public: //~ Accessors ---------------------------------------------
		/**
		 * @return Is this data structure empty?
		 */
		bool empty() const noexcept { return m_wrc_titles.empty(); }

		/**
		 * @return The number of worksheets in the titles map.
		 */
		size_t size() const noexcept { return m_wrc_titles.size(); }

		/**
		 * @return The number of worksheets in the titles map.
		 */
		size_t wkt_count() const noexcept { return m_wrc_titles.size(); }

		/**
		 * @return the Row-Columns Titles for the specified worksheet.
		 */
		Row_Column_Titles& operator()(const int wkt_idx);
		Row_Column_Titles const& operator()(const int wkt_idx) const;

		/**
		 * @return A sequence of all the worksheet indices in the titles map.
		 */
		vector<int> wkt_indices() const;

		/**
		 * @return The title of the given worksheet.
		 */
		optional<string> wkt_title(const int wkt_idx) const;

		/**
		 * @return A list of all the worksheet titles.
		 */
		vector<string> wkt_titles() const
		{
			vector<string> parsed_titles;
			for (const int wkt_idx : wkt_indices()) {
				parsed_titles.push_back(wkt_title(wkt_idx).value());
			}
			return parsed_titles;
		}
	};
} // namespace simple_xml
