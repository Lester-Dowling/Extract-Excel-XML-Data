// Simple-XML-Parser/Node-Visitor.hpp
// Started 15 Aug 2018
#pragma once
#include <deque>
#include <memory>
#include <string>
#include <optional>
#include <functional>
#include "Simple-XML-Parser/Node.hpp"

namespace excel_xml_parser {

	class Node_Visitor {
	public:
		typedef std::shared_ptr<Node_Visitor> SP;

		Node_Visitor(excel_xml_parser::Node::SP top_node);
		virtual ~Node_Visitor() = default;

		std::string name() const;

		std::string text() const;

		int col() const { return current_node->col_idx; }

		int row() const { return current_node->row_idx; }

		int wkt() const { return current_node->wkt_idx; }

		std::map<std::string, std::string> const& attributes() const;

		std::optional<std::string> attribute(std::string attribute_name) const;

		size_t depth() const { return current_node_path.size(); }

		std::string path_to_string() const;

		static void all_depth_first(
		  excel_xml_parser::Node::SP root,
		  std::function<bool(Node_Visitor&)> callback);

	protected:
		excel_xml_parser::Node::SP current_node;

		typedef std::deque<excel_xml_parser::Node::SP> Node_Path_Type;
		typedef std::deque<excel_xml_parser::Node::SP>::const_iterator Node_Path_Iterator;
		Node_Path_Type current_node_path;

		bool is_current_node_root() const { return current_node_path.empty(); }

		virtual bool resume_parent();

		virtual bool visit_first_child();

		virtual bool visit_next_sibling();

		void throw_if_current_is_null() const;
	};
} // namespace excel_xml_parser
