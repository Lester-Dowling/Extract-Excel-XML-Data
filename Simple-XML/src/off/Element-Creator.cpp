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
	namespace a = boost::algorithm;

	Element_Creator::Element_Creator(vector<Element>& elements)
	  : Element_Visitor{ elements }
	{
	}

	void Element_Creator::set_text(vector<char> y)
	{
		m_elements[current_index].text.assign(y.begin(), y.end());
		a::trim(m_elements[current_index].text);
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << m_elements[current_index].text
			 << " --- length = " << m_elements[current_index].text.size() << endl;
#endif
	}

	void Element_Creator::set_attribute(string name, string value)
	{
		m_elements[current_index].attributes[name] = value;
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": ";
		if (m_elements[current_index].attributes.empty()) {
			cout << "empty";
		}
		else {
			cout << name << '=' << m_elements[current_index].attributes[name] << endl;
		}
#endif
	}

	void Element_Creator::new_child()
	{
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << endl;
#endif
		const bool new_child_is_root = m_elements.empty();
		m_elements.push_back(Element{});
		Element::Index new_element_index = m_elements.size() - 1;
		if (!new_child_is_root) {
			m_elements[current_index].children.push_back(new_element_index);
			current_index_path.push_back(current_index);
		}
		current_index = new_element_index;
	}

	void Element_Creator::new_element(vector<char> x)
	{
		new_child();
		m_elements[current_index].name.assign(x.begin(), x.end());
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << m_elements[current_index].name << endl;
#endif
		if (m_elements[current_index].name == "Row") {
			m_col_idx = 0;
			++m_row_idx;
		}
		else if (m_elements[current_index].name == "Cell") {
			++m_col_idx;
		}
		else if (m_elements[current_index].name == "Worksheet") {
			m_row_idx = 0;
			m_col_idx = 0;
			++m_wkt_idx;
		}
		m_elements[current_index].row_idx = m_row_idx;
		m_elements[current_index].col_idx = m_col_idx;
		m_elements[current_index].wkt_idx = m_wkt_idx;
	}

	bool Element_Creator::close_singleton()
	{
		throw_if_current_is_null();
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << m_elements[current_index].name << endl;
#endif
		return resume_parent();
	}

	bool Element_Creator::verify_closing_tag(vector<char> x)
	{
		throw_if_current_is_null();
		bool verify = std::equal(
		  x.begin(),
		  x.end(),
		  m_elements[current_index].name.begin(),
		  m_elements[current_index].name.end());
#ifdef TRACE_CREATOR
		cout << ">>> " << __FUNCTION__ << ": " << string{ x.begin(), x.end() } << " --- "
			 << (verify ? "correct" : "WRONG") << endl;
#endif
		resume_parent();
		return verify;
	}
} // namespace simple_xml
