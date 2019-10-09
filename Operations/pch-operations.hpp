#pragma once
#ifdef _WINDOWS
#include <WinSDKVer.h>
#include <SDKDDKVer.h>
// include <windows.h>
#endif // _WINDOWS
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <cctype>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <initializer_list>
#include <vector>
#include <list>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <limits>
#include <optional>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/regex.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_match.hpp>
#include <boost/spirit/include/qi_match_attr.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>
#include <boost/tokenizer.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/locale/encoding_utf.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/format_date_parser.hpp>
#include <boost/date_time/special_values_parser.hpp>

namespace f = boost::filesystem;
namespace po = boost::program_options;
typedef std::vector<std::string> VectorString;
typedef std::shared_ptr<VectorString> VectorStringSP;
typedef std::vector<VectorStringSP> VectorVectorString;
typedef std::shared_ptr<VectorVectorString> VectorVectorStringSP;
typedef std::map<std::string, VectorStringSP> MapStringToVectorString;
typedef std::vector<std::wstring> VectorWString;

inline bool starts_with(std::string_view big, std::string_view x)
{
	return big.size() >= x.size() && big.compare(0, x.size(), x) == 0;
}
