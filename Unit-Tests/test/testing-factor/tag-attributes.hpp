// tag-attributes.hpp
// Started 15 Aug 2018
#pragma once
#include <string>
#include <map>
#include <boost/bind.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/spirit/include/qi.hpp>

namespace learning {
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct tag_attributes : qi::grammar<Iterator, void(), ascii::space_type>
	{
		typedef std::map<std::string, std::string> Result_Type;
		typedef Result_Type::iterator Result_Iterator;
		Result_Type result{};

		typedef std::vector<char> CharSeq;
		qi::rule<Iterator, void(), ascii::space_type> start;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_name;
		qi::rule<Iterator, CharSeq(), ascii::space_type> attribute_value;
		qi::rule<Iterator, fusion::vector<CharSeq, CharSeq>(), ascii::space_type>
		  assignment;

		void set_attribute(fusion::vector<CharSeq, CharSeq> const& v)
		{
			const std::string attribute_name{ fusion::at_c<0>(v).begin(),
											  fusion::at_c<0>(v).end() };
			const std::string attribute_value{ fusion::at_c<1>(v).begin(),
											   fusion::at_c<1>(v).end() };
			result[attribute_name] = attribute_value;
		}

		tag_attributes()
		  : tag_attributes<Iterator>::base_type{ start, "start" }
		{
			typedef tag_attributes<Iterator> This;

			attribute_name %= qi::lexeme[+qi::char_("a-zA-Z0-9:_-")];
			attribute_value %=
			  qi::lit('"') >> qi::no_skip[*(qi::char_ - '"')] > qi::lit('"');
			assignment %= attribute_name > qi::lit('=') > attribute_value;
			start = +(assignment[boost::bind(&This::set_attribute, this, _1)]);

			attribute_name.name("attribute_name");
			attribute_value.name("attribute_value");
			qi::on_error<qi::fail>(
			  start,
			  std::cout << phoenix::val("Error! Expecting ") << qi::_4 // what failed?
						<< std::endl);
		}
	};
} // namespace learning
