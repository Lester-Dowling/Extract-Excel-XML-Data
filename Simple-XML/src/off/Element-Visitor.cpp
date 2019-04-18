/**
 * @file   Simple-XML/src/Element-Visitor.cpp
 * @date   Started 2018-08-14
 * @author Lester J. Dowling
 */
#include "pch-simple-xml.hpp"
#include "Simple-XML/Element-Visitor.hpp"
#define TRACE_VISITOR
#undef TRACE_VISITOR

#ifdef NDEBUG
#undef TRACE_VISITOR
#endif

namespace simple_xml {
	using std::string;
	using std::vector;
	using std::map;
	using std::optional;
	using std::runtime_error;

	Element_Visitor::Element_Visitor(vector<Element>& elements)
	  : m_elements{ elements }
	  , current_index{}
	{
	}

	void Element_Visitor::throw_if_current_is_null() const
	{
		if (!current_index)
			throw runtime_error{ "Current XML element is null." };
	}

	string Element_Visitor::name() const
	{
		throw_if_current_is_null();
		return current_index->name;
	}

	string Element_Visitor::text() const
	{
		throw_if_current_is_null();
		return current_index->text;
	}

	map<string, string> const& Element_Visitor::attributes() const
	{
		return current_index->attributes;
	}

	optional<string> Element_Visitor::attribute(string attribute_name) const
	{
		const auto fitr = current_index->attributes.find(attribute_name);
		if (fitr != current_index->attributes.end())
			return fitr->second;
		else
			return {};
	}


	bool Element_Visitor::visit_first_child()
	{
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << current_index->name << ' '
				  << current_index->children.size() << " children" << ')' << path()
				  << std::endl;
#endif
		if (current_index->children.empty())
			return false;
		current_index_path.push_back(current_index);
		current_index = current_index->children.front();
		return true;
	}

	bool Element_Visitor::resume_parent()
	{
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << current_index->name << ')'
				  << path() << std::endl;
#endif
		if (this->is_current_index_root())
			return false;
		current_index = current_index_path.back();
		current_index_path.pop_back();
		return true;
	}

	bool Element_Visitor::visit_next_sibling()
	{
		if (this->is_current_index_root())
			return false;
		excel_xml_parser::Element::SP parent = current_index_path.back();
#ifdef TRACE_VISITOR
		std::cout << ">>> " << __FUNCTION__ << ": " << '(' << parent->name << ' '
				  << parent->children.size() << " children" << ')' << path() << std::endl;
#endif
		bool take_next = false;
		for (auto sibling_node : parent->children) {
			if (take_next) {
				current_index = sibling_node;
				return true;
			}
			if (current_index == sibling_node) {
				take_next = true;
			}
		}
		return false;
	}

	string Element_Visitor::path_to_string() const
	{
		string result;
		Element_Path_Iterator nitr = current_index_path.begin();
		Element_Path_Iterator const nend = current_index_path.end();
		while (++nitr != nend) { // Elide root with "++nitr".
			excel_xml_parser::Element::SP node = *nitr;
			result += node->name;
			result += " --> ";
		}
		result += current_index->name;
		return result;
	}

	void Element_Visitor::all_depth_first(
	  excel_xml_parser::Element::SP root,
	  std::function<bool(Element_Visitor&)> callback)
	{
		Element_Visitor visitor{ root };
		while (true) {
			while (visitor.visit_first_child()) {
				if (!callback(visitor))
					return;
			}
			while (true) {
				if (visitor.visit_next_sibling()) {
					if (!callback(visitor))
						return;
					break;
				}
				else { // No more siblings => retreat to parent:
					if (!visitor.resume_parent())
						return;
				}
			}
		}
	}
} // namespace simple_xml
