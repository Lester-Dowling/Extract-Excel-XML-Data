/**
 * @file   Pseudo-XPath/Grade.hpp
 * @date   Started 2019-03-28
 * @author Lester J. Dowling
 */
#pragma once
#include <list>
#include <memory>
#include <string>
namespace pseudo_xpath {

	/**
	 * Filter atom for a single attribute within an XML tag.
	 */
	struct Attribute_Filter {
		std::string attribute_name;
		char filter_operator; // can be: = < >
		std::string filter_value;
		int filter_number;
		bool good_filter_number;
	};

	class Grade {
	public:
		std::string xml_node_name;
		std::list<Attribute_Filter> attribute_filters; // Default: all filters must be true.
		bool is_alternation{ false }; // == true => only one filter must be true.
		std::shared_ptr<Grade> next_grade;

		typedef std::shared_ptr<Grade> SP;

		inline std::string const& name() const { return xml_node_name; }

		inline std::list<Attribute_Filter>& filters() { return attribute_filters; }

		inline SP next() const { return next_grade; }

		std::string to_string();

		static std::string path_to_string(const SP root);

		/**
		 * Concatenate a list of @c Grade's attributes into a @c string.
		 */
		std::string attribute_filters_to_string();
	};
} // namespace pseudo_xpath
