// mini-xml-tag.hpp
// Started 15 Aug 2018
#pragma once
#include <string>
#include <map>
#include <boost/fusion/include/adapt_struct.hpp>

namespace practice {
	struct mini_xml_tag
	{
		std::string name;
		std::string attributes;
		std::map<std::string, std::string> attribute_dict;
	};
} // namespace practice
BOOST_FUSION_ADAPT_STRUCT(
  practice::mini_xml_tag,
  (std::string, name)(std::string, attributes));
