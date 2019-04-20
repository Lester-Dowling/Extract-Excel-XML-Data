// Pseudo-XPath/Grade-Creator.cpp
// Started 29 Mar 2019
#include "pch-xpath-parser.hpp"
#include "Pseudo-XPath/Grade-Creator.hpp"
using std::string;

#define TRACE_CREATOR
#undef TRACE_CREATOR

#ifdef NDEBUG
#undef TRACE_CREATOR
#endif

namespace pseudo_xpath {

	Grade_Creator::Grade_Creator(Grade::SP root)
	  : current_grade{ root }
	{
		root->xml_node_name = "root";
	}

	void Grade_Creator::new_element(std::vector<char> x)
	{
		if (!current_grade)
			throw std::runtime_error{ "Current XPath grade is null." };
		current_grade->next_grade = Grade::SP{ new Grade };
		current_grade = current_grade->next_grade;
		current_grade->xml_node_name.assign(x.begin(), x.end());
		current_grade_path.push_back(current_grade);
#ifdef TRACE_CREATOR
		std::cout << ">>> " << __FUNCTION__ << ": " << current_grade->name << std::endl;
#endif
	}

	inline bool is_number_char(const char ch)
	{
		return ch == '-' || ch == '.' || std::isdigit(ch);
	}

	inline bool is_only_number_chars(const string text)
	{
		return std::all_of(text.begin(), text.end(), &is_number_char);
	}

	void Grade_Creator::add_filter(
	  const string n,
	  const char op,
	  const string v,
	  const bool q,
	  const bool d)
	{
		bool good_filter_number = false;
		int filter_number = 0;
		// if (!q && is_only_number_chars(v))
		if (d)
			try {
				filter_number = std::stoi(v);
				good_filter_number = true;
			} catch (std::invalid_argument const&) {
				good_filter_number = false;
			}
		current_grade->attribute_filters.emplace_back(
		  Attribute_Filter{ (n.empty() ? current_grade->name() : n),
							op,
							v,
							filter_number,
							good_filter_number });
	}
} // namespace pseudo_xpath
