// Pseudo-XPath/Grade.hpp
// Started 28 Mar 2019
#pragma once
#include <list>
#include <memory>
#include <string>
namespace pseudo_xpath {

	struct Attribute_Filter
	{
		std::string attribute_name;
		char filter_operator; // can be: = < >
		std::string filter_value;
		int filter_number;
		bool good_filter_number;
	};

	class Grade
	{
	  public:
		std::string xml_node_name;
		std::list<Attribute_Filter> attribute_filters;
		std::shared_ptr<Grade> next_grade;

		typedef std::shared_ptr<Grade> SP;

		inline std::string const& name() const { return xml_node_name; }

		inline std::list<Attribute_Filter>& filters() { return attribute_filters; }

		inline SP next() const { return next_grade; }

		static std::string path_to_string(const SP root);
	};
} // namespace pseudo_xpath
