// Simple-XML/src/Element-Creator.cpp
// Started 15 Aug 2018
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element-Creator.hpp"
#define TRACE_CREATOR
#undef TRACE_CREATOR

#ifdef NDEBUG
#undef TRACE_CREATOR
#endif

namespace simple_xml {

	Element_Creator::Element_Creator(std::vector<Element>& elements)
	  : Node_Visitor{ elements }
	{
		// root->name = "root";
	}

	void Element_Creator::set_text(std::vector<char> y)
	{
		current_node->text.assign(y.begin(), y.end());
		boost::algorithm::trim(current_node->text);
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->text
				  << " --- length = " << current_node->text.size() << std::endl;
#endif
	}

	void Element_Creator::set_attribute(std::string name, std::string value)
	{
		current_node->attributes[name] = value;
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": ";
		if (current_node->attributes.empty()) {
			std::cout << "empty";
		}
		else {
			std::cout << name << '=' << current_node->attributes[name] << std::endl;
		}
#endif
	}

	void Element_Creator::new_child()
	{
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << std::endl;
#endif
		throw_if_current_is_null();
		m_elements.push_back(Element{});
		Element::Index new_element_index = m_elements.size() - 1;
		m_elements[current_node].children.push_back(new_element_index);
		current_node_path.push_back(current_node);
		current_node = new_element_index;
	}

	void Element_Creator::new_element(std::vector<char> x)
	{
		new_child();
		current_node->name.assign(x.begin(), x.end());
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->name << std::endl;
#endif
		if (current_node->name == "Row") {
			m_col_idx = 0;
			++m_row_idx;
		}
		else if (current_node->name == "Cell") {
			++m_col_idx;
		}
		else if (current_node->name == "Worksheet") {
			m_row_idx = 0;
			m_col_idx = 0;
			++m_wkt_idx;
		}
		current_node->row_idx = m_row_idx;
		current_node->col_idx = m_col_idx;
		current_node->wkt_idx = m_wkt_idx;
	}

	bool Element_Creator::close_singleton()
	{
		throw_if_current_is_null();
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_node->name << std::endl;
#endif
		return resume_parent();
	}

	bool Element_Creator::verify_closing_tag(std::vector<char> x)
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
} // namespace simple_xml
