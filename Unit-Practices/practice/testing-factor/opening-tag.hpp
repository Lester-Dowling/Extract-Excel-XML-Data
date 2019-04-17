// opening-tag.hpp
// Started 14 Aug 2018
#pragma once
#include <string>
#include <boost/spirit/include/qi.hpp>

namespace practice {
	// namespace fusion = boost::fusion;
	// namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct opening_tag : qi::grammar<Iterator, mini_xml_tag(), ascii::space_type>
	{
		qi::rule<Iterator, mini_xml_tag(), ascii::space_type> start;
		qi::rule<Iterator, std::string(), ascii::space_type> head;
		qi::rule<Iterator, std::string(), ascii::space_type> tail;

		opening_tag()
		  : base_type{ start }
		{
			head %= qi::lexeme[+ascii::alnum];
			tail %= qi::no_skip[*(qi::char_ - '>')];
			start %= qi::lit('<') >> head >> tail >> qi::lit('>');
		}
	};
}
