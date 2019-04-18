/**
 * @file   Simple-XML/src/Worksheet-Row-Column-Titles.cpp
 * @date   Started 2019-04-11
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
namespace simple_xml {
	using W = Worksheet_Row_Column_Titles;

	void W::verify_existing_idx(const int wkt_idx) const
	{
		if (m_wrc_titles.count(wkt_idx) != 1)
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}

	void W::verify_non_existing_idx(const int wkt_idx) const
	{
		if (m_wrc_titles.count(wkt_idx) != 0)
			throw runtime_error{ "Worksheet already existing at idx " +
								 to_string(wkt_idx) };
	}

	void W::add_worksheet(const int wkt_idx, const string title)
	{
		verify_non_existing_idx(wkt_idx);
		m_wrc_titles.emplace(std::make_pair(wkt_idx, Column_Row_Titles{ title }));
		m_previous_wkt_idx = wkt_idx;
	}

	int W::add_worksheet(const string title)
	{
		const int wkt_idx = 1 + m_previous_wkt_idx;
		this->add_worksheet(wkt_idx, title);
		return wkt_idx;
	}

	void W::add_row(const int wkt_idx, const int row_idx, const string title)
	{
		verify_existing_idx(wkt_idx);
		m_wrc_titles[wkt_idx].add_row(row_idx, title);
	}

	int W::add_row(const int wkt_idx, const string title)
	{
		verify_existing_idx(wkt_idx);
		return m_wrc_titles[wkt_idx].add_row(title);
	}

	void W::add_col(const int wkt_idx, const int col_idx, const string title)
	{
		verify_existing_idx(wkt_idx);
		m_wrc_titles[wkt_idx].add_col(col_idx, title);
	}

	int W::add_col(const int wkt_idx, const string title)
	{
		verify_existing_idx(wkt_idx);
		return m_wrc_titles[wkt_idx].add_col(title);
	}

	size_t W::row_count(const int wkt_idx, const bool throwing) const
	{
		if (throwing)
			verify_existing_idx(wkt_idx);
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_count();
		else
			return 0;
	}

	size_t W::col_count(const int wkt_idx, const bool throwing) const
	{
		if (throwing)
			verify_existing_idx(wkt_idx);
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_count();
		else
			return 0;
	}

	vector<int> W::wkt_indices() const
	{
		vector<int> indices;
		for (auto const& p : m_wrc_titles)
			indices.push_back(p.first);
		return indices;
	}

	vector<int> W::row_indices(const int wkt_idx, const bool throwing) const
	{
		if (throwing)
			verify_existing_idx(wkt_idx);
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_indices();
		else
			return {};
	}

	vector<int> W::col_indices(const int wkt_idx, const bool throwing) const
	{
		if (throwing)
			verify_existing_idx(wkt_idx);
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_indices();
		else
			return {};
	}

	optional<string> W::wkt_title(const int wkt_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.wkt_title();
		else
			return {};
	}

	optional<string> W::row_title(const int wkt_idx, const int row_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_title(row_idx);
		else
			return {};
	}

	optional<string> W::col_title(const int wkt_idx, const int col_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_title(col_idx);
		else
			return {};
	}
} // namespace simple_xml
