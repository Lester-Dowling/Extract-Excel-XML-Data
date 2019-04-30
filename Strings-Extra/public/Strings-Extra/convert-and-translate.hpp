/**
 * @file   Strings-Extra/convert-and-translate.hpp
 * @date   Started 2019-04-30
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <optional>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
namespace strings {
	namespace a = boost::algorithm;
	using std::string;
	using std::optional;

	/**
	 * Translate a single capital letter to a column number.
	 * For example, given "C" then translate that to "3".
	 */
	inline string translate_column_title_uppercase_letter(string column_title)
	{
		if (column_title.size() == 1) {
			if (std::isupper(column_title.front())) {
				const int corresponding_column_number =
				  1 + static_cast<int>(column_title.front()) - static_cast<int>('A');
				column_title = std::to_string(corresponding_column_number);
			}
		}
		return column_title;
	}

	/**
	 * Convert a single capital letter to a column number.
	 * For example, given "C" then convert that to 3.
	 */
	inline optional<int> convert_column_title_uppercase_letter(const string column_title)
	{
		if (column_title.size() == 1) {
			if (std::isupper(column_title.front())) {
				const int corresponding_column_number =
				  1 + static_cast<int>(column_title.front()) - static_cast<int>('A');
				return corresponding_column_number;
			}
		}
		return {};
	}
} // namespace strings
