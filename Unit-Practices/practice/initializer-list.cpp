/**
 * @file   Unit-Practices/practice/initializer_list.cpp
 * @date   Started 2018-08-15
 * @author Lester J. Dowling
 */
#include "pch-practice.hpp"
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

/*
 * Practice and learn about the operation of initializer_lists.
 */

BOOST_AUTO_TEST_SUITE(practice_initializer_lists_suite, *utf::enabled())
namespace {
	int sum(std::initializer_list<int> numbers)
	{
		int result = 0;
		for (auto i : numbers) {
			result += i;
		}
		return result;
	}

	std::queue<std::string> set_queue_path(std::initializer_list<std::string> node_names)
	{
		std::queue<std::string> result;
		for (auto i : node_names) {
			result.push(i);
		}
		return result;
	}

	std::stack<std::string> set_stack_path(std::initializer_list<std::string> node_names)
	{
		std::stack<std::string> result;
		for (auto i : node_names) {
			result.push(i);
		}
		return result;
	}

	std::deque<std::string> set_deque_path(std::initializer_list<std::string> node_names)
	{
		std::deque<std::string> result;
		for (auto i : node_names) {
			result.push_back(i);
		}
		return result;
	}
} // namespace


BOOST_AUTO_TEST_CASE(verify_initializer_list)
{
	const int total = sum({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
	BOOST_TEST(total == 55);
}


BOOST_AUTO_TEST_CASE(verify_filter_path)
{
	std::queue<std::string> fp =
	  set_queue_path({ "Workbook", "Worksheet", "Table", "Row" });
	BOOST_TEST(std::string{ "Workbook" } == fp.front());
	fp.pop();
	BOOST_TEST(std::string{ "Worksheet" } == fp.front());
	fp.pop();
	BOOST_TEST(std::string{ "Table" } == fp.front());
	fp.pop();
	BOOST_TEST(std::string{ "Row" } == fp.front());
	fp.pop();
}


BOOST_AUTO_TEST_CASE(verify_equality_of_stacks)
{
	std::stack<std::string> s1 =
	  set_stack_path({ "Workbook", "Worksheet", "Table", "Row" });
	std::stack<std::string> s2 =
	  set_stack_path({ "Workbook", "Worksheet", "Table", "Row" });
	BOOST_TEST(s1 == s2);
}


BOOST_AUTO_TEST_CASE(verify_deque_for_path)
{
	std::deque<std::string> d1 =
	  set_deque_path({ "Workbook", "Worksheet", "Table", "Row" });
	std::deque<std::string> d2 =
	  set_deque_path({ "Workbook", "Worksheet", "Table", "Row" });
	BOOST_TEST(d1 == d2);
	BOOST_TEST(d1[0] == d2[0]);
	BOOST_TEST(d1[1] == d2[1]);
	BOOST_TEST(d1[2] == d2[2]);
	BOOST_TEST(d1[3] == d2[3]);
}

BOOST_AUTO_TEST_SUITE_END()
