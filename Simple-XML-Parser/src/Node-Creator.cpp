// Simple-XML-Parser/src/Node-Creator.cpp
// Started 15 Aug 2018
#include "pch-xml-parser.hpp"
#include "Simple-XML-Parser/Node-Creator.hpp"
#define TRACE_CREATOR
#undef TRACE_CREATOR

#ifdef NDEBUG
#undef TRACE_CREATOR
#endif

namespace excel_xml_parser {

	Node_Creator::Node_Creator(excel_xml_parser::Node::SP root)
	  : Node_Visitor{ root }
	{
		root->name = "root";
	}

	void Node_Creator::set_text(std::vector<char> y)
	{
		current_node->text.assign(y.begin(), y.end());
		boost::algorithm::trim(current_node->text);
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->text
				  << " --- length = " << current_node->text.size() << std::endl;
#endif
	}

	void Node_Creator::set_attribute(std::string name, std::string value)
	{
		current_node->attributes[name] = value;
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": ";
		if (current_node->attributes.empty()) {
			std::cout << "empty";
		} else {
			std::cout << name << '=' << current_node->attributes[name] << std::endl;
		}
#endif
	}

	void Node_Creator::new_child()
	{
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << std::endl;
#endif
		throw_if_current_is_null();
		current_node->children.push_back(
		  excel_xml_parser::Node::SP{ new excel_xml_parser::Node });
		current_node_path.push_back(current_node);
		current_node = current_node->children.back();
	}

	void Node_Creator::new_element(std::vector<char> x)
	{
		new_child();
		current_node->name.assign(x.begin(), x.end());
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->name << std::endl;
#endif
		if (current_node->name == "Row") {
			m_col_idx = 0;
			++m_row_idx;
		} else if (current_node->name == "Cell") {
			++m_col_idx;
		} else if (current_node->name == "Worksheet") {
			m_row_idx = 0;
			m_col_idx = 0;
			++m_wkt_idx;
		}
		current_node->row_idx = m_row_idx;
		current_node->col_idx = m_col_idx;
		current_node->wkt_idx = m_wkt_idx;
	}

	bool Node_Creator::close_singleton()
	{
		throw_if_current_is_null();
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->name << std::endl;
#endif
		return resume_parent();
	}

	bool Node_Creator::verify_closing_tag(std::vector<char> x)
	{
		throw_if_current_is_null();
		bool verify = std::equal(
		  x.begin(), x.end(), current_node->name.begin(), current_node->name.end());
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << std::string{ x.begin(), x.end() }
				  << " --- " << (verify ? "correct" : "WRONG") << std::endl;
#endif
		resume_parent();
		return verify;
	}
} // namespace excel_xml_parser
