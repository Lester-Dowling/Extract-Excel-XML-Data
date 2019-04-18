// Simple-XML/Element-Visitor.hpp
// Started 15 Aug 2018
#pragma once
#include <map>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include "Simple-XML/Element.hpp"

namespace simple_xml {

	class Element_Visitor {
	public:
		Element_Visitor(std::vector<Element>& elements);
		virtual ~Element_Visitor() = default;

		std::string name() const;

		std::string text() const;

		int col() const { return current_index->col_idx; }

		int row() const { return current_index->row_idx; }

		int wkt() const { return current_index->wkt_idx; }

		std::map<std::string, std::string> const& attributes() const;

		std::optional<std::string> attribute(std::string attribute_name) const;

		size_t depth() const { return current_index_path.size(); }

		std::string path_to_string() const;

		static void all_depth_first(
		  Element::Index root,
		  std::function<bool(Node_Visitor&)> callback);

	protected:
		std::vector<Element>& m_elements; // Ref to the collection of Elements.
		Element::Index current_index;

		typedef std::vector<Element::Index> Node_Path_Type;
		typedef Node_Path_Type::const_iterator Node_Path_Iterator;
		Node_Path_Type current_index_path;

		bool is_current_index_root() const { return current_index_path.empty(); }

		virtual bool resume_parent();

		virtual bool visit_first_child();

		virtual bool visit_next_sibling();

		void throw_if_current_is_null() const;
	};
} // namespace simple_xml
