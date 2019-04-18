// Simple-XML/Element.hpp
// Started 2019-04-18
#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace simple_xml {
	/**
	 * A single node within the XML document model.
	 */
	class Element {
	public:
		using Index = std::size_t;

		std::string name;
		std::map<std::string, std::string> attributes;
		std::string text;
		std::vector<Index> children;
		int row_idx; // The row number this node is in.
		int col_idx; // The column number this node is in.
		int wkt_idx; // The worksheet number this node is in.
	};
} // namespace simple_xml
