/**
 * @file   Pseudo-XPath/parsing.hpp
 * @date   Started 2019-05-24
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <sstream>
#include "Pseudo-XPath/Grade.hpp"
namespace pseudo_xpath {
	using std::string;

	/**
	 * Parse the given text as a pseudo XPath.
	 *
	 * @return The root node of the parsed pseudo XPath.
	 */
	Grade::SP parse(const string xpath_text);
}
