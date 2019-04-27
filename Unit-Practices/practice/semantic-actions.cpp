/**
 * @file   Unit-Practices/practice/semantic-actions.cpp
 * @date   Started 2019-04-12
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
#include "testing-factor/mini-xml-tag.hpp"
#include "testing-factor/opening-tag.hpp"
#include "testing-factor/tag-attributes.hpp"
#include "io/pair.hpp"
namespace utf = boost::unit_test;

/*
 * Practise and develop an understanding of semantic actions.
 */

BOOST_AUTO_TEST_SUITE(practice_semantic_actions_suite, *utf::enabled())
namespace practice {
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct int_parser : qi::grammar<Iterator, void(), ascii::space_type> {
		typedef int_parser<Iterator> This;
		typedef int Result_Type;
		Result_Type result = 0;
		void set_result(int i) { result = i; }
		qi::rule<Iterator, void(), ascii::space_type> start;

		int_parser()
		  : base_type{ start }
		{
			start = qi::lit('<') >> qi::int_[boost::bind(&This::set_result, this, _1)] >>
					qi::lit('>');
		}
	};

	template<typename Iterator>
	struct coords_parser : qi::grammar<Iterator, void(), ascii::space_type> {
		typedef coords_parser<Iterator> This;
		typedef std::pair<double, double> Result_Type;
		Result_Type result{};
		void set_first(double x) { result.first = x; }
		void set_second(double y) { result.second = y; }
		qi::rule<Iterator, void(), ascii::space_type> start;

		coords_parser()
		  : base_type{ start }
		{
			start = qi::lit('(') >> qi::double_[boost::bind(&This::set_first, this, _1)] >>
					qi::lit(',') >> qi::double_[boost::bind(&This::set_second, this, _1)] >>
					qi::lit(')');
		}
	};

	template<typename Iterator>
	struct assignment_parser : qi::grammar<Iterator, void(), ascii::space_type> {
		typedef assignment_parser<Iterator> This;
		typedef std::pair<std::string, std::string> Result_Type;
		Result_Type result{};
		void append_head(char x) { result.first += x; }
		void set_head(std::vector<char> x) { result.first.assign(x.begin(), x.end()); }
		void set_tail(std::vector<char> y) { result.second.assign(y.begin(), y.end()); }
		qi::rule<Iterator, void(), ascii::space_type> start;
		qi::rule<Iterator, void(), ascii::space_type> head;
		qi::rule<Iterator, void(), ascii::space_type> tail;

		assignment_parser()
		  : base_type{ start }
		{
			auto head =
			  qi::lexeme[+ascii::alnum[boost::bind(&This::append_head, this, _1)]];
			// [boost::bind(&This::append_head, this, _1)]
			// auto tail = qi::lit('"') >> qi::no_skip[*(qi::char_ - '"')] >> qi::lit('"');
			// auto tail = qi::no_skip[*(qi::char_ - '"')];

			auto tail = qi::lexeme[*qi::char_];
			// [boost::bind(&This::set_tail, this, _1)]
			// Logger::WriteMessage(typeid(tail).name());
			// auto start = head >> qi::lit('=') >> tail;
			start = head; // >> tail;
		}
	};
} // namespace practice

BOOST_AUTO_TEST_CASE(int_parser_semantic_action)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	using StringIterator = std::string::const_iterator;
	using Parser = practice::int_parser<StringIterator>;
	const std::string str = " <22> ";
	Parser parser;
	StringIterator itr = str.begin(), end = str.end();
	BOOST_TEST(qi::phrase_parse(itr, end, parser, ascii::space));
	BOOST_TEST(parser.result == 22);
}

BOOST_AUTO_TEST_CASE(double_parser_semantic_action)
{
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	const std::string str = "  (  11  ,  22.3 )   ";
	typedef std::string::const_iterator StringIterator;
	practice::coords_parser<StringIterator> parser;
	StringIterator itr = str.begin(), end = str.end();
	BOOST_TEST(qi::phrase_parse(itr, end, parser, ascii::space));
	BOOST_TEST(parser.result == std::make_pair(11.0, 22.3));
}

BOOST_AUTO_TEST_SUITE_END()
