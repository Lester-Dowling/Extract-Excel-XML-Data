// Unit-Tests/test/grammars.cpp
// Started 15 Aug 2018
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

/*
 * Unit tests to verify the correct operation of grammars.
 */

BOOST_AUTO_TEST_SUITE(test_grammars_suite, *utf::enabled())

namespace learning {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct word_list_verifier : qi::grammar<Iterator, ascii::space_type>
	{
		qi::rule<Iterator, ascii::space_type> start, word;

		word_list_verifier()
		  : base_type{ start }
		{
			word = +ascii::alnum;
			start = word >> *(word);
		}
	};

	template<typename Iterator>
	struct word_list : qi::grammar<Iterator, std::vector<std::string>(), ascii::space_type>
	{
		qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> start;
		qi::rule<Iterator, std::string(), ascii::space_type> word;

		word_list()
		  : base_type{ start }
		{
			word %= qi::lexeme[+ascii::alnum];
			start %= *(word);
		}
	};

	template<typename Iterator>
	struct head_word : qi::grammar<Iterator, std::vector<std::string>(), ascii::space_type>
	{
		qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> start;
		qi::rule<Iterator, std::string(), ascii::space_type> head;
		qi::rule<Iterator, std::string(), ascii::space_type> tail;

		head_word()
		  : base_type{ start }
		{
			head %= qi::lexeme[+ascii::alnum];
			tail %= qi::no_skip[*(qi::char_ - '>')];
			start %= qi::lit('<') >> head >> tail >> qi::lit('>');
		}
	};
} // namespace learning

BOOST_AUTO_TEST_CASE(verify_list_of_words)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	std::string str = " 22   333   4444  55555  a bb ccc dddd e .";

	typedef learning::word_list_verifier<std::string::const_iterator>
	  word_list_verifier_grammar;
	word_list_verifier_grammar g; // Our grammar

	std::string::const_iterator iter = str.begin();
	std::string::const_iterator end = str.end();
	BOOST_TEST(qi::phrase_parse(iter, end, g, ascii::space));
}

BOOST_AUTO_TEST_CASE(list_of_words)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	std::string str = " 22   333   4444  55555  a bb ccc dddd e .";
	typedef std::string::const_iterator Iterator;
	typedef learning::word_list<Iterator> word_list_grammar;
	word_list_grammar g; // Our grammar

	Iterator iter = str.begin();
	Iterator end = str.end();
	std::vector<std::string> result;
	BOOST_TEST(qi::phrase_parse(iter, end, g, ascii::space, result));
	std::ostringstream oss;
	oss << "Result = ";
	for (auto ss : result) {
		oss << ss << ' ';
	}
	// BOOST_TEST_MESSAGE(oss.str().c_str());
	BOOST_TEST(std::string{ "22" } == result.front());
	BOOST_TEST(std::string{ "e" } == result.back());
	BOOST_TEST(size_t{ 9 } == result.size());
}

BOOST_AUTO_TEST_CASE(head_word)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	std::string str = "<22   333   4444  55555  a bb ccc dddd e >";
	typedef std::string::const_iterator Iterator;
	typedef learning::head_word<Iterator> word_list_grammar;
	word_list_grammar g; // Our grammar

	Iterator iter = str.begin();
	Iterator end = str.end();
	std::vector<std::string> result;
	BOOST_TEST(qi::phrase_parse(iter, end, g, ascii::space, result));
	std::ostringstream oss;
	oss << "Result = ";
	for (auto ss : result) {
		oss << ss << ' ';
	}
	// BOOST_TEST_MESSAGE(oss.str().c_str());
	BOOST_TEST(std::string{ "22" } == result.front());
	BOOST_TEST(size_t{ 2 } == result.size());
}

BOOST_AUTO_TEST_SUITE_END()
