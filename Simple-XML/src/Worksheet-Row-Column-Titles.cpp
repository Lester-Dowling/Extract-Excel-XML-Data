/**
 * @file   Simple-XML/src/Worksheet-Row-Column-Titles.cpp
 * @date   Started 2019-04-11
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
namespace simple_xml {

	void Worksheet_Row_Column_Titles::throw_if_existing_wkt(const int wkt_idx) const
	{
		if (m_wrc_titles.count(wkt_idx) != 1)
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}


	void Worksheet_Row_Column_Titles::throw_if_non_existing_wkt(const int wkt_idx) const
	{
		if (m_wrc_titles.count(wkt_idx) != 0)
			throw runtime_error{ "Worksheet already existing at idx " +
								 to_string(wkt_idx) };
	}


	void Worksheet_Row_Column_Titles::add_worksheet(const int wkt_idx, const string title)
	{
		throw_if_non_existing_wkt(wkt_idx);
		m_wrc_titles.emplace(std::make_pair(wkt_idx, Row_Column_Titles{ title }));
		m_previous_wkt_idx = wkt_idx;
	}


	int Worksheet_Row_Column_Titles::add_worksheet(const string title)
	{
		const int wkt_idx = 1 + m_previous_wkt_idx;
		this->add_worksheet(wkt_idx, title);
		return wkt_idx;
	}


	void Worksheet_Row_Column_Titles::add_row(
	  const int wkt_idx,
	  const int row_idx,
	  const string title)
	{
		throw_if_existing_wkt(wkt_idx);
		m_wrc_titles[wkt_idx].add_row(row_idx, title);
	}


	int Worksheet_Row_Column_Titles::add_row(const int wkt_idx, const string title)
	{
		throw_if_existing_wkt(wkt_idx);
		return m_wrc_titles[wkt_idx].add_row(title);
	}


	void Worksheet_Row_Column_Titles::add_col(
	  const int wkt_idx,
	  const int col_idx,
	  const string title)
	{
		throw_if_existing_wkt(wkt_idx);
		m_wrc_titles[wkt_idx].add_col(col_idx, title);
	}


	int Worksheet_Row_Column_Titles::add_col(const int wkt_idx, const string title)
	{
		throw_if_existing_wkt(wkt_idx);
		return m_wrc_titles[wkt_idx].add_col(title);
	}


	size_t Worksheet_Row_Column_Titles::row_count(const int wkt_idx, const bool throwing)
	  const
	{
		if (throwing)
			throw_if_existing_wkt(wkt_idx);
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_count();
		else
			return 0;
	}


	size_t Worksheet_Row_Column_Titles::col_count(const int wkt_idx, const bool throwing)
	  const
	{
		if (throwing)
			throw_if_existing_wkt(wkt_idx);
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_count();
		else
			return 0;
	}


	vector<int> Worksheet_Row_Column_Titles::wkt_indices() const
	{
		vector<int> indices;
		for (auto const& p : m_wrc_titles)
			indices.push_back(p.first);
		return indices;
	}


	vector<int> Worksheet_Row_Column_Titles::row_indices(
	  const int wkt_idx,
	  const bool throwing) const
	{
		if (throwing)
			throw_if_existing_wkt(wkt_idx);
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_indices();
		else
			return {};
	}


	vector<int> Worksheet_Row_Column_Titles::col_indices(
	  const int wkt_idx,
	  const bool throwing) const
	{
		if (throwing)
			throw_if_existing_wkt(wkt_idx);
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_indices();
		else
			return {};
	}


	optional<string> Worksheet_Row_Column_Titles::wkt_title(const int wkt_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.wkt_title();
		else
			return {};
	}


	optional<string> Worksheet_Row_Column_Titles::row_title(
	  const int wkt_idx,
	  const int row_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.row_title(row_idx);
		else
			return {};
	}


	optional<string> Worksheet_Row_Column_Titles::col_title(
	  const int wkt_idx,
	  const int col_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.col_title(col_idx);
		else
			return {};
	}


	Row_Column_Titles& Worksheet_Row_Column_Titles::rc_titles(const int wkt_idx)
	{
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second;
		else
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}


	Row_Column_Titles const& Worksheet_Row_Column_Titles::rc_titles(const int wkt_idx) const
	{
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second;
		else
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}

} // namespace simple_xml
