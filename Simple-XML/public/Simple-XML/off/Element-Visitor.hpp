/**
 * @file   Simple-XML/Element-Visitor.hpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#pragma once
#include <map>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include "Simple-XML/Element.hpp"

namespace simple_xml {
	using std::map;
	using std::optional;
	using std::string;
	using std::vector;

	class Element_Visitor {
	public:
		Element_Visitor(vector<Element>& elements);
		virtual ~Element_Visitor() = default;

		string name() const;

		string text() const;

		int col() const { return current_index->col_idx; }

		int row() const { return current_index->row_idx; }

		int wkt() const { return current_index->wkt_idx; }

		map<string, string> const& attributes() const;

		optional<string> attribute(string attribute_name) const;

		size_t depth() const { return current_index_path.size(); }

		string path_to_string() const;

		static void all_depth_first(
		  Element::Index root,
		  std::function<bool(Node_Visitor&)> callback);

	protected:
		vector<Element>& m_elements; // Ref to the collection of Elements.
		Element::Index current_index;

		typedef vector<Element::Index> Node_Path_Type;
		typedef Node_Path_Type::const_iterator Node_Path_Iterator;
		Node_Path_Type current_index_path;

		bool is_current_index_root() const { return current_index_path.empty(); }

		virtual bool resume_parent();

		virtual bool visit_first_child();

		virtual bool visit_next_sibling();

		void throw_if_current_is_null() const;
	};
} // namespace simple_xml
