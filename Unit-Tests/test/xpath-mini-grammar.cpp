// Unit-Tests/test/xpath-mini-grammar.cpp
// Started 15 Aug 2018
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Pseudo-XPath-Parser/mini-grammar.hpp"
#include "Pseudo-XPath-Parser/Grade.hpp"
#include <boost/spirit/repository/include/qi_confix.hpp>
namespace utf = boost::unit_test;
namespace fsn = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;
namespace repo = spirit::repository;
namespace phx = boost::phoenix;
using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
using Grade = pseudo_xpath_parser::Grade;
using XPath_Grammar = pseudo_xpath_parser::mini_grammar<Stream_Iterator>;

/*
 * Unit tests to verify the correct operation of the pseudo XPath grammar.
 */

BOOST_AUTO_TEST_SUITE(test_xpath_mini_grammar_suite, *utf::enabled())


BOOST_AUTO_TEST_CASE(single_attribute_filter)
{
	const char* const xpath_text = "Cell[ Column=1 ]   ";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Cell");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Column");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_number == 1);
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(Grade::path_to_string(g.result) == "Cell[Column=1]");
}

BOOST_AUTO_TEST_CASE(implicit_attribute_filter_named)
{
	const char* const xpath_text = "Worksheet['Profit and Loss']";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Worksheet");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "Profit and Loss");
	BOOST_TEST(Grade::path_to_string(g.result) == "Worksheet[Worksheet='Profit and Loss']");
}

BOOST_AUTO_TEST_CASE(implicit_attribute_filter_numbered)
{
	const char* const xpath_text = "Worksheet[1]";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Worksheet");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(v->filters().front().good_filter_number);
	BOOST_TEST(Grade::path_to_string(g.result) == "Worksheet[Worksheet=1]");
}

BOOST_AUTO_TEST_CASE(column_path_1)
{
	const char* const xpath_text =
	  "Workbook, Worksheet[1], Table, Row[1],Cell,Data[ss:Type=String]";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);
}

BOOST_AUTO_TEST_CASE(grammar_xml_attribute_filter)
{
	const char* const xpath_text = "  Worksheet   ,   Table   ,   Row[ Row=12 ] , "
								   "Cell[ Column=1  ss::ValueType=Number ]   ";
	XPath_Grammar g;
	std::istringstream sample_stream{ xpath_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 0;
	Grade::SP v = g.result;
	BOOST_TEST(v);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 1);
	BOOST_TEST(v->name() == "Worksheet");
	BOOST_TEST(v->filters().empty());

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 2);
	BOOST_TEST(v->name() == "Table");
	BOOST_TEST(v->filters().empty());

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 3);
	BOOST_TEST(v->name() == "Row");
	BOOST_TEST(v->filters().size() == 1);
	BOOST_TEST(v->filters().front().attribute_name == "Row");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "12");
	BOOST_TEST(v->filters().front().good_filter_number);

	v = v->next();
	BOOST_TEST(v);
	path_depth++;
	BOOST_TEST(path_depth == 4);
	BOOST_TEST(v->name() == "Cell");
	BOOST_TEST(v->filters().size() == 2);
	BOOST_TEST(v->filters().front().attribute_name == "Column");
	BOOST_TEST(v->filters().front().filter_operator == '=');
	BOOST_TEST(v->filters().front().filter_value == "1");
	BOOST_TEST(v->filters().front().good_filter_number);
	BOOST_TEST(v->filters().back().attribute_name == "ss::ValueType");
	BOOST_TEST(v->filters().back().filter_operator == '=');
	BOOST_TEST(v->filters().back().filter_value == "Number");
	BOOST_TEST(!v->filters().back().good_filter_number);

	BOOST_TEST(
	  Grade::path_to_string(g.result) ==
	  "Worksheet --> Table --> Row[Row=12] --> Cell[Column=1 ss::ValueType='Number']");
}

BOOST_AUTO_TEST_CASE(column_name_or_number)
{
	const std::string filter_by_name{ "Name of a column" };
	const std::string filter_by_number{ "6" };
	int filter_col_idx = -1;
	try {
		filter_col_idx = std::stoi(filter_by_name);
		BOOST_FAIL("Successfully parsed text");
	}
	catch (std::invalid_argument const&) {
		BOOST_TEST(filter_col_idx == -1);
	}
}

namespace learning {
	// A metafunction to compute the type
	// of the confix() construct
	template<typename Prefix, typename Suffix = Prefix>
	struct confix_spec {
		using type =
		  typename spirit::result_of::terminal<repo::tag::confix(Prefix, Suffix)>::type;
	};
} // namespace learning

BOOST_AUTO_TEST_CASE(confix_double_quote)
{
	const char kDQ = '"';
	const std::string dq{ kDQ };
	learning::confix_spec<std::string>::type const dq_confix = repo::confix(dq, dq);
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
	learning::confix_spec<std::string>::type const sq_confix = repo::confix(sq, sq);
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
