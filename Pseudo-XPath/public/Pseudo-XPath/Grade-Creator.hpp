// Pseudo-XPath/Grade-Creator.hpp
// Started 28 Mar 2019
#pragma once
#include <memory>
#include <string>
#include <deque>
#include <vector>
#include "Pseudo-XPath/Grade.hpp"
namespace pseudo_xpath {

	class Grade_Creator
	{
	  protected:
		Grade::SP current_grade;

		typedef std::deque<Grade::SP> Grade_Path_Type;
		typedef Grade_Path_Type::const_iterator Grade_Path_Iterator;
		Grade_Path_Type current_grade_path;

		bool is_current_grade_root() const { return current_grade_path.empty(); }

	  public:
		typedef std::shared_ptr<Grade_Creator> SP;

		Grade_Creator(Grade::SP first_grade);

		void new_element(std::vector<char> x);

		/**
		 * Append a new filter to the current grade.
		 *
		 * @param[in] n The name of the attribute to filter on.
		 *
		 * @param[in] op The filter operation: =, < or >.
		 *
		 * @param[in] v The filter value to match.
		 *
		 * @param[in] q Was the filter value quoted in the original XPath text?
		 *
		 * @param[in] d Is the filter value only digits?
		 */
		void add_filter(
		  const std::string n,
		  const char op,
		  const std::string v,
		  const bool q,
		  const bool d);

		size_t depth() const { return current_grade_path.size(); }
	};
} // namespace pseudo_xpath
