/**
 * @file   Simple-XML/Element.hpp
 * @date   Started 2019-04-18
 * @author Lester J. Dowling
 */
#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace simple_xml {
	using std::map;
	using std::string;
	using std::vector;
	using std::optional;

	/**
	 * A single node within the XML document model.
	 */
	class Element {
		string m_name;
		string m_text;

	public:
		using Index = std::size_t;

		map<string, string> attributes;
		vector<Index> children;
		int row_idx{ -1 }; // The row number this node is in.
		int col_idx{ -1 }; // The column number this node is in.
		int wkt_idx{ -1 }; // The worksheet number this node is in.

		Element() = default;

		Element(vector<char> const& n) { set_name(n); }

		Element(string const& n) { set_name(n); }

		Element(vector<char> const& n, vector<char> const& t)
		{
			set_name(n);
			set_text(t);
		}

		Element(string const& n, string const& t)
		{
			set_name(n);
			set_text(t);
		}

		string const& name() const { return m_name; }
		void set_name(vector<char> const& y);
		void set_name(string const& y);

		string const& text() const { return m_text; }
		void set_text(vector<char> const& y);
		void set_text(string const& y);

		optional<string> attribute(string attribute_name) const;
	};
} // namespace simple_xml
