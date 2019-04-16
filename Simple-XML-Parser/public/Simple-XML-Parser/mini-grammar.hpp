// Simple-XML-Parser/mini-grammar.hpp
// Started 14 Aug 2018
#pragma once
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/bind.hpp>
#include "Simple-XML-Parser/Node.hpp"
#include "Simple-XML-Parser/Node-Creator.hpp"
namespace excel_xml_parser {
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct mini_grammar : qi::grammar<Iterator, void(), ascii::space_type> //
	{
		using base_type = typename mini_grammar::base_type;
		using start_type = typename base_type::start_type;
		using attr_type = typename start_type::attr_type;
		using Node = excel_xml_parser::Node;
		using Node_Creator = excel_xml_parser::Node_Creator;

		Node::SP result{ new Node };
		Node_Creator::SP creator{ new Node_Creator{ result } };
		std::vector<char> remainder;
		void set_remainder(std::vector<char> y) { remainder = y; }

		start_type start;
		start_type tail;
		start_type element;
		start_type children;
		start_type sibling;
		start_type closing_tag;
		start_type header;

		typedef std::vector<char> CharSeq;
		qi::rule<Iterator, CharSeq(), ascii::space_type> xml_identifier;
		qi::rule<Iterator, CharSeq(), ascii::space_type> text_node;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_name;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_value;
		qi::rule<Iterator, fusion::vector<CharSeq, CharSeq>(), ascii::space_type>
		  assignment;
		start_type attributes;

		void set_attribute(fusion::vector<CharSeq, CharSeq> const& v)
		{
			const std::string attribute_name{ fusion::at_c<0>(v).begin(),
											  fusion::at_c<0>(v).end() };
			const std::string attribute_value{ fusion::at_c<1>(v).begin(),
											   fusion::at_c<1>(v).end() };
			creator->set_attribute(attribute_name, attribute_value);
		}

		mini_grammar()
		  : base_type(start, "start")
		{
			typedef mini_grammar<Iterator> This;

			xml_identifier %= qi::lexeme[+qi::char_("a-zA-Z0-9:_-")];
			attribute_name %= qi::lexeme[+qi::char_("a-zA-Z0-9:_-")];
			attribute_value %=
			  qi::lit('"') >> qi::no_skip[*(qi::char_ - '"')] > qi::lit('"');
			assignment %= attribute_name > qi::lit('=') > attribute_value;
			attributes = *(assignment[boost::bind(&This::set_attribute, this, _1)]);

			element =
			  qi::lit('<') >>
			  xml_identifier[boost::bind(&Node_Creator::new_element, creator, _1)] >>
			  attributes >>
			  (qi::lit("/>")[boost::bind(&Node_Creator::close_singleton, creator)] |
			   (qi::lit('>') >> children >> closing_tag));

			closing_tag =
			  qi::lit("</") >>
			  xml_identifier[boost::bind(&Node_Creator::verify_closing_tag, creator, _1)] >
			  qi::lit('>');

			text_node = qi::lexeme[+(qi::char_ - '<' - '>')]
								  [boost::bind(&Node_Creator::set_text, creator, _1)];

			sibling = element | text_node;

			children = *(sibling);

			header =
			  qi::lit("<?") > qi::omit[+(qi::char_ - '?' - '<' - '>')] > qi::lit("?>");

			start = *header > element >> tail;

			tail = qi::lexeme[*ascii::print][boost::bind(&This::set_remainder, this, _1)];
			start.name("start");
			element.name("element");
			children.name("children");
			sibling.name("sibling");
			closing_tag.name("closing_tag");
			text_node.name("text_node");
			xml_identifier.name("xml_identifier");

			qi::on_error<qi::fail>(
			  start,
			  std::cout << phoenix::val("Error! Expecting ")
						<< qi::_4 // what failed?
								  // << phoenix::val(" here: \"")
								  // << phoenix::construct<std::string>(qi::_3, qi::_2)   //
								  // iterators to error-pos, end
								  // << phoenix::val("\"")
						<< std::endl);
		}
	};
}
