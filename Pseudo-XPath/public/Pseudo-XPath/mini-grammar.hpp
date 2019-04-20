// Pseudo-XPath/mini-grammar.hpp
// Started 29 Mar 2019
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
#include "Pseudo-XPath/Grade.hpp"
#include "Pseudo-XPath/Grade-Creator.hpp"
namespace pseudo_xpath {
	namespace fusion = boost::fusion;
	namespace phx = boost::phoenix;
	namespace spirit = boost::spirit;
	namespace qi = spirit::qi;
	namespace ascii = spirit::ascii;
	constexpr char kDQ = '"';
	constexpr char kSQ = '\'';

	template<typename Iterator>
	class mini_grammar : public qi::grammar<Iterator, void(), ascii::space_type> //
	{
		using base_type = typename mini_grammar::base_type;
		using start_type = typename base_type::start_type;
		using attr_type = typename start_type::attr_type;
		using This = typename mini_grammar<Iterator>;

		using char_seq_t = std::vector<char>;
		using char_seq_rule_t = qi::rule<Iterator, char_seq_t(), ascii::space_type>;
		using quoted_attr_t = fusion::vector<char_seq_t, int>;
		using quoted_seq_rule_t = qi::rule<Iterator, quoted_attr_t(), ascii::space_type>;
		using filter_attr_t = fusion::vector<char_seq_t, char, quoted_attr_t>;

	  public:
		/**
		 * The parsed result of this grammar.
		 */
		Grade::SP result{ new Grade };

	  private:
		Grade_Creator::SP creator{ new Grade_Creator{ result } };
		char_seq_rule_t xml_identifier;
		char_seq_rule_t attribute_name;
		quoted_seq_rule_t attribute_value_dq; // A double-quoted attribute value.
		quoted_seq_rule_t attribute_value_sq; // A single-quoted attribute value.
		quoted_seq_rule_t attribute_value_nq; // An unquoted attribute value.
		quoted_seq_rule_t attribute_value_dg; // An attribute value with only digits.
		quoted_seq_rule_t implicit_value;
		quoted_seq_rule_t attribute_value;
		const int dq_flag{ 1 }; // Flag for double quoted value.
		const int sq_flag{ 2 }; // Flag for single quoted value.
		const int nq_flag{ 3 }; // Flag for unquoted value (no quote).
		const int dg_flag{ 4 }; // Flag for value with digits only.
		enum Quote_Type : int
		{
			kDoubleQuoted = 1,
			kSingleQuoted,
			kNotQuoted,
			kDigitsOnly
		};
		qi::rule<Iterator, filter_attr_t(), ascii::space_type> binary_filter;
		qi::rule<Iterator, void(), ascii::space_type> attributes;
		start_type start;

		char_seq_t remainder;
		void set_remainder(char_seq_t const& r) { remainder = r; }

		/**
		 * A component part of a pseudo XPath was successfully parsed, so
		 * add it to the list of @c Grades.
		 */
		void add_filter(filter_attr_t const& v)
		{
			const std::string attribute_name{ fusion::at_c<0>(v).begin(),
											  fusion::at_c<0>(v).end() };
			const char filter_operator{ fusion::at_c<1>(v) };
			const quoted_attr_t& quoted_attr = fusion::at_c<2>(v);
			const std::string filter_value{ fusion::at_c<0>(quoted_attr).begin(),
											fusion::at_c<0>(quoted_attr).end() };
			const int quote_type = fusion::at_c<1>(quoted_attr);
			const bool quoted = nq_flag != quote_type;
			const bool digits_only = dg_flag == quote_type;
			creator->add_filter(
			  attribute_name, filter_operator, filter_value, quoted, digits_only);
		}

		/**
		 * An implicitly given filter within a pseudo XPath was successfully parsed,
		 * so add it to the list of @c Grades.
		 */
		void add_implicit_filter(quoted_attr_t const& v)
		{
			const std::string attribute_name{};
			const char filter_operator{ '=' };
			const std::string filter_value{ fusion::at_c<0>(v).begin(),
											fusion::at_c<0>(v).end() };
			const int quote_type = fusion::at_c<1>(v);
			const bool quoted = nq_flag != quote_type;
			const bool digits_only = dg_flag == quote_type;
			creator->add_filter(
			  attribute_name, filter_operator, filter_value, quoted, digits_only);
		}

	  public:
		mini_grammar()
		  : base_type{ start }
		  , xml_identifier{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_name{ qi::lexeme[+qi::char_("a-zA-Z0-9:_-")] }
		  , attribute_value_dq{ qi::lit(kDQ) >> qi::no_skip[*(qi::char_ - kDQ)] >
								qi::lit(kDQ) >> qi::attr(phx::ref(dq_flag)) }
		  , attribute_value_sq{ qi::lit(kSQ) >> qi::no_skip[*(qi::char_ - kSQ)] >
								qi::lit(kSQ) >> qi::attr(phx::ref(sq_flag)) }
		  , attribute_value_nq{ qi::lexeme[+qi::alnum] >> qi::attr(phx::ref(nq_flag)) }
		  , attribute_value_dg{ qi::lexeme[+qi::digit] >> qi::attr(phx::ref(dg_flag)) }
		  , attribute_value{ attribute_value_dq | attribute_value_sq | attribute_value_dg |
							 attribute_value_nq }
		  , implicit_value{ attribute_value_dq | attribute_value_sq | attribute_value_dg }
		  , binary_filter{ attribute_name >> qi::char_("=<>") >> attribute_value }
		  , attributes{ implicit_value[boost::bind(&This::add_implicit_filter, this, _1)] |
						  +binary_filter[boost::bind(&This::add_filter, this, _1)],
						"attributes" }
		  , start{ (xml_identifier[boost::bind(&Grade_Creator::new_element, creator, _1)] >>
					-(qi::lit('[') > attributes > qi::lit(']'))) %
					 qi::lit(','),
				   "start" }
		{
			qi::on_error<qi::fail>(
			  start,
			  std::cout << phx::val("Error! Expecting ") << qi::_4 // what failed?
						<< std::endl);
		}
	};
} // namespace pseudo_xpath
