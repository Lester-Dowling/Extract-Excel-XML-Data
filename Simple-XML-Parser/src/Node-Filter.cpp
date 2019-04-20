// Simple-XML-Parser/src/Node-Filter.cpp
// Started 14 Aug 2018
#include "pch-xml-parser.hpp"
#include "Simple-XML-Parser/Node-Filter.hpp"
#define TRACE_FILTER
//#undef TRACE_FILTER

#ifdef NDEBUG
#undef TRACE_FILTER
#endif

namespace excel_xml_parser {

	Node_Filter::Node_Filter(excel_xml_parser::Node::SP root)
	  : Node_Visitor{ root }
	{}

	void Node_Filter::set_filter_path(pseudo_xpath::Grade::SP filter_path)
	{
		m_filter_path = filter_path;
	}

	bool Node_Filter::xml_node_matches_filter_node(
	  const excel_xml_parser::Node::SP node,
	  const Filter_Node fn) const
	{
		using std::cout;
		using std::endl;
		using Attribute_Filter = pseudo_xpath::Attribute_Filter;
		// cout << "node->name == " << node->name << endl;
		// cout << "fn->name   == " << fn->name() << endl;
		if (node->name != fn->name())
			return false;
		// cout << "XML node == " << node->name << endl;
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
							if (!(node->col_idx == filter_number))
								return false;
							break;
						case '<':
							if (!(node->col_idx < filter_number))
								return false;
							break;
						case '>':
							if (!(node->col_idx > filter_number))
								return false;
							break;
						default:
							throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
					switch (filter_operator) {
						case '=':
							if (const auto col_title =
								  m_titles->col_title(node->wkt_idx, node->col_idx);
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
							if (!(node->row_idx == filter_number))
								return false;
							break;
						case '<':
							if (!(node->row_idx < filter_number))
								return false;
							break;
						case '>':
							if (!(node->row_idx > filter_number))
								return false;
							break;
						default:
							throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
					switch (filter_operator) {
						case '=':
							if (const auto row_title =
								  m_titles->row_title(node->wkt_idx, node->row_idx);
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
							if (!(node->wkt_idx == filter_number))
								return false;
							break;
						case '<':
							if (!(node->wkt_idx < filter_number))
								return false;
							break;
						case '>':
							if (!(node->wkt_idx > filter_number))
								return false;
							break;
						default:
							throw std::runtime_error{ "Impossible filter operator" };
					}
				else // no good_filter_number => try filter_value
					switch (filter_operator) {
						case '=':
							if (const auto wkt_title = m_titles->wkt_title(node->wkt_idx);
								wkt_title.has_value() && *wkt_title != filter_value) //
							{
								return false;
							}
							break;
						default:
							throw std::runtime_error{ "Bad filter operator" };
					}
			} else if (node->attributes.count(filter_name)) {
				if (node->attributes[filter_name] != filter_value) {
					return false;
				}
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

	bool Node_Filter::current_path_matches_filter_path() const
	{
		using std::cout;
		using std::endl;
		__TRACER("XML path == " << path_to_string());
		__TRACER("XPath    == " << pseudo_xpath::Grade::path_to_string(m_filter_path));
		Node_Visitor::Node_Path_Iterator node_itr = current_node_path.begin();
		Node_Visitor::Node_Path_Iterator const node_end = current_node_path.end();
		Filter_Node filter_node = m_filter_path;
		if (!filter_node)
			return true;
		while (++node_itr != node_end) { // Skip root node with ++node_itr.
			if (!(filter_node = filter_node->next())) {
				__TRACER("<<< No more filters => accept all nodes below.");
				return true;
			}
			__TRACER("filter_node.name == " << filter_node->name());
			if (!xml_node_matches_filter_node(*node_itr, filter_node)) {
				__TRACER(
				  "<<< Node " << (*node_itr)->name << " failed to match filter "
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
		if (!xml_node_matches_filter_node(current_node, filter_node)) {
			__TRACER(
			  "<<< Current Node " << current_node->name << " failed to match filter "
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

	void Node_Filter::all_siblings(
	  excel_xml_parser::Node::SP root,
	  pseudo_xpath::Grade::SP filter_path,
	  Worksheet_Row_Column_Titles::SP titles,
	  std::function<bool(Node_Visitor&)> filter_predicate)
	{
		Node_Filter visitor{ root };
		visitor.set_filter_path(filter_path);
		visitor.set_titles(titles);
		while (true) {
			while (visitor.visit_first_child()) {
				if (visitor.current_path_matches_filter_path()) {
					if (!filter_predicate(visitor))
						return;
				}
			}
			while (true) {
				if (visitor.visit_next_sibling()) {
					if (visitor.current_path_matches_filter_path()) {
						if (!filter_predicate(visitor))
							return;
					}
					break;
				} else { // No more siblings => retreat to parent:
					if (!visitor.resume_parent())
						return;
				}
			}
		}
	}

#if 0
			else {
			std::cout << "Filtered out path: ";
			for (auto filtered_out_node : visitor.current_node_path) {
				std::cout << filtered_out_node->name << "   ";
			}
			std::cout << std::endl;
	}
#endif
} // namespace excel_xml_parser
