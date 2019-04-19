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
	using std::function;

	class Element_Visitor {
	protected:
		vector<Element>& m_elements; // Ref to the collection of Elements.
		Element::Index current_index;

		using Element_Path_Type = vector<Element::Index>;
		using Element_Path_Iterator = Element_Path_Type::const_iterator;
		Element_Path_Type current_index_path;
		bool is_current_index_root() const { return current_index_path.empty(); }
		Element& parent() const noexcept { return m_elements[current_index_path.back()]; }

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

		size_t depth() const;

		string path_to_string() const;

	public: //~ Visit -------------------------------------------------
		void visit_all_depth_first(function<bool(Element_Visitor&)> callback);

		bool visit_first_child();

		bool visit_next_sibling();

		bool resume_parent();

	protected: //~ Customize Visit ---------------------------------------
		virtual bool visit_all_predicate() { return true; }
	};
} // namespace simple_xml
