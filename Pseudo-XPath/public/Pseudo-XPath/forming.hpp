/**
 * @file   Pseudo-XPath/forming.hpp
 * @date   Started 2019-04-28
 * @author Lester J. Dowling
 */
#pragma once
#include <string>
#include <sstream>
#include "Strings-Extra/forming.hpp"
namespace pseudo_xpath {
	using std::string;

	inline string prefix(string worksheet_name)
	{
		// Worksheet[ss:Name="Profit Loss"]
		std::string xp = "Workbook, Worksheet[";
		strings::append_quoted_if_not_number(
		  xp, worksheet_name, "Missing worksheet name");
		xp += "], Table, ";
		return xp;
	}


	inline string prefix(const int worksheet_number)
	{
		std::ostringstream xp;
		xp << "Workbook, Worksheet[";
		xp << worksheet_number;
		xp << "], Table, ";
		return xp.str();
	}


	inline string prefix() { return "Workbook, Worksheet, Table, "; }


} // namespace pseudo_xpath
