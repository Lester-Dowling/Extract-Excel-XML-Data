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
#ifdef TRACE_VISITOR
	using std::cout;
	using std::endl;
#endif
	using std::string;
	using std::vector;
	using std::map;
	using std::optional;
	using std::runtime_error;
	using std::function;

	bool Element_Visitor::visit_first_child()
	{
#ifdef TRACE_VISITOR
		cout << ">>> " << __FUNCTION__ << ": " << '(' << name() << ' ' << children().size()
			 << " children" << ')' << path_to_string() << endl;
#endif
		if (current().children.empty())
			return false;
		current_index_path.push_back(current_index);
		current_index = current().children.front();
		return true;
	}


	bool Element_Visitor::resume_parent()
	{
#ifdef TRACE_VISITOR
		cout << ">>> " << __FUNCTION__ << ": " << '(' << current_index->name << ')'
			 << path() << endl;
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
#ifdef TRACE_VISITOR
		cout << ">>> " << __FUNCTION__ << ": " << '(' << parent().name() << ' '
			 << parent().children.size() << " children" << ')' << path_to_string() << endl;
#endif
		bool take_next = false;
		for (auto sibling_index : parent().children) {
			if (take_next) {
				current_index = sibling_index;
				return true;
			}
			if (current_index == sibling_index) {
				take_next = true;
			}
		}
		return false;
	}


	size_t Element_Visitor::depth() const
	{
		return m_elements.empty() ? 0 : current_index_path.size() + 1;
	}


	string Element_Visitor::path_to_string() const
	{
		if (m_elements.empty())
			return {};
		string result;
		Element_Path_Iterator nitr = current_index_path.begin();
		Element_Path_Iterator const nend = current_index_path.end();
		while (nitr != nend) {
			const Element::Index idx = *nitr++;
			result += m_elements[idx].name();
			result += " --> ";
		}
		result += current().name();
		return result;
	}


	void Element_Visitor::visit_all_depth_first(function<bool(Element_Visitor&)> callback)
	{
		if (m_elements.empty())
			return;
		current_index = 0;
		if (!callback(*this))
			return;
		while (true) {
			while (visit_first_child()) {
				if (visit_all_predicate()) {
					if (!callback(*this))
						return;
				}
			}
			while (true) {
				if (visit_next_sibling()) {
					if (visit_all_predicate()) {
						if (!callback(*this))
							return;
					}
					break;
				}
				else { // No more siblings => retreat to parent:
					if (!resume_parent())
						return;
				}
			}
		}
	}
} // namespace simple_xml
