// Unit-Tests/test/synthesized-attributes.cpp
// Started 15 Aug 2018
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML-Parser/Node.hpp"
#include "Simple-XML-Parser/Node-Creator.hpp"
#include "Simple-XML-Parser/Node-Visitor.hpp"
namespace utf = boost::unit_test;
namespace fsn = boost::fusion;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;
using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;

/*
 * Unit tests to verify the correct operation of grammars.
 */

BOOST_AUTO_TEST_SUITE(test_synthesized_attributes_suite, *utf::enabled())

BOOST_AUTO_TEST_CASE(int_value_synthesized_attribute)
{
	const std::string sample_input = "123  ";
	int result = 0;
	BOOST_TEST(qi::parse(sample_input.begin(), sample_input.end(), qi::int_, result));
	BOOST_TEST(result == 123);
}

BOOST_AUTO_TEST_CASE(std_pair_as_attribute_of_compound_component)
{
	// the following parses "1.0 2.0" into a pair of double
	const std::string sample_input = "1.0 2.0";
	const auto grammar = qi::double_ >> qi::double_;

	std::pair<double, double> p;
	// Requires include <boost/fusion/adapted/std_pair.hpp>
	BOOST_TEST(qi::phrase_parse(
	  sample_input.begin(),
	  sample_input.end(),
	  grammar,
	  ascii::space,
	  p)); // attribute to fill while parsing
	BOOST_TEST(p.first == 1.0);
	BOOST_TEST(p.second == 2.0);
}

BOOST_AUTO_TEST_CASE(std_tuple_as_attribute_of_compound_component)
{
	// the following parses "1.0 2.0" into a pair of double
	const std::string sample_input = "1.0 2.0";
	const auto grammar = qi::double_ >> qi::double_;

	std::tuple<double, double> p;
	// Requires include <boost/fusion/adapted/std_tuple.hpp>
	BOOST_TEST(qi::phrase_parse(
	  sample_input.begin(),
	  sample_input.end(),
	  grammar,
	  ascii::space,
	  p)); // attribute to fill while parsing
	BOOST_TEST(std::get<0>(p) == 1.0);
	BOOST_TEST(std::get<1>(p) == 2.0);
}

namespace practice {
	template<typename Iterator>
	struct CSV_Doubles : qi::grammar<Iterator, std::list<double>(), ascii::space_type> //
	{
		using start_type = typename CSV_Doubles::base_type::start_type;
		using attr_type = typename start_type::attr_type;
		const start_type start;

		CSV_Doubles()
		  : CSV_Doubles::base_type{ start }
		  , start{ qi::double_ % ',' }
		{
			// BOOST_TEST_MESSAGE("attr_type : " << typeid(attr_type).name());
		}
	};
} // namespace practice

