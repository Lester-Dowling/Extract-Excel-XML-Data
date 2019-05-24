/**
 * @file   Pseudo-XPath/parsing.cpp
 * @date   Started 2019-05-24
 * @author Lester J. Dowling
 */
#pragma once
#include "pch-xpath-parser.hpp"
#include "Pseudo-XPath/parsing.hpp"
#include "Pseudo-XPath/mini-grammar.hpp"
namespace pseudo_xpath {
	using std::string;
	using std::runtime_error;
	using String_Iterator = string::const_iterator;
	using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
	using XPath_Grammar = pseudo_xpath::mini_grammar<String_Iterator>;

	Grade::SP parse(const string xpath_text)
	{
		XPath_Grammar xpath_parser;
		String_Iterator sitr = xpath_text.begin();
		String_Iterator const send = xpath_text.end();
		if (!qi::phrase_parse(sitr, send, xpath_parser, ascii::space))
			throw runtime_error{ "Failed to parse this XPath: " + xpath_text };
		return xpath_parser.result;
	}
} // namespace pseudo_xpath
