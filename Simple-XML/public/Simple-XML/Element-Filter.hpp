/**
 * @file   Simple-XML/Element-Filter.hpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <functional>
#include <optional>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Visitor.hpp"
#include "Simple-XML/Worksheet-Row-Column-Titles.hpp"
#include "Pseudo-XPath/Grade.hpp"

namespace simple_xml {
	using pseudo_xpath::Grade;
	using std::function;
	using std::map;
	using std::string;
	using std::vector;

	class Element_Filter : public Element_Visitor {
		Worksheet_Row_Column_Titles& m_titles;
		Grade::SP m_filter_path;
		using This = Element_Filter&; // Indicate member function returns *this.

		/**
		 * Match the given XML element against the @c filter_node and return true if all
		 * the attribute filters in the node are true.
		 */
		bool filters_all_true(Element const& ele, const Grade::SP filter_node) const;

		/**
		 * Match the given XML element against the @c filter_node and return true if at
		 * least one of the attribute filters in the node is true.
		 */
		bool filters_one_true(Element const& ele, const Grade::SP filter_node) const;

		/**
		 * Match the given XML element against the @c filter_node: either with @c
		 * filters_all_true or @c filters_one_true.
		 */
		bool filters_true(Element const& ele, const Grade::SP filter_node) const;

		/**
		 * Return true if the attribute matches.
		 */
		bool attribute_match(
		  const int idx,
		  std::optional<std::string> const& title,
		  pseudo_xpath::Attribute_Filter const& filter_attribute) const;

	public:
		Element_Filter(vector<Element>& elements, Worksheet_Row_Column_Titles& titles)
		  : Element_Visitor{ elements }
		  , m_titles{ titles }
		{
		}

	public: //~ Filter ------------------------------------------------
		This set_filter_path(Grade::SP filter_path);

		Grade::SP filter_path() const { return m_filter_path; }

	protected:
		/**
		 * Current path matches filter path.
		 */
		bool visit_all_predicate() override;
	};
} // namespace simple_xml