BOOST_AUTO_TEST_CASE(grammar_csv_number_list_to_std_vector)
{
	const char* const sample_text = "  1.12   ,   2.23   ,   3.34   ";
	typedef practice::CSV_Doubles<Stream_Iterator> Grammar;
	Grammar::attr_type v; // std::vector<double> std::list<double> std::deque<double>
	// v.reserve(10);

	std::istringstream sample_stream{ sample_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(Grammar{}, ascii::space, v);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	BOOST_REQUIRE_EQUAL(v.size(), 3);
	auto vitr = v.cbegin();
	BOOST_REQUIRE(*vitr++ == 1.12);
	BOOST_REQUIRE(*vitr++ == 2.23);
	BOOST_REQUIRE(*vitr++ == 3.34);
	BOOST_REQUIRE(vitr == v.cend());
}

/**
 * The following test with qi::rule fails.  Do not know why.
 */
// BOOST_AUTO_TEST_CASE(rule_csv_number_list_to_std_vector)
//{
//	const char* const sample_text = "1,2,3";
//	std::vector<double> v;
//	v.reserve(10);
//	typedef boost::spirit::basic_istream_iterator<char> Iterator;
//	qi::rule<Iterator, std::vector<double>(), ascii::space_type> start;
//	std::istringstream sample_stream{ sample_text };
//	sample_stream.unsetf(std::ios::skipws);
//	sample_stream >> qi::phrase_match(start, ascii::space, v);
//	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
//	BOOST_REQUIRE_EQUAL(v.size(), 3);
//	BOOST_TEST(v.at(0) == 1);
//	BOOST_TEST(v.at(1) == 2);
//	BOOST_TEST(v.at(2) == 3);
//}

namespace practice {
	template<typename Iterator>
	struct XML_Attribute_Filter : qi::grammar<Iterator, void(), ascii::space_type> //
	{
		using base_type = typename XML_Attribute_Filter::base_type;
		using start_type = typename base_type::start_type;
		using attr_type = typename start_type::attr_type;
		using This = typename XML_Attribute_Filter<Iterator>;
		using Node = excel_xml_parser::Node;
		using Node_Creator = excel_xml_parser::Node_Creator;

		Node::SP result{ new Node };
		Node_Creator::SP creator{ new Node_Creator{ result } };

		typedef std::vector<char> CharSeq;
		qi::rule<Iterator, CharSeq(), ascii::space_type> xml_identifier;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_name;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_value_dq;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_value_sq;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_value_nq;
		qi::rule<Iterator, fsn::vector<CharSeq, CharSeq>(), ascii::space_type> assignment;
		qi::rule<Iterator, void(), ascii::space_type> attributes;
		const start_type start;

		void set_attribute(fsn::vector<CharSeq, CharSeq> const& v)
		{
			const std::string attribute_name{ fsn::at_c<0>(v).begin(),
											  fsn::at_c<0>(v).end() };
			const std::string attribute_value{ fsn::at_c<1>(v).begin(),
											   fsn::at_c<1>(v).end() };
			creator->set_attribute(attribute_name, attribute_value);
		}

		XML_Attribute_Filter()
		  : base_type{ start }
		  , xml_identifier{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_name{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_value_dq{ qi::lit('"') >> qi::no_skip[*(qi::char_ - '"')] >
								qi::lit('"') }
		  , attribute_value_sq{ qi::lit('\'') >> qi::no_skip[*(qi::char_ - '\'')] >
								qi::lit('\'') }
		  , attribute_value_nq{ qi::lexeme[+qi::alnum] }
		  , assignment{ attribute_name > qi::lit('=') >
						(attribute_value_dq | attribute_value_sq | attribute_value_nq) }
		  , attributes{ +assignment[boost::bind(&This::set_attribute, this, _1)] }
		  , start{ (xml_identifier[boost::bind(&Node_Creator::new_element, creator, _1)] >>
					-(qi::lit('[') > attributes > qi::lit(']'))) %
				   ',' }
		{}
	};
} // namespace practice

BOOST_AUTO_TEST_CASE(grammar_xml_attribute_filter)
{
	using Node_Visitor = excel_xml_parser::Node_Visitor;
	const char* const sample_text = "  Worksheet   ,   Table   ,   Row[ Row=12 ] , "
									"Cell[Column=1 ss::ValueType=Number]   ";
	typedef practice::XML_Attribute_Filter<Stream_Iterator> Grammar;
	Grammar g;

	std::istringstream sample_stream{ sample_text };
	sample_stream.unsetf(std::ios::skipws);
	sample_stream >> qi::phrase_match(g, ascii::space);
	BOOST_REQUIRE(sample_stream.good() || sample_stream.eof());
	int path_depth = 1;
	Node_Visitor::all_depth_first(g.result, [&](Node_Visitor& v) -> bool {
		switch (path_depth++) {
			case 1:
				BOOST_TEST(v.name() == "Worksheet");
				return true;
			case 2:
				BOOST_TEST(v.name() == "Table");
				return true;
			case 3:
				BOOST_TEST(v.name() == "Row");
				BOOST_REQUIRE(v.attribute("Row").has_value());
				BOOST_TEST(v.attribute("Row").value() == "12");
				return true;
			case 4:
				BOOST_TEST(v.name() == "Cell");
				BOOST_REQUIRE(v.attribute("Column").has_value());
				BOOST_TEST(v.attribute("Column").value() == "1");
				BOOST_REQUIRE(v.attribute("ss::ValueType").has_value());
				BOOST_TEST(v.attribute("ss::ValueType").value() == "Number");
				return true;
			default:
				BOOST_FAIL("XML node visitor path too deep");
				return false;
		}
	});
}

BOOST_AUTO_TEST_SUITE_END()
