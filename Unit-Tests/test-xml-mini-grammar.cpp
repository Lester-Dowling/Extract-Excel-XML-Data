/**
 * @file   Unit-Tests/test-xml-mini-grammar.cpp
 * @date   Started 2019-04-19
 * @author Lester J. Dowling
 */
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Creator.hpp"
#include "Simple-XML/mini-grammar.hpp"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_xml_mini_grammar_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of simple_xml::Element.
 */

namespace a = boost::algorithm;
namespace ascii = boost::spirit::ascii;
namespace f = boost::filesystem;
namespace qi = boost::spirit::qi;
namespace tt = boost::test_tools;
using std::runtime_error;
using std::string;
using std::vector;
using file_in_memory_t = std::vector<char>;
using Memory_Iterator = file_in_memory_t::const_iterator;
using Stream_Iterator = boost::spirit::basic_istream_iterator<char>;
using String_Iterator = string::const_iterator;
using XML_Grammar = simple_xml::mini_grammar<Memory_Iterator>;
using simple_xml::Element;
using simple_xml::Element_Creator;

namespace {

	const f::path samples_dir{
		"C:/Users/ljdowling/Projects/Extract-Excel-XML-Data-2017/sample-xml"
	};

	void load_xml_file(const f::path xml_path, Element_Creator* element_creator)
	{
		if (!f::exists(xml_path))
			throw runtime_error{ "No such file: " + xml_path.string() };
		f::ifstream xml_stream{ xml_path };
		if (!xml_stream)
			throw runtime_error{ "Could not open file: " + xml_path.string() };
		boost::system::error_code ec;
		const boost::uintmax_t xml_file_size = f::file_size(xml_path, ec);
		if (ec || xml_file_size == static_cast<boost::uintmax_t>(-1))
			throw runtime_error{ "Failed to get XML file size." };

		file_in_memory_t file_in_memory;
		file_in_memory.reserve(xml_file_size);
		xml_stream.unsetf(std::ios::skipws); // No white space skipping!
		std::copy(
		  std::istream_iterator<char>(xml_stream),
		  std::istream_iterator<char>(),
		  std::back_inserter(file_in_memory));

		Memory_Iterator fitr = file_in_memory.begin();
		Memory_Iterator const fend = file_in_memory.end();
		XML_Grammar xml_parser{ element_creator };
		if (!phrase_parse(fitr, fend, xml_parser, ascii::space))
			throw runtime_error{ "Parsing failed." };
	}
} // namespace


BOOST_AUTO_TEST_CASE(single_value_worksheet)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	load_xml_file(samples_dir / "single-value-worksheet.xml", &creator);
	BOOST_TEST(elements.size() == 40);
	BOOST_TEST(elements.front().name() == "Workbook");
	for (auto const& e : elements) {
		if (e.name() == "Data")
			BOOST_TEST(e.text() == "3.14");
	}
}


BOOST_AUTO_TEST_CASE(single_column_of_values_worksheet)
{
	vector<Element> elements;
	Element_Creator creator{ elements };
	load_xml_file(samples_dir / "single-column-of-values-worksheet.xml", &creator);
	BOOST_TEST(elements.size() == 52);
	BOOST_TEST(elements.front().name() == "Workbook");
	const vector<string> expected_values = { "2", "4", "6", "8", "10" };
	vector<string> parsed_values;
	for (auto const& e : elements) {
		if (e.name() == "Data")
			parsed_values.push_back(e.text());
	}
	BOOST_TEST(parsed_values == expected_values);
}

BOOST_AUTO_TEST_SUITE_END()
