/**
 * @file   Operations/Program.hpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#pragma once
#include "Program-Base.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <limits>
#include "Operations/Calculator.hpp"
#include "Simple-XML-Parser/Worksheet-Row-Column-Titles.hpp"
#include "Simple-XML-Parser/Node.hpp"
#include "Simple-XML-Parser/Node-Visitor.hpp"
#include "Pseudo-XPath-Parser/Grade.hpp"
namespace operations {
	using std::make_shared;
	using std::shared_ptr;
	using excel_xml_parser::Worksheet_Row_Column_Titles;

	class Program : public Program_Base {
	public:
		/**
		 * Prepend the row number to each line of output.
		 */
		bool gEnumerateRows = false;

		/**
		 * Write out only the worksheet titles, nothing else.  Usually, this
		 * option is specified to check what worksheets are in the workbook.
		 */
		bool gWriteWorksheetTitles = false;

		/**
		 * Write out only the column titles, nothing else.  Usually, this option
		 * is specified to check what the worksheet contains.
		 */
		bool gWriteColumnTitles = false;

		/**
		 * Write out only the row titles, nothing else.  Usually, this option is
		 * specified to check what the worksheet contains.
		 */
		bool gWriteRowTitles = false;

		/**
		 * The worksheet to extract from if no worksheet ref was given.
		 */
		std::string gDefaultWorksheet{ "1" };

		/**
		 * The column which contains the titles for each row.
		 */
		std::string gRowTitlesColumn;

		/**
		 * The row which contains the titles for each column.
		 */
		int gColumnTitlesRow;

		/**
		 * How many rows do the column titles occupy.  Usually just one, though
		 * sometimes lengthy column titles span two or three rows.
		 */
		size_t gColumnTitleSpan = 1;

		/**
		 * Write out all valid cell refs, nothing else.  Usually, this option is
		 * specified to check what valid cell refs can be given in a calc
		 * expression.
		 */
		bool gWriteCellRefs = false;

		/**
		 * The pseudo XPath as given on the command line.
		 */
		std::string gXPathText;

		/**
		 * An arithmetic expression given on the command line intended to be
		 * parsed by the Calculator for each extracted data value.
		 */
		std::string gEachArithmeticExpression;

		/**
		 * An calculator expression given on the command line to be evaluated
		 * once.
		 */
		std::string gCalcText;

		/**
		 * Filename of a text file which contains calc script to be worked out by the
		 * calculator.
		 */
		std::string gCalcFile;

		/**
		 * Calculator for computing arithmetic expressions.
		 */
		Calculator gCalculator;

	public: //~ Ctors ---------------------------------------------------------
		Program(int argc, char** argv, std::ostream& out, std::ostream& err)
		  : Program_Base{ argc, argv, out, err }
		{
			parse_command_line_options();
		}

	public: //~ Accessors -----------------------------------------------------
		std::streamsize precision() const
		{
			// Set max precision if gPrecision is negative:
			return (gPrecision < 0) ? std::numeric_limits<double>::digits10 + 1
									: gPrecision;
		}

		std::string xpath_prefix(std::string worksheet_name);

		std::string xpath_prefix(const int worksheet_number);

		std::string xpath_prefix();

	public: //~ Overrides -----------------------------------------------------
		void setup_option_descriptions() override;
		void perform_requested_operation() override;
		// void save() override{};

	public: //~ XML operations ------------------------------------------------
		typedef std::vector<char> file_in_memory_t;
		typedef file_in_memory_t::const_iterator Memory_Iterator;
		using Node = excel_xml_parser::Node;

		/**
		 * Load and parse the given XML file.
		 *
		 * @return The root node of the parsed XML file.
		 */
		Node::SP load_xml_file(const boost::filesystem::path xml_path);

	public: //~ XPath operations ----------------------------------------------
		using Grade = pseudo_xpath_parser::Grade;

		/**
		 * Parse the given pseudo XPath expression.
		 *
		 * @return The root node of the parsed pseudo XPath.
		 */
		Grade::SP parse_xpath_text(const std::string xpath_text, const int wkt_idx = 1);

		/**
		 * Find and return a single node's text as specified by the precise pseudo
		 * XPath.
		 */
		std::string extract_single_text(Node::SP xml_root, Grade::SP xpath_root);

		/**
		 * Find and return a single node's text converted to a double.  The exact
		 * XML node is specified precisely by the pseudo XPath.
		 */
		double extract_single_number(Node::SP xml_root, Grade::SP xpath_root);

	private: //~ -----------------------------------------------------
		/**
		 * Precision of calculator outputs.
		 */
		int gPrecision{ -1 };

		/**
		 * Titles of all worksheets, rows and columns within the workbook.
		 */
		shared_ptr<Worksheet_Row_Column_Titles> m_titles{
			make_shared<Worksheet_Row_Column_Titles>()
		};

		int m_visited_row = 0;
		int m_visited_col = 0;

		void compute_xpath_and_write_results(Node::SP xml_root);
		void compute_calc_and_write_results(Node::SP xml_root);

		/**
		 * Find and put into @c m_titles all the worksheet names in the workbook.
		 */
		void extract_worksheet_titles(Node::SP xml_root);

		/**
		 * Write to std output every worksheet name in the workbook.
		 */
		void write_worksheet_titles();

		/**
		 * Find and put into m_titles all the column titles in the workbook.
		 */
		void extract_column_titles(Node::SP);

		/**
		 * Write to std output the column titles in every worksheet.
		 */
		void write_column_titles();

		/**
		 * Find and put into m_titles all the row titles in the workbook.
		 */
		void extract_row_titles(Node::SP);

		/**
		 * Write to std output all the row titles in every worksheet.
		 */
		void write_row_titles();

		/**
		 * Write to std output the Cartesian product of all possible cell refs.
		 */
		void write_cell_refs();

		/**
		 * Visit all XML nodes and print all fields of the nodes.
		 */
		bool write_all_fields_visit(excel_xml_parser::Node_Visitor&);

		/**
		 * Visit all XML nodes and print their text, only.  No attributes are
		 * printed.
		 */
		bool write_text_visit(excel_xml_parser::Node_Visitor&);

		/**
		 * Extract a single data cell, usually specified exactly by a precise
		 * XPath.
		 */
		std::string m_one_data;
		bool one_data_visit(excel_xml_parser::Node_Visitor&);
	};
} // namespace operations
