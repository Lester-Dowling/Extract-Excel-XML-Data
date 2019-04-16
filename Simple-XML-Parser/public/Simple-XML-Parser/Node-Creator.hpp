// Simple-XML-Parser/Node-Creator.hpp
// Started 15 Aug 2018
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Simple-XML-Parser/Node.hpp"
#include "Simple-XML-Parser/Node-Visitor.hpp"

namespace excel_xml_parser {

	class Node_Creator : public excel_xml_parser::Node_Visitor
	{
		int m_row_idx = 0; // Current row number.
		int m_col_idx = 0; // Current column number.
		int m_wkt_idx = 0; // Current worksheet number.

	  public:
		typedef std::shared_ptr<Node_Creator> SP;

		Node_Creator(excel_xml_parser::Node::SP top_node);

		void new_child();

		void new_element(std::vector<char> x);

		void set_attribute(std::string name, std::string value);

		bool close_singleton();

		bool verify_closing_tag(std::vector<char> x);

		void set_text(std::vector<char> y);
	};
} // namespace excel_xml_parser
