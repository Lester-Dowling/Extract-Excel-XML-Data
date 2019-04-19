/**
 * @file   Simple-XML/src/Element-Creator.cpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element-Creator.hpp"
#define TRACE_CREATOR
#undef TRACE_CREATOR

#ifdef NDEBUG
#undef TRACE_CREATOR
#endif

namespace simple_xml {
#ifdef TRACE_CREATOR
	using std::cout;
	using std::endl;
#endif
	using std::string;
	using std::vector;

	Element_Creator::Element_Creator(vector<Element>& elements)
	  : Element_Visitor{ elements }
	{
		assert(m_elements.empty());
	}

	void Element_Creator::set_text(vector<char> const& y)
	{
		current().set_text(y);
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << current().text
			 << " --- length = " << current().text.size() << endl;
#endif
	}

	void Element_Creator::set_attribute(string n, string v)
	{
		current().attributes[n] = v;
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": ";
		if (m_elements[current_index].attributes.empty()) {
			cout << "empty";
		}
		else {
			cout << n << '=' << current().attributes[n] << endl;
		}
#endif
	}

	Element::Index Element_Creator::new_element(vector<char> const& new_name)
	{
		// New child:
		const bool new_child_is_root = m_elements.empty();
		m_elements.emplace_back(new_name);
		const Element::Index new_element_index = m_elements.size() - 1;
		if (!new_child_is_root) {
			current().children.push_back(new_element_index);
			current_index_path.push_back(current_index);
		}
		current_index = new_element_index;
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << current().name() << endl;
#endif
		if (current().name() == "Row") {
			m_col_idx = 0;
			++m_row_idx;
		}
		else if (current().name() == "Cell") {
			++m_col_idx;
		}
		else if (current().name() == "Worksheet") {
			m_row_idx = 0;
			m_col_idx = 0;
			++m_wkt_idx;
		}
		current().row_idx = m_row_idx;
		current().col_idx = m_col_idx;
		current().wkt_idx = m_wkt_idx;
		return new_element_index;
	}

	bool Element_Creator::close_singleton()
	{
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << current().name() << endl;
#endif
		if (!current().text().empty())
			throw std::runtime_error{ "Unexpected text in XML singleton" };
		return resume_parent();
	}

	bool Element_Creator::verify_closing_tag(vector<char> const& x)
	{
		const bool verify =
		  std::equal(x.begin(), x.end(), current().name().begin(), current().name().end());
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << string{ x.begin(), x.end() } << " --- "
			 << (verify ? "correct" : "WRONG") << endl;
#endif
		if (!verify)
			throw std::runtime_error{ "Mismatched closing tag: " + current().name() };
		resume_parent();
		return verify;
	}
} // namespace simple_xml
