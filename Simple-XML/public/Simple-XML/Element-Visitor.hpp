/**
 * @file   Simple-XML/Element-Visitor.hpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#pragma once
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include "Simple-XML/Element.hpp"

namespace simple_xml {
	using std::map;
	using std::string;
	using std::vector;

	class Element_Visitor {
	protected:
		vector<Element>& m_elements; // Ref to the collection of Elements.
		Element::Index current_index;

		Element& parent() const noexcept { return m_elements[current_index_path.back()]; }

		typedef vector<Element::Index> Element_Path_Type;
		typedef Element_Path_Type::const_iterator Element_Path_Iterator;
		Element_Path_Type current_index_path;
		bool is_current_index_root() const { return current_index_path.empty(); }

	public: //~ Ctors et al -------------------------------------------
		Element_Visitor(vector<Element>& elements)
		  : m_elements{ elements }
		  , current_index{}
		{
		}

		virtual ~Element_Visitor() = default;

	public: //~ Accessors ---------------------------------------------
		Element& current() noexcept { return m_elements.at(current_index); }

		Element const& current() const noexcept { return m_elements.at(current_index); }

		// string const& name() const { return current().name(); }

		// string const& text() const { return current().text(); }

		// map<string, string> const& attributes() const { return current().attributes; };

		// vector<Element::Index> const& children() const { return current().children; }

		// int col_idx() const { return current().col_idx; }

		// int row_idx() const { return current().row_idx; }

		// int wkt_idx() const { return current().wkt_idx; }

		// optional<string> attribute(string attribute_name) const
		//{
		//	return current().attribute(attribute_name);
		//}

		virtual bool resume_parent();

		virtual bool visit_first_child();

		virtual bool visit_next_sibling();

		size_t depth() const
		{
			return m_elements.empty() ? 0 : current_index_path.size() + 1;
		}

		string path_to_string() const;

		static void depth_first(
		  vector<Element>& elements,
		  std::function<bool(Element_Visitor&)> callback);
	};
} // namespace simple_xml
