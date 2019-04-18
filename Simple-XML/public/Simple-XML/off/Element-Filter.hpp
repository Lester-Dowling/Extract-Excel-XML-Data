/**
 * @file   Simple-XML/Element-Filter.hpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <functional>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Visitor.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "Pseudo-XPath-Parser/Grade.hpp"

namespace simple_xml {

	class Element_Filter : public Element_Visitor {
	public:
		typedef std::shared_ptr<Element_Filter> SP;

		static void all_siblings(
		  excel_xml_parser::Node::SP root,
		  pseudo_xpath_parser::Grade::SP filter_path,
		  Worksheet_Row_Column_Titles::SP titles,
		  std::function<bool(excel_xml_parser::Node_Visitor&)> callback);

	protected:
		Element_Filter(excel_xml_parser::Node::SP root);

		void set_filter_path(pseudo_xpath_parser::Grade::SP filter_path);

		using Filter_Node = pseudo_xpath_parser::Grade::SP;
		Filter_Node m_filter_path;
		bool current_path_matches_filter_path() const;
		bool xml_node_matches_filter_node(
		  const excel_xml_parser::Node::SP node,
		  const Filter_Node fn) const;

		Worksheet_Row_Column_Titles::SP m_titles;
		void set_titles(Worksheet_Row_Column_Titles::SP t) { m_titles = t; }
	};
} // namespace simple_xml
