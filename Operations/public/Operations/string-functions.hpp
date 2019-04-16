/**
 * @file   Operations/string-functions.hpp
 * @date   Started Wed 10 Apr 2019 7:50:00 AM EST
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>

namespace operations {
	namespace a = boost::algorithm;
	using std::string;

	inline bool is_comma(char ch) { return ch == ','; }

	inline string erase_commas(string text)
	{
		text.erase(
		  std::remove_if(text.begin(), text.end(), boost::bind(is_comma, _1)), text.end());
		return text;
	}

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

	/**
	 * Append the string @c nm onto the end of @c bdr but wrap quotes around @c nm
	 * if it is not a number.  If @c nm is empty, throw a @c runtime_error with
	 * the given error message.
	 */
	inline void append_quoted_if_not_number(string& bdr, string nm, string empty_error_msg)
	{
		a::trim(nm);
		if (nm.empty())
			throw std::runtime_error{ empty_error_msg };
		double nm_parsed_number;
		std::istringstream nm_iss{ nm };
		nm_iss >> nm_parsed_number;
		if (nm_iss.eof())
			// Valid number => don't quote it:
			bdr += nm;
		else if (!is_quoted(nm)) {
			// Wrap quotes around non-numbers:
			bdr += '\'';
			bdr += nm;
			bdr += '\'';
		}
		else
			// Ref is already quoted, so append as is:
			bdr += nm;
	}
} // namespace operations
