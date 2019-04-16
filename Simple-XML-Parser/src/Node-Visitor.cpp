// Simple-XML-Parser/src/Node-Visitor.cpp
// Started 14 Aug 2018
#include "pch-xml-parser.hpp"
#include "Simple-XML-Parser/Node-Visitor.hpp"
#define TRACE_VISITOR
#undef TRACE_VISITOR

#ifdef NDEBUG
#undef TRACE_VISITOR
#endif

namespace excel_xml_parser {

	Node_Visitor::Node_Visitor(excel_xml_parser::Node::SP root)
	  : current_node{ root }
	{
	}

	void Node_Visitor::throw_if_current_is_null() const
	{
		if (!current_node)
			throw std::runtime_error{ "Current XML Node is null." };
	}

	std::string Node_Visitor::name() const
	{
		throw_if_current_is_null();
		return current_node->name;
	}

	std::string Node_Visitor::text() const
	{
		throw_if_current_is_null();
		return current_node->text;
	}

	std::map<std::string, std::string> const& Node_Visitor::attributes() const
	{
		return current_node->attributes;
	}

	std::optional<std::string> Node_Visitor::attribute(std::string attribute_name) const
	{
		const auto fitr = current_node->attributes.find(attribute_name);
		if (fitr != current_node->attributes.end())
			return fitr->second;
		else
			return {};
	}


	bool Node_Visitor::visit_first_child()
	{
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << current_node->name << ' '
				  << current_node->children.size() << " children" << ')' << path()
				  << std::endl;
#endif
		if (current_node->children.empty())
			return false;
		current_node_path.push_back(current_node);
		current_node = current_node->children.front();
		return true;
	}

	bool Node_Visitor::resume_parent()
	{
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << current_node->name << ')'
				  << path() << std::endl;
#endif
		if (this->is_current_node_root())
			return false;
		current_node = current_node_path.back();
		current_node_path.pop_back();
		return true;
	}

	bool Node_Visitor::visit_next_sibling()
	{
		if (this->is_current_node_root())
			return false;
		excel_xml_parser::Node::SP parent = current_node_path.back();
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << parent->name << ' '
				  << parent->children.size() << " children" << ')' << path() << std::endl;
#endif
		bool take_next = false;
		for (auto sibling_node : parent->children) {
			if (take_next) {
				current_node = sibling_node;
				return true;
			}
			if (current_node == sibling_node) {
				take_next = true;
			}
		}
		return false;
	}

	std::string Node_Visitor::path_to_string() const
	{
		std::string result;
		Node_Path_Iterator nitr = current_node_path.begin();
		Node_Path_Iterator const nend = current_node_path.end();
		while (++nitr != nend) { // Elide root with "++nitr".
			excel_xml_parser::Node::SP node = *nitr;
			result += node->name;
			result += " --> ";
		}
		result += current_node->name;
		return result;
	}

	void Node_Visitor::all_depth_first(
	  excel_xml_parser::Node::SP root,
	  std::function<bool(Node_Visitor&)> callback)
	{
		Node_Visitor visitor{ root };
		while (true) {
			while (visitor.visit_first_child()) {
				if (!callback(visitor))
					return;
			}
			while (true) {
				if (visitor.visit_next_sibling()) {
					if (!callback(visitor))
						return;
					break;
				}
				else { // No more siblings => retreat to parent:
					if (!visitor.resume_parent())
						return;
				}
			}
		}
	}
} // namespace excel_xml_parser
