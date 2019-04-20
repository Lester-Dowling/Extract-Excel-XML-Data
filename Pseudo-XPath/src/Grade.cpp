// Pseudo-XPath/Grade.cpp
// Started 14 Apr 2019
#pragma once
#include "pch-xpath-parser.hpp"
#include "Pseudo-XPath/Grade.hpp"
namespace pseudo_xpath {
	using std::string;

	void append_attribute_filters(string& grade_path, Grade::SP v)
	{
		if (!v->attribute_filters.empty()) {
			grade_path += '[';
			bool insert_space = false;
			for (Attribute_Filter const& af : v->attribute_filters) {
				if (insert_space)
					grade_path += " ";
				insert_space = true;
				grade_path += af.attribute_name;
				grade_path += af.filter_operator;
				if (!af.good_filter_number)
					grade_path += '\'';
				grade_path += af.filter_value;
				if (!af.good_filter_number)
					grade_path += '\'';
			}
			grade_path += ']';
		}
	}

	/*static*/ std::string Grade::path_to_string(const SP root)
	{
		if (!root)
			return {};
		Grade::SP v = root->next();
		string grade_path;
		if (v) {
			grade_path = v->name();
			append_attribute_filters(grade_path, v);
		}
		while (v = v->next()) {
			grade_path += " --> " + v->name();
			append_attribute_filters(grade_path, v);
		}
		return grade_path;
	}
} // namespace pseudo_xpath
