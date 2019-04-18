/**
 * @file   Simple-XML/Element.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace simple_xml {
	using std::map;
	using std::string;
	using std::vector;

	/**
	 * A single node within the XML document model.
	 */
	class Element {
	public:
		using Index = std::size_t;

		string name;
		map<string, string> attributes;
		string text;
		vector<Index> children;
		int row_idx; // The row number this node is in.
		int col_idx; // The column number this node is in.
		int wkt_idx; // The worksheet number this node is in.
	};
} // namespace simple_xml
