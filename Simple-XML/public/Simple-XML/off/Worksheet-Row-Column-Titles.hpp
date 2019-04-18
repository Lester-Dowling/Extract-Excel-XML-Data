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
#include <boost/noncopyable.hpp>
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
	class Worksheet_Row_Column_Titles : private boost::noncopyable {
		map<int, Column_Row_Titles> m_wrc_titles;
		int m_previous_wkt_idx = 0;

		/**
		 * Throw a @c runtime_error if the given idx does exist in the titles map.
		 */
		void verify_existing_idx(const int wkt_idx) const;

		/**
		 * Throw a @c runtime_error if the given idx has already been inserted into the
		 * titles map.
		 */
		void verify_non_existing_idx(const int wkt_idx) const;

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
		 * Add a new row to the titles map for the given worksheet idx.
		 *
		 * @param[in] wkt_idx The worksheet idx where the row is.
		 *
		 * @param[in] row_idx The idx of the row within the worksheet.
		 *
		 * @param[in] title The title of the row.
		 */
		void add_row(const int wkt_idx, const int row_idx, const string title);

		/**
		 * Add a new row to the titles map at the idx of one plus the previous idx.
		 *
		 * @param[in] wkt_idx The worksheet idx where the row is.
		 *
		 * @param[in] title The title of the row.
		 *
		 * @return The idx of the newly inserted row.
		 */
		int add_row(const int wkt_idx, const string title);

		/**
		 * Add a new column to the titles map for the given worksheet idx.
		 *
		 * @param[in] wkt_idx The worksheet idx where the column is.
		 *
		 * @param[in] col_idx The idx of the column within the worksheet.
		 *
		 * @param[in] title The title of the column.
		 */
		void add_col(const int wkt_idx, const int col_idx, const string title);

		/**
		 * Add a new column to the titles map at the idx of one plus the previous idx.
		 *
		 * @param[in] wkt_idx The worksheet idx where the column is.
		 *
		 * @param[in] title The title of the column.
		 *
		 * @return The idx of the newly inserted column.
		 */
		int add_col(const int wkt_idx, const string title);

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
		 * @return The number of worksheets in the titles map.
		 */
		size_t size() const noexcept { return m_wrc_titles.size(); }

		/**
		 * @return The number of worksheets in the titles map.
		 */
		size_t wkt_count() const noexcept { return m_wrc_titles.size(); }

		/**
		 * @return The number of rows within the given worksheet in the titles map.
		 */
		size_t row_count(const int wkt_idx, const bool throwing = true) const;

		/**
		 * @return The number of columns within the given worksheet in the titles map.
		 */
		size_t col_count(const int wkt_idx, const bool throwing = true) const;

		/**
		 * @return A sequence of all the worksheet indices in the titles map.
		 */
		vector<int> wkt_indices() const;

		/**
		 * @return A sequence of all the row indices within the given worksheet in the
		 * titles map.
		 */
		vector<int> row_indices(const int wkt_idx, const bool throwing = true) const;

		/**
		 * @return A sequence of all the column indices within the given worksheet in the
		 * titles map.
		 */
		vector<int> col_indices(const int wkt_idx, const bool throwing = true) const;

		/**
		 * @return The title of the given worksheet.
		 */
		optional<string> wkt_title(const int wkt_idx) const;

		/**
		 * @return The title of the row within the given worksheet.
		 */
		optional<string> row_title(const int wkt_idx, const int row_idx) const;

		/**
		 * @return The title of the column within the given worksheet.
		 */
		optional<string> col_title(const int wkt_idx, const int col_idx) const;
	};
} // namespace simple_xml
