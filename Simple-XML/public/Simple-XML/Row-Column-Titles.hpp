/**
 * @file   Simple-XML/Row-Column-Titles.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <memory>
#include <string>
#include <map>
#include <utility>
#include <optional>
namespace simple_xml {
	using std::map;
	using std::optional;
	using std::string;
	using std::vector;

	class Row_Column_Titles {
		typedef map<int, string> titles_map_t;

		string m_worksheet_title;
		titles_map_t m_col_titles, m_row_titles;
		int m_previous_row_idx = 0;
		int m_previous_col_idx = 0;

	public: //~ Ctors et al -------------------------------------------
		// typedef std::shared_ptr<Row_Column_Titles> SP;

		Row_Column_Titles() {} // Requirement for std::map.

		explicit Row_Column_Titles(const string wkt_title)
		  : m_worksheet_title{ wkt_title }
		{
		}

	public: //~ Mutators ----------------------------------------------
		void add_row(const int row_idx, const string title)
		{
			const string multiple_row_title_separator{ ", " };
			if (m_row_titles.count(row_idx))
				m_row_titles[row_idx] += multiple_row_title_separator + title;
			else
				m_row_titles[row_idx] = title;
			m_previous_row_idx = row_idx;
		}

		int add_row(const string title)
		{
			const int row_idx = m_previous_row_idx + 1;
			this->add_row(row_idx, title);
			return row_idx;
		}

		void add_col(const int col_idx, const string title)
		{
			m_col_titles[col_idx] = title;
			m_previous_col_idx = col_idx;
		}

		int add_col(const string title)
		{
			const int col_idx = m_previous_col_idx + 1;
			this->add_col(col_idx, title);
			return col_idx;
		}

	public: //~ Accessors ---------------------------------------------
		size_t row_count() const { return m_row_titles.size(); }

		size_t col_count() const { return m_col_titles.size(); }

		vector<int> row_indices() const
		{
			vector<int> indices;
			for (auto const& p : m_row_titles)
				indices.push_back(p.first);
			return indices;
		}

		vector<int> col_indices() const
		{
			vector<int> indices;
			for (auto const& p : m_col_titles)
				indices.push_back(p.first);
			return indices;
		}

		string wkt_title() const { return m_worksheet_title; }

		optional<string> row_title(const int row_idx) const
		{
			const auto fitr = m_row_titles.find(row_idx);
			if (fitr != m_row_titles.end())
				return fitr->second;
			else
				return {};
		}

		optional<string> col_title(const int col_idx) const
		{
			const auto fitr = m_col_titles.find(col_idx);
			if (fitr != m_col_titles.end())
				return fitr->second;
			else
				return {};
		}

		/**
		 * @return A list of all the row titles.
		 */
		vector<string> row_titles() const
		{
			vector<string> parsed_titles;
			for (const int row_idx : row_indices()) {
				parsed_titles.push_back(row_title(row_idx).value());
			}
			return parsed_titles;
		}

		/**
		 * @return A list of all the column titles.
		 */
		vector<string> col_titles() const
		{
			vector<string> parsed_titles;
			for (const int col_idx : col_indices()) {
				parsed_titles.push_back(col_title(col_idx).value());
			}
			return parsed_titles;
		}
	};

} // namespace simple_xml
