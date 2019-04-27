/**
 * @file   Unit-Practices/practice/parsers.cpp
 * @date   Started 2019-04-7
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include "testing-factor/mini-xml-tag.hpp"
#include "testing-factor/opening-tag.hpp"
#include "testing-factor/tag-attributes.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

/*
 * Practise and develop an understanding of parsers.
 */

BOOST_AUTO_TEST_SUITE(practice_parsers_suite, *utf::enabled())
namespace practice {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	bool parse_numbers(Iterator first, Iterator last)
	{
		bool r = qi::phrase_parse(
		  first,								/* start iterator */
		  last,									/* end iterator */
		  qi::double_ >> *(',' >> qi::double_), /* the parser */
		  ascii::space							/* the skip-parser */
		);
		if (first != last) // Fail if we did not get a full match.
			return false;
		return r;
	}
} // namespace practice

namespace test {
	template<typename P>
	bool parser(char const* input, P const& p, bool full_match = true)
	{
		using boost::spirit::qi::parse;
		char const* f(input);
		char const* l(f + strlen(f));
		if (parse(f, l, p) && (!full_match || (f == l)))
			return true;
		else
			return false;
	}
} // namespace test

namespace test {
	template<typename P, typename T>
	bool parser_attr(char const* input, P const& p, T& attr, bool full_match = true)
	{
		using boost::spirit::qi::parse;
		char const* f(input);
		char const* l(f + strlen(f));
		if (parse(f, l, p, attr) && (!full_match || (f == l)))
			return true;
		else
			return false;
	}
} // namespace test

BOOST_AUTO_TEST_CASE(parse_valid_list_of_numbers) //
{
	std::string str = "2, 3, 4, 5";
	BOOST_TEST(practice::parse_numbers(str.begin(), str.end()));
}

BOOST_AUTO_TEST_CASE(fail_parse_of_invalid_numerical_list) //
{
	std::string str = "a, b, c, d";
	BOOST_TEST(!practice::parse_numbers(str.begin(), str.end()));
}

BOOST_AUTO_TEST_CASE(simple_string_parsers)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	BOOST_TEST(test::parser("boost", "boost"));				   // plain literal
	BOOST_TEST(test::parser("boost", qi::lit("boost")));	   // explicit literal
	BOOST_TEST(test::parser("boost", ascii::string("boost"))); // ascii::string
}

BOOST_AUTO_TEST_CASE(string_parsers_from_a_std_string)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	std::string s = "boost";
	BOOST_TEST(test::parser("boost", s));				 // direct
	BOOST_TEST(test::parser("boost", qi::lit(s)));		 // explicit
	BOOST_TEST(test::parser("boost", ascii::string(s))); // ascii::string
}

BOOST_AUTO_TEST_CASE(lazy_strings_using_phoenix)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace phx = boost::phoenix;
	BOOST_TEST(test::parser("boost", phx::val("boost")));				 // direct
	BOOST_TEST(test::parser("boost", qi::lit(phx::val("boost"))));		 // explicit
	BOOST_TEST(test::parser("boost", ascii::string(phx::val("boost")))); // ascii::string
}

BOOST_AUTO_TEST_CASE(symbols_with_data)
{
	namespace qi = boost::spirit::qi;
	qi::symbols<char, int> sym;
	sym.add("Apple", 1)("Banana", 2)("Orange", 3);
	int i;
	BOOST_TEST(test::parser_attr("Banana", sym, i));
	BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(symbols_with_case_insensitive_matching)
{
	namespace qi = boost::spirit::qi;
	qi::symbols<char, int> sym;
	// Symbol strings are added in lowercase:
	sym.add("apple", 1)("banana", 2)("orange", 3);
	int i;
	// ...because sym is used for case-insensitive parsing
	BOOST_TEST(test::parser_attr("Apple", qi::no_case[sym], i));
	BOOST_TEST(i == 1);
	BOOST_TEST(test::parser_attr("ORANGE", qi::no_case[sym], i));
	BOOST_TEST(i == 3);
}

BOOST_AUTO_TEST_CASE(simple_string_to_stream_parser)
{
	namespace qi = boost::spirit::qi;
	std::string str;
	BOOST_TEST(test::parser_attr("abc", qi::stream, str));
	BOOST_TEST(str == std::string{ "abc" });
}

BOOST_AUTO_TEST_CASE(parsers_for_simple_character_classification)
{
	namespace ascii = boost::spirit::ascii;
	char returned;
	BOOST_TEST(test::parser_attr("1", ascii::alnum, returned));
	BOOST_TEST(returned == '1');
	BOOST_TEST(test::parser_attr(" ", ascii::space, returned));
	BOOST_TEST(returned == ' ');
	BOOST_TEST(test::parser_attr("1", ascii::digit, returned));
	BOOST_TEST(returned == '1');
	BOOST_TEST(test::parser_attr("a", ascii::lower, returned));
	BOOST_TEST(returned == 'a');
}

BOOST_AUTO_TEST_CASE(parsers_for_character_strings)
{
	namespace ascii = boost::spirit::ascii;
	std::string returned;
	BOOST_TEST(test::parser_attr("12a", *ascii::alnum, returned));
	BOOST_TEST(returned == std::string{ "12a" });
	returned.clear();
	BOOST_TEST(test::parser_attr(" \t", +ascii::space, returned));
	BOOST_TEST(returned == std::string{ " \t" });
	returned.clear();
	BOOST_TEST(test::parser_attr("123", +ascii::digit, returned));
	BOOST_TEST(returned == std::string{ "123" });
	returned.clear();
	BOOST_TEST(test::parser_attr("abc", *ascii::lower, returned));
	BOOST_TEST(returned == std::string{ "abc" });
}

BOOST_AUTO_TEST_SUITE_END()
