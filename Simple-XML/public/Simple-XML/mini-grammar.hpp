/**
 * @file   Simple-XML/mini-grammar.hpp
 * @date   Started 2018-08-14
 * @author Lester J. Dowling
 */
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
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Creator.hpp"
namespace simple_xml {
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	constexpr char kDQ = '"';
	constexpr char kSQ = '\'';
	inline const auto DQ = qi::lit(kDQ);

	template<typename Iterator>
	class mini_grammar
	  : public qi::grammar<Iterator, void(), ascii::space_type> //
	{
		using Base_Type = typename mini_grammar::base_type;
		using Start_Type = typename Base_Type::start_type;
		using Attr_Type = typename Start_Type::attr_type;
		using This = mini_grammar<Iterator>;
		using Char_Seq = std::vector<char>;
		using Char_Seq_Rule = qi::rule<Iterator, Char_Seq(), ascii::space_type>;
		using Assignment_Rule =
		  qi::rule<Iterator, fusion::vector<Char_Seq, Char_Seq>(), ascii::space_type>;

		Element_Creator *creator;
		Char_Seq_Rule xml_identifier;
		Char_Seq_Rule attribute_name;
		Char_Seq_Rule attribute_value;
		Assignment_Rule assignment;
		Start_Type attributes;
		Char_Seq_Rule text_node;
		Start_Type closing_tag;
		Start_Type comment;
		Start_Type xml_header;
		Start_Type tail;
		// The following rules are recursive:
		Start_Type sibling;
		Start_Type children;
		Start_Type element;
		Start_Type start;

		std::vector<char> remainder;
		void set_remainder(std::vector<char> y) { remainder = y; }

		void set_attribute(fusion::vector<Char_Seq, Char_Seq> const &v)
		{
			const std::string attribute_name{ fusion::at_c<0>(v).begin(),
											  fusion::at_c<0>(v).end() };
			const std::string attribute_value{ fusion::at_c<1>(v).begin(),
											   fusion::at_c<1>(v).end() };
			creator->set_attribute(attribute_name, attribute_value);
		}

	public:
		mini_grammar(Element_Creator *element_creator)
		  : Base_Type(start, "start")
		  , creator{ element_creator }
		  , xml_identifier{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_name{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_value{ qi::lit(kDQ) >> qi::no_skip[*(qi::char_ - kDQ)] >
							 qi::lit(kDQ) }
		  , assignment{ attribute_name > qi::lit('=') > attribute_value, "assignment" }
		  , attributes{ *(assignment[boost::bind(&This::set_attribute, this, _1)]) }
		  , text_node{ qi::lexeme[+(qi::char_ - '<' - '>')]
								 [boost::bind(&Element_Creator::set_text, creator, _1)] }
		  , closing_tag{ qi::lit("</") >> //
						   xml_identifier[boost::bind(
							 &Element_Creator::verify_closing_tag,
							 creator,
							 _1)] > qi::lit('>'),
						 "closing_tag" }
		  , xml_header{ qi::lit("<?") > qi::omit[+(qi::char_ - '?' - '<' - '>')] >
						  qi::lit("?>"),
						"xml-header" }
		  , comment{ qi::lit("<!--") > qi::omit[+(qi::char_ - '>')] > qi::lit("-->"),
					 "comment" }
		  , tail{ qi::lexeme[*ascii::print][boost::bind(&This::
			  set_remainder, this, _1)] }
		{
			element %=
			  qi::lit('<') >>
			  xml_identifier[boost::bind(&Element_Creator::new_element, creator, _1)] >>
			  attributes >>
			  (qi::lit("/>")[boost::bind(&Element_Creator::close_singleton, creator)] |
			   (qi::lit('>') >> children >> closing_tag));

			sibling = element | text_node | comment;

			children = *sibling;

			start = *xml_header > element >> tail;

			element.name("element");
			children.name("children");
			sibling.name("sibling");

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
} // namespace simple_xml
