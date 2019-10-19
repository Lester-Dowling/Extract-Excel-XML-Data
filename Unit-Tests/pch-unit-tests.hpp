#pragma once
#ifdef _WINDOWS
#include <WinSDKVer.h>
#include <SDKDDKVer.h>
// include <windows.h>
#endif // _WINDOWS
#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <numeric>
#include <string>
#include <deque>
#include <map>
#include <queue>
#include <stack>
#include <list>
#include <vector>
#include <stdexcept>

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
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct_inline.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_struct_inline.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/regex.hpp>
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
#include <boost/tokenizer.hpp>


namespace std {
	template<typename T>
	ostream& operator<<(ostream& o, const std::stack<T>& s)
	{
		std::stack<T> y{ s };
		bool first_e = true;
		while (!y.empty()) {
			const auto e = y.top();
			y.pop();
			if (first_e) {
				o << e;
				first_e = false;
			}
			else
				o << ',' << ' ' << e;
		}
		return o;
	}

	template<typename T>
	ostream& operator<<(ostream& o, const std::deque<T>& s)
	{
		bool first_e = true;
		for (const auto& e : s) {
			if (first_e) {
				o << e;
				first_e = false;
			}
			else
				o << ',' << ' ' << e;
		}
		return o;
	}

	template<class U, class V>
	ostream& operator<<(ostream& o, const pair<U, V>& p)
	{
		return o << '(' << p.first << ", " << p.second << ')';
	}

	template<typename T>
	ostream& operator<<(ostream& o, const std::vector<T>& s)
	{
		bool first_e = true;
		for (const auto& e : s) {
			if (first_e) {
				o << e;
				first_e = false;
			}
			else
				o << ',' << ' ' << e;
		}
		return o;
	}
} // namespace std
