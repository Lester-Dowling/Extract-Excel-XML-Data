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

	Element_Filter& Element_Filter::set_filter_path(Grade::SP filter_path)
	{
		m_filter_path = filter_path;
		return *this;
	}

	bool Element_Filter::xml_element_matches_filter_node(
	  Element const& ele,
	  const Grade::SP fn) const
	{
#ifdef TRACE_ELEMENT_FILTER
		const char* const FN = "[xml_element_matches_filter_node] ";
#endif
		using Attribute_Filter = pseudo_xpath::Attribute_Filter;
		if (ele.name() != fn->name())
			return false;
		for (Attribute_Filter const& filter_attribute : fn->filters()) {
			auto const& filter_name = filter_attribute.attribute_name;
			auto const& filter_operator = filter_attribute.filter_operator;
			auto const& filter_value = filter_attribute.filter_value;
			auto const& filter_number = filter_attribute.filter_number;
			auto const& good_filter_number = filter_attribute.good_filter_number;

			__TRACER(FN << "filter_name        = " << filter_attribute.attribute_name);
			__TRACER(FN << "filter_operator    = " << filter_attribute.filter_operator);
			__TRACER(FN << "filter_value       = " << filter_attribute.filter_value);
			__TRACER(FN << "filter_number      = " << filter_attribute.filter_number);
			__TRACER(FN << "good_filter_number = " << filter_attribute.good_filter_number);

			if (filter_name == "Cell" || filter_name == "Column") {
				__TRACER(FN << "filter_name == Cell");
				if (good_filter_number)
					switch (filter_operator) {
					case '=':
						if (ele.col_idx != filter_number)
							return false;
						else {
							__TRACER(FN << "ele.col_idx == filter_number");
							__TRACER(FN << ele.col_idx << " == " << filter_number);
						}
						break;
					case '<':
						if (!(ele.col_idx < filter_number))
							return false;
						break;
					case '>':
						if (!(ele.col_idx > filter_number))
							return false;
						break;
					default:
						throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
				  if (filter_operator == '=') {
					const auto col_title = m_titles.col_title(ele.wkt_idx, ele.col_idx);
					if (!col_title.has_value())
						return false;
					if (*col_title != filter_value) //
					{
						__TRACER(FN << "*col_title != filter_value");
						__TRACER(
						  FN << "'" << *col_title << " ' != ' " << filter_value << "'");
						return false;
					}
					else {
						__TRACER(FN << "*col_title == filter_value");
						__TRACER(
						  FN << "'" << *col_title << "' == '" << filter_value << "'");
					}
				}
				else
					throw std::runtime_error{ "Bad filter operator for Cell" };
				__TRACER("Cell matched successfully!");
			}
			else if (filter_name == "Row") {
				__TRACER(FN << "filter_name == Row");
				if (good_filter_number)
					switch (filter_operator) {
					case '=':
						if (ele.row_idx != filter_number)
							return false;
						else {
							__TRACER(FN << "ele.row_idx == filter_number");
							__TRACER(FN << ele.row_idx << " == " << filter_number);
						}
						break;
					case '<':
						if (!(ele.row_idx < filter_number))
							return false;
						else {
							__TRACER(FN << "ele.row_idx < filter_number");
							__TRACER(FN << ele.row_idx << " < " << filter_number);
						}
						break;
					case '>':
						if (!(ele.row_idx > filter_number))
							return false;
						else {
							__TRACER(FN << "ele.row_idx > filter_number");
							__TRACER(FN << ele.row_idx << " > " << filter_number);
						}
						break;
					default:
						throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
				  if (filter_operator == '=') {
					const auto row_title = m_titles.row_title(ele.wkt_idx, ele.row_idx);
					if (!row_title.has_value())
						return false;
					if (*row_title != filter_value) //
					{
						__TRACER(FN << "*row_title != filter_value");
						__TRACER(
						  FN << "'" << *row_title << " ' != ' " << filter_value << "'");
						return false;
					}
					else {
						__TRACER(FN << "*row_title == filter_value");
						__TRACER(
						  FN << "'" << *row_title << "' == '" << filter_value << "'");
					}
				}
				else
					throw std::runtime_error{ "Bad filter operator for Row" };
				__TRACER("Row matched successfully!");
			}
			else if (filter_name == "Worksheet") {
				__TRACER(FN << "filter_name == Worksheet");
				if (good_filter_number)
					switch (filter_operator) {
					case '=':
						if (ele.wkt_idx != filter_number)
							return false;
						break;
					case '<':
						if (!(ele.wkt_idx < filter_number))
							return false;
						break;
					case '>':
						if (!(ele.wkt_idx > filter_number))
							return false;
						break;
					default:
						throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
				  if (filter_operator == '=') {
					const auto wkt_title = m_titles.wkt_title(ele.wkt_idx);
					if (!wkt_title.has_value())
						return false;
					if (*wkt_title != filter_value) //
					{
						return false;
					}
					else {
						__TRACER(FN << "*wkt_title == filter_value");
						__TRACER(
						  FN << "'" << *wkt_title << "' == '" << filter_value << "'");
					}
				}
				else
					throw std::runtime_error{ "Bad filter operator for Worksheet" };
				__TRACER("Worksheet matched successfully!");
			}
			else {
				const auto attribute_value = ele.attribute(filter_name);
				if (!attribute_value.has_value())
					return false;
				if (*attribute_value != filter_value)
					return false;
			}
		}
		return true;
	} // namespace simple_xml

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
			if (!xml_element_matches_filter_node(visited_element, filter_node)) {
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
		if (!xml_element_matches_filter_node(current(), filter_node)) {
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
