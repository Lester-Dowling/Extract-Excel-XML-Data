// Simple-XML-Parser/Node-Filter.hpp
// Started 15 Aug 2018
#pragma once
#include <string>
#include <deque>
#include <initializer_list>
#include <functional>
#include "Simple-XML-Parser/Node.hpp"
#include "Simple-XML-Parser/Node-Visitor.hpp"
#include "Simple-XML-Parser/Worksheet-Row-Column-Titles.hpp"
#include "Pseudo-XPath/Grade.hpp"

namespace excel_xml_parser {

	class Node_Filter : public excel_xml_parser::Node_Visitor
	{
	  public:
		typedef std::shared_ptr<Node_Filter> SP;

		static void all_siblings(
		  excel_xml_parser::Node::SP root,
		  pseudo_xpath::Grade::SP filter_path,
		  Worksheet_Row_Column_Titles::SP titles,
		  std::function<bool(excel_xml_parser::Node_Visitor&)> callback);

	  protected:
		Node_Filter(excel_xml_parser::Node::SP root);

		void set_filter_path(pseudo_xpath::Grade::SP filter_path);

		using Filter_Node = pseudo_xpath::Grade::SP;
		Filter_Node m_filter_path;
		bool current_path_matches_filter_path() const;
		bool xml_node_matches_filter_node(
		  const excel_xml_parser::Node::SP node,
		  const Filter_Node fn) const;

		Worksheet_Row_Column_Titles::SP m_titles;
		void set_titles(Worksheet_Row_Column_Titles::SP t) { m_titles = t; }
	};
} // namespace excel_xml_parser
