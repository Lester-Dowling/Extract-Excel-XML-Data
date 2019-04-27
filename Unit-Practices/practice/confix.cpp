/**
 * @file   Unit-Practices/practice/confix.cpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
namespace utf = boost::unit_test;
namespace fsn = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace repo = spirit::repository;
namespace phx = boost::phoenix;

/*
 * Practise and learn about spirit::repository::confix.
 */
BOOST_AUTO_TEST_SUITE(practice_confix_suite, *utf::enabled())
namespace practice {
	// A metafunction to compute the type
	// of the confix() construct
	template<typename Prefix, typename Suffix = Prefix>
	struct confix_spec {
		using type =
		  typename spirit::result_of::terminal<repo::tag::confix(Prefix, Suffix)>::type;
	};
} // namespace practice

BOOST_AUTO_TEST_CASE(confix_double_quote)
{
	const char kDQ = '"';
	const std::string dq{ kDQ };
	practice::confix_spec<std::string>::type const dq_confix = repo::confix(dq, dq);
	const char* const sample_data = "Sample quoted string";
	const std::string sample_dq_string{ dq + sample_data + dq };
	std::string attr_dq;
	bool dqr = qi::parse(
	  sample_dq_string.begin(),
	  sample_dq_string.end(),
	  dq_confix[*(qi::char_ - kDQ)],
	  attr_dq);
	BOOST_TEST(dqr);
	BOOST_TEST(attr_dq == sample_data);
}

BOOST_AUTO_TEST_CASE(confix_single_quote)
{
	const char kSQ = '\'';
	const std::string sq{ kSQ };
	practice::confix_spec<std::string>::type const sq_confix = repo::confix(sq, sq);
	const char* const sample_data = "Sample quoted string";
	const std::string sample_sq_string{ sq + sample_data + sq };
	std::string attr_sq;
	bool sqr = qi::parse(
	  sample_sq_string.begin(),
	  sample_sq_string.end(),
	  sq_confix[*(qi::char_ - kSQ)],
	  attr_sq);
	BOOST_TEST(sqr);
	BOOST_TEST(attr_sq == sample_data);
}

template<typename P, typename T>
bool test_parser_attr(char const* input, P const& p, T& attr, bool full_match = true)
{
	char const* f{ input };
	char const* l(f + strlen(f));
	if (qi::parse(f, l, p, attr) && (!full_match || (f == l)))
		return true;
	else
		return false;
}

BOOST_AUTO_TEST_CASE(attr)
{
	std::string str;
	test_parser_attr("", qi::attr("boost"), str);
	BOOST_TEST(str == "boost");

	double d;
	test_parser_attr("", qi::attr(1.0), d);
	BOOST_TEST(d == 1.0);

	d = 0.0;
	double d1 = 1.2;
	test_parser_attr("", qi::attr(phx::ref(d1)), d);
	BOOST_TEST(d == 1.2);
}

BOOST_AUTO_TEST_CASE(const_char_seq)
{
	constexpr char kDQ = '"';
	constexpr char kSQ = '\'';
	typedef std::vector<char> char_seq_t;
	typedef const char_seq_t const_char_seq_t;
	const_char_seq_t dq{ kDQ }, sq{ kSQ };
	BOOST_TEST(dq.size() == 1);
	BOOST_TEST(sq.size() == 1);
	BOOST_TEST(dq.front() == kDQ);
	BOOST_TEST(sq.front() == kSQ);
}

BOOST_AUTO_TEST_SUITE_END()
