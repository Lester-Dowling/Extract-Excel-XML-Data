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
	using std::map;
	using std::string;
	using std::vector;
	using std::function;
	using pseudo_xpath_parser::Grade;

	class Element_Filter : public Element_Visitor {
		Worksheet_Row_Column_Titles& m_titles;
		Grade::SP m_filter_path;

		bool xml_element_matches_filter_node(Element const& e, const Grade::SP fn) const;

	public:
		Element_Filter(vector<Element>& elements, Worksheet_Row_Column_Titles& titles)
		  : Element_Visitor{ elements }
		  , m_titles{ titles }
		{
		}

	public: //~ Filter ------------------------------------------------
		void set_filter_path(Grade::SP filter_path) { m_filter_path = filter_path; }

		/**
		* Current path matches filter path.
		*/
		bool visit_all_predicate() override;
	};
} // namespace simple_xml
