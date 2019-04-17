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
namespace practice {
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	///////////////////////////////////////////////////////////////////////////
	//  Our mini XML tree representation
	///////////////////////////////////////////////////////////////////////////
	struct mini_xml;

	typedef boost::variant<boost::recursive_wrapper<mini_xml>, std::string> mini_xml_node;

	struct mini_xml_tag
	{
		std::string name;
		std::string attributes;
	};

	struct mini_xml
	{
		mini_xml_tag tag;
		std::vector<mini_xml_node> children;
	};
} // namespace practice

BOOST_FUSION_ADAPT_STRUCT(
  practice::mini_xml_tag,
  (std::string, name)(std::string, attributes))

BOOST_FUSION_ADAPT_STRUCT(
  practice::mini_xml,
  (practice::mini_xml_tag, tag)(std::vector<practice::mini_xml_node>, children))
