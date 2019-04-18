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

	class Element_Creator : public Element_Visitor {
		int m_row_idx = 0; // Current row number.
		int m_col_idx = 0; // Current column number.
		int m_wkt_idx = 0; // Current worksheet number.
		void new_child();

	public:
		Element_Creator(std::vector<Element>& elements);

		void new_element(std::vector<char> x);

		void set_attribute(std::string name, std::string value);

		bool close_singleton();

		bool verify_closing_tag(std::vector<char> x);

		void set_text(std::vector<char> y);
	};
} // namespace simple_xml
