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


	std::string Grade::to_string()
	{
		if (name().empty())
			return {};
		else
			return name() + attribute_filters_to_string() +
				   (next() ? " --> " + next()->to_string() : string{});
	}


	/*static*/ std::string Grade::path_to_string(const SP root)
	{
		if (!root)
			return {};
		else if (!root->next())
			return {};
		else
			return root->next()->to_string();
	}
} // namespace pseudo_xpath
