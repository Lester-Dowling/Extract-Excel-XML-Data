/**
 * @file   Unit-Practices/practice/tokenizer.cpp
 * @date   Started 2019-04-12
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

/*
 * Practise and learn about the operation of Boost tokenizer.
 */

BOOST_AUTO_TEST_SUITE(practice_tokenizer_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(break_up_a_phrase_into_words)
{
	using namespace std;
	using namespace boost;
	string s = "This is,  a test";
	tokenizer<> tok{ s };
	tokenizer<>::iterator itr = tok.begin();
	BOOST_TEST(*itr++ == "This");
	BOOST_TEST(*itr++ == "is");
	BOOST_TEST(*itr++ == "a");
	BOOST_TEST(*itr++ == "test");
}

BOOST_AUTO_TEST_CASE(break_up_a_csv_into_fields)
{
	using namespace std;
	using namespace boost;
	string s = "\"Field1\",\" Field 2 \",123,456.789";
	using Tokenizer = tokenizer<escaped_list_separator<char>>;
	Tokenizer tok{ s };
	Tokenizer::iterator itr = tok.begin();
	BOOST_TEST(*itr++ == "Field1");
	BOOST_TEST(*itr++ == " Field 2 ");
	BOOST_TEST(*itr++ == "123");
	BOOST_TEST(*itr++ == "456.789");
}


BOOST_AUTO_TEST_SUITE_END()
