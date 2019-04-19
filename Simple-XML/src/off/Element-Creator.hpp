/**
 * @file   Simple-XML/src/Element-Creator.hpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <vector>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Visitor.hpp"

namespace simple_xml {
	using std::string;
	using std::vector;

	class Element_Creator : public Element_Visitor {
		int m_row_idx = 0; // Current row number.
		int m_col_idx = 0; // Current column number.
		int m_wkt_idx = 0; // Current worksheet number.
		void new_child();
		Element& current() noexcept { return m_elements[current_index]; }
		Element const& current() const noexcept { return m_elements[current_index]; }

	public:
		Element_Creator(vector<Element>& elements);

		void new_element(vector<char> const& x);

		void set_attribute(string name, string value);

		bool close_singleton();

		bool verify_closing_tag(vector<char> x);

		void set_text(vector<char> const& y);
	};
} // namespace simple_xml
