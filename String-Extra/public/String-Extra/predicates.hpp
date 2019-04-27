/**
 * @file   String-Extra/predicates.hpp
 * @date   Started 2019-04-28
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
namespace string_extra {
	using std::string;


	inline bool is_comma(char ch) { return ch == ','; }


	/**
	 * Return true if the given string is wrapped by single or double quotes.
	 */
	inline bool is_quoted(string const& str)
	{
		if (str.front() == '\'' && str.back() == '\'')
			return true;
		if (str.front() == '"' && str.back() == '"')
			return true;
		return false;
	}

} // namespace string_extra
