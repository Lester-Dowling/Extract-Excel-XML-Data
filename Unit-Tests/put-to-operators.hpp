#pragma once
#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <string>
#include <deque>
#include <map>
#include <queue>
#include <stack>
#include <list>
#include <vector>
#include <stdexcept>


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
