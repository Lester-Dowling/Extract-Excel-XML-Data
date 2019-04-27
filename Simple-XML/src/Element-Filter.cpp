/**
 * @file   Simple-XML/src/Element-Filter.cpp
 * @date   Started 2018-08-14
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element-Filter.hpp"

#define TRACE_ELEMENT_FILTER
#undef TRACE_ELEMENT_FILTER
#ifdef TRACE_ELEMENT_FILTER
#define __TRACER(args) std::cout << args << std::endl;
#else // not defined
#define __TRACER(args)
#endif

namespace simple_xml {
	using std::cout;
	using std::endl;
	using std::runtime_error;

	Element_Filter& Element_Filter::set_filter_path(Grade::SP filter_path)
	{
		m_filter_path = filter_path;
		return *this;
	}


	bool Element_Filter::attribute_match(
	  const int idx, // ele.idx
	  std::optional<std::string> const&
		title, // m_titles.col_title(ele.wkt_idx, ele.col_idx);
	  pseudo_xpath::Attribute_Filter const& filter_attribute) const
	{
		if (filter_attribute.good_filter_number)
			switch (filter_attribute.filter_operator) {
			case '=':
				return idx == filter_attribute.filter_number;
			case '<':
				return idx < filter_attribute.filter_number;
			case '>':
				return idx > filter_attribute.filter_number;
			default:
				throw runtime_error{ "Impossible filter operator" };
			}
		else // no good_filter_number => try filter_value
		  if (filter_attribute.filter_operator == '=') {
			if (!title.has_value())
				return false;
			return *title == filter_attribute.filter_value;
		}
		else
			throw runtime_error{ "Bad operator for " + filter_attribute.attribute_name };
	}


	bool Element_Filter::filters_all_true(Element const& ele, const Grade::SP fn) const
	{
#ifdef TRACE_ELEMENT_FILTER
		const char* const FN = "[filters_all_true] ";
#endif
		using Attribute_Filter = pseudo_xpath::Attribute_Filter;
		if (ele.name() != fn->name())
			return false;
		for (Attribute_Filter const& filter_attribute : fn->filters()) {
			auto const& filter_name = filter_attribute.attribute_name;
			auto const& filter_value = filter_attribute.filter_value;
			__TRACER(FN << "filter_name        = " << filter_attribute.attribute_name);
			__TRACER(FN << "filter_value       = " << filter_attribute.filter_value);

			if (filter_name == "Cell" || filter_name == "Column") {
				if (attribute_match(
					  ele.col_idx,
					  m_titles(ele.wkt_idx).col_title(ele.col_idx),
					  filter_attribute))
					continue;
				else
					return false;
			}
			else if (filter_name == "Row") {
				if (attribute_match(
					  ele.row_idx,
					  m_titles(ele.wkt_idx).row_title(ele.row_idx),
					  filter_attribute))
					continue;
				else
					return false;
			}
			else if (filter_name == "Worksheet") {
				if (attribute_match(
					  ele.wkt_idx, m_titles.wkt_title(ele.wkt_idx), filter_attribute))
					continue;
				else
					return false;
			}
			else {
				const auto attribute_value = ele.attribute(filter_name);
				if (!attribute_value.has_value())
					return false;
				if (*attribute_value == filter_value)
					continue;
				else
					return false;
			}
		}
		return true;
	}


	bool Element_Filter::filters_one_true(Element const& ele, const Grade::SP fn) const
	{
#ifdef TRACE_ELEMENT_FILTER
		const char* const FN = "[filters_one_true] ";
#endif
		using Attribute_Filter = pseudo_xpath::Attribute_Filter;
		if (ele.name() != fn->name())
			return false;
		for (Attribute_Filter const& filter_attribute : fn->filters()) {
			auto const& filter_name = filter_attribute.attribute_name;
			auto const& filter_value = filter_attribute.filter_value;
			__TRACER(FN << "filter_name        = " << filter_attribute.attribute_name);
			__TRACER(FN << "filter_value       = " << filter_attribute.filter_value);

			if (filter_name == "Cell" || filter_name == "Column") {
				if (attribute_match(
					  ele.col_idx,
					  m_titles(ele.wkt_idx).col_title(ele.col_idx),
					  filter_attribute))
					return true;
				else
					continue;
			}
			else if (filter_name == "Row") {
				if (attribute_match(
					  ele.row_idx,
					  m_titles(ele.wkt_idx).row_title(ele.row_idx),
					  filter_attribute))
					return true;
				else
					continue;
			}
			else if (filter_name == "Worksheet") {
				if (attribute_match(
					  ele.wkt_idx, m_titles.wkt_title(ele.wkt_idx), filter_attribute))
					return true;
				else
					continue;
			}
			else {
				const auto attribute_value = ele.attribute(filter_name);
				if (!attribute_value.has_value())
					continue;
				if (*attribute_value == filter_value)
					return true;
				else
					continue;
			}
		}
		return false;
	}


	bool Element_Filter::filters_true(Element const& ele, const Grade::SP fn) const
	{
		if (fn->is_alternation)
			return filters_one_true(ele, fn);
		else
			return filters_all_true(ele, fn);
	}


	bool Element_Filter::visit_all_predicate()
	{
		__TRACER(">>> XPath    == " << Grade::path_to_string(m_filter_path));
		__TRACER(">>> XML path == " << path_to_string());
		Grade::SP filter_node = m_filter_path;
		if (!filter_node)
			throw std::runtime_error{ "No filter path set for Element Filter." };
		for (const Element::Index visited_index : m_current_index_path) {
			__TRACER("-------------------");
			Element const& visited_element = m_elements[visited_index];
			__TRACER(visited_element.all_fields_to_string());
			if (!(filter_node = filter_node->next())) {
				__TRACER("<<< No more filters => accept all nodes below.  Return true.");
				return true;
			}
			__TRACER(
			  "filter_node.name == " << filter_node->name()
									 << filter_node->attribute_filters_to_string());
			if (!filters_true(visited_element, filter_node)) {
				__TRACER(
				  "<<< Node " << visited_element.name() << " failed to match filter "
							  << filter_node->name()
							  << filter_node->attribute_filters_to_string()
							  << ".  Return false.");
				return false;
			}
		}
		__TRACER("-------------------");
		// Finally, try the current Element:
		__TRACER("Current " << current().all_fields_to_string());
		if (!(filter_node = filter_node->next())) {
			__TRACER("<<< No more filters => accept current node.  Return true.");
			return true;
		}
		__TRACER(
		  "filter_node.name == " << filter_node->name()
								 << filter_node->attribute_filters_to_string());
		if (!filters_true(current(), filter_node)) {
			__TRACER(
			  "<<< Current Node "
			  << current().name() << " failed to match filter " << filter_node->name()
			  << filter_node->attribute_filters_to_string() << ".  Return false.");
			return false;
		}
		if (filter_node = filter_node->next()) {
			__TRACER("<<< Some filters remain untested.  Return false.");
			return false;
		}
		__TRACER("<<< All filters passed.  Return true.");
		return true;
	}

} // namespace simple_xml
