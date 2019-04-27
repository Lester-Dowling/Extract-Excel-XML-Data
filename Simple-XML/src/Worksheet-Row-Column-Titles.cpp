/**
 * @file   Simple-XML/src/Worksheet-Row-Column-Titles.cpp
 * @date   Started 2019-04-11
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
namespace simple_xml {

	void Worksheet_Row_Column_Titles::add_worksheet(const int wkt_idx, const string title)
	{
		if (m_wrc_titles.count(wkt_idx))
			throw runtime_error{ "Worksheet already exists at idx " + to_string(wkt_idx) };
		m_wrc_titles.emplace(std::make_pair(wkt_idx, Row_Column_Titles{ title }));
		m_previous_wkt_idx = wkt_idx;
	}


	int Worksheet_Row_Column_Titles::add_worksheet(const string title)
	{
		const int wkt_idx = 1 + m_previous_wkt_idx;
		this->add_worksheet(wkt_idx, title);
		return wkt_idx;
	}


	vector<int> Worksheet_Row_Column_Titles::wkt_indices() const
	{
		vector<int> indices;
		for (auto const& p : m_wrc_titles)
			indices.push_back(p.first);
		return indices;
	}


	optional<string> Worksheet_Row_Column_Titles::wkt_title(const int wkt_idx) const
	{
		const auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second.wkt_title();
		else
			return {};
	}


	Row_Column_Titles& Worksheet_Row_Column_Titles::operator()(const int wkt_idx)
	{
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second;
		else
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}


	Row_Column_Titles const& Worksheet_Row_Column_Titles::operator()(
	  const int wkt_idx) const
	{
		auto fitr = m_wrc_titles.find(wkt_idx);
		if (fitr != m_wrc_titles.end())
			return fitr->second;
		else
			throw runtime_error{ "No such worksheet at idx " + to_string(wkt_idx) };
	}

} // namespace simple_xml
