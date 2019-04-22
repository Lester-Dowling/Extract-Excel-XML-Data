// Pseudo-XPath/Grade.cpp
// Started 14 Apr 2019
#pragma once
#include "pch-xpath-parser.hpp"
#include "Pseudo-XPath/Grade.hpp"
namespace pseudo_xpath {
	using std::string;

	std::string Grade::attribute_filters_to_string()
	{
		std::string result;
		if (!attribute_filters.empty()) {
			result += '[';
			bool insert_space = false;
			for (Attribute_Filter const& af : attribute_filters) {
				if (insert_space)
					result += " ";
				insert_space = true;
				result += af.attribute_name;
				result += af.filter_operator;
				if (!af.good_filter_number)
					result += '\'';
				result += af.filter_value;
				if (!af.good_filter_number)
					result += '\'';
			}
			result += ']';
		}
		return result;
	}

	/*static*/ std::string Grade::path_to_string(const SP root)
	{
		if (!root)
			return {};
		Grade::SP v = root->next();
		string grade_path;
		if (v) {
			grade_path = v->name();
			grade_path += v->attribute_filters_to_string();
		}
		while (v = v->next()) {
			grade_path += " --> " + v->name();
			grade_path += v->attribute_filters_to_string();
		}
		return grade_path;
	}
} // namespace pseudo_xpath
