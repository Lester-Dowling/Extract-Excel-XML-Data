/**
 * @file   Pseudo-XPath/forming.hpp
 * @date   Started 2019-04-28
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <sstream>
#include "String-Extra/forming.hpp"
namespace pseudo_xpath {
	using std::string;

	/**
	 * Standard pseudo XPath prefix given a worksheet name.
	 */
	inline string prefix(string worksheet_name)
	{
		// Worksheet[ss:Name="Profit Loss"]
		std::string xp = "Workbook, Worksheet[";
		string_extra::append_quoted_if_not_number(xp, worksheet_name, "Missing worksheet name");
		xp += "], Table, ";
		return xp;
	}

	/**
	 * Standard pseudo XPath prefix given a worksheet number
	 */
	inline string prefix(const int worksheet_number)
	{
		std::ostringstream xp;
		xp << "Workbook, Worksheet[";
		xp << worksheet_number;
		xp << "], Table, ";
		return xp.str();
	}


	/**
	 * Default pseudo XPath prefix for the first worksheet.
	 */
	inline string prefix() { return "Workbook, Worksheet, Table, "; }


} // namespace pseudo_xpath
