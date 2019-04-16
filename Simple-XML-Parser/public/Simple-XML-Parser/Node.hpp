// Simple-XML-Parser/Node.hpp
// Started 14 Aug 2018
#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace excel_xml_parser {
	class Node
	{
	  public:
		typedef std::shared_ptr<Node> SP;

		std::string name;
		std::map<std::string, std::string> attributes;
		std::string text;
		std::vector<SP> children;
		int row_idx; // The row number this node is in.
		int col_idx; // The column number this node is in.
		int wkt_idx; // The worksheet number this node is in.
	};
} // namespace excel_xml_parser
