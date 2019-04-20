/**
 * @file   Simple-XML/src/Element-Filter.cpp
 * @date   Started 2018-08-14
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element-Filter.hpp"
#define TRACE_FILTER
//#undef TRACE_FILTER

#ifdef NDEBUG
#undef TRACE_FILTER
#endif

namespace simple_xml {
	using std::cout;
	using std::endl;

	bool Element_Filter::xml_element_matches_filter_node(
	  Element const& ele,
	  const Grade::SP fn) const
	{
		using Attribute_Filter = pseudo_xpath::Attribute_Filter;
		// cout << "e.name == " << visited_element.name() << endl;
		// cout << "fn->name   == " << fn->name() << endl;
		if (ele.name() != fn->name())
			return false;
		// cout << "XML node == " << visited_element.name() << endl;
		for (Attribute_Filter const& filter_attribute : fn->filters()) {
			auto const& filter_name = filter_attribute.attribute_name;
			auto const& filter_operator = filter_attribute.filter_operator;
			auto const& filter_value = filter_attribute.filter_value;
			auto const& filter_number = filter_attribute.filter_number;
			auto const& good_filter_number = filter_attribute.good_filter_number;

			if (filter_name == "Column" || filter_name == "Cell") {
				if (good_filter_number)
					switch (filter_operator) {
						case '=':
							if (!(ele.col_idx == filter_number))
								return false;
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
					switch (filter_operator) {
						case '=':
							if (const auto col_title =
								  m_titles.col_title(ele.wkt_idx, ele.col_idx);
								col_title.has_value() && *col_title != filter_value) //
							{
								return false;
							}
							break;
						default:
							throw std::runtime_error{ "Bad filter operator" };
					}
			} else if (filter_name == "Row") {
				if (good_filter_number)
					switch (filter_operator) {
						case '=':
							if (!(ele.row_idx == filter_number))
								return false;
							break;
						case '<':
							if (!(ele.row_idx < filter_number))
								return false;
							break;
						case '>':
							if (!(ele.row_idx > filter_number))
								return false;
							break;
						default:
							throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
					switch (filter_operator) {
						case '=':
							if (const auto row_title =
								  m_titles.row_title(ele.wkt_idx, ele.row_idx);
								row_title.has_value() && *row_title != filter_value) //
							{
								return false;
							}
							break;
						default:
							throw std::runtime_error{ "Bad filter operator" };
					}
			} else if (filter_name == "Worksheet") {
				if (good_filter_number)
					switch (filter_operator) {
						case '=':
							if (!(ele.wkt_idx == filter_number))
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
					switch (filter_operator) {
						case '=':
							if (const auto wkt_title = m_titles.wkt_title(ele.wkt_idx);
								wkt_title.has_value() && *wkt_title != filter_value) //
							{
								return false;
							}
							break;
						default:
							throw std::runtime_error{ "Bad filter operator" };
					}
			} else if (const auto attribute_value = ele.attribute(filter_name);
					   attribute_value.has_value() && *attribute_value != filter_value) //
			{
				return false;
			}
		}
		return true;
	}

#undef TRACE_CURRENT_PATH_MATCHES_FILTER_PATH
// define TRACE_CURRENT_PATH_MATCHES_FILTER_PATH
#ifdef TRACE_CURRENT_PATH_MATCHES_FILTER_PATH
#define __TRACER(args) std::cout << args << std::endl;
#else // not defined
#define __TRACER(args)
#endif

	bool Element_Filter::visit_all_predicate()
	{
		using std::cout;
		using std::endl;
		__TRACER("XML path == " << path_to_string());
		__TRACER("XPath    == " << Grade::path_to_string(m_filter_path));
		Grade::SP filter_node = m_filter_path;
		if (!filter_node)
			throw std::runtime_error{ "No filter path set for Element Filter." };
		for (const Element::Index visited_index : current_index_path) {
			Element const& visited_element = m_elements[visited_index];
			if (!(filter_node = filter_node->next())) {
				__TRACER("<<< No more filters => accept all nodes below.");
				return true;
			}
			__TRACER("filter_node.name == " << filter_node->name());
			if (!xml_element_matches_filter_node(visited_element, filter_node)) {
				__TRACER(
				  "<<< Node " << visited_element.name() << " failed to match filter "
							  << filter_node->name());
				return false;
			}
		}
		// Finally, try the current node:
		if (!(filter_node = filter_node->next())) {
			__TRACER("<<< No more filters => accept current node.");
			return true;
		}
		__TRACER("filter_node.name == " << filter_node->name());
		if (!xml_element_matches_filter_node(current(), filter_node)) {
			__TRACER(
			  "<<< Current Node " << current().name() << " failed to match filter "
								  << filter_node->name());
			return false;
		}
		if (filter_node = filter_node->next()) {
			__TRACER("<<< Some filters remain untested.");
			return false;
		}
		__TRACER("<<< All filters passed.");
		return true;
	}

} // namespace simple_xml
