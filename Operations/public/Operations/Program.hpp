/**
 * @file   Operations/Program.hpp
 * @date   Started Wed 02 Mar 2011 12:02:06 PM EST
 * @author Lester J. Dowling
 */
#pragma once
#include "Program-Base.hpp"
#include <vector>
#include <string>
#include <limits>
#include "Operations/Calculator.hpp"
#include "Pseudo-XPath/Grade.hpp"
#include "Simple-XML/Element.hpp"
#include "Simple-XML/Element-Visitor.hpp"
#include "Simple-XML/Document.hpp"
namespace operations {
	using std::vector;
	using std::string;
	using std::ostream;
	using std::numeric_limits;

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
		string gDefaultWorksheet{ "1" };

		/**
		 * The column which contains the titles for each row.
		 */
		string gRowTitlesColumn{ "1" };

		/**
		 * The row which contains the titles for each column.
		 */
		int gColumnTitlesRow = 1;

		/**
		 * How many rows do the column titles occupy.  Usually just one, though
		 * sometimes lengthy column titles span two or three rows.
		 */
		int gColumnTitleRowCount = 1;

		/**
		 * Write out all valid cell refs, nothing else.  Usually, this option is
		 * specified to check what valid cell refs can be given in a calc
		 * expression.
		 */
		bool gWriteCellRefs = false;

		/**
		 * Write out all fields of every Element.
		 */
		bool gWriteAllFields = false;

		/**
		 * Do not look for column titles in the workbook.  Usually, this option is specified
		 * when the rows in a worksheet only contain data without a title.
		 */
		bool gNoColumnTitles = false;

		/**
		 * Do not look for row titles in the workbook.  This option saves time when the
		 * worksheet is only a list of data without titles for each row.
		 */
		bool gNoRowTitles = false;

		/**
		 * The pseudo XPath as given on the command line.
		 */
		string gXPathText;

		/**
		 * An arithmetic expression given on the command line intended to be
		 * parsed by the Calculator for each extracted data value.
		 */
		string gEachCalc;

		struct Each_Calc_Constraint {
			bool _is_valid{ false };
			bool _no_constraint{ false };
			std::string _name;
			char _operator{ 0 }; // can be: = < >
			std::string _value;
			int _number{ 0 };
			bool _good_number{ false };

			void clear()
			{
				_is_valid = false;
				_no_constraint = false;
				_name.clear();
				_operator = 0;
				_value.clear();
				_number = 0;
				_good_number = false;
			}
		};

		Each_Calc_Constraint m_each_calc_constraint;

		/**
		 * An calculator expression given on the command line to be evaluated
		 * once.
		 */
		string gCalcText;

		/**
		 * Filename of a text file which contains calc script to be worked out by the
		 * calculator.
		 */
		string gCalcFile;

		/**
		 * Calculator for computing arithmetic expressions.
		 */
		Calculator gCalculator;

		/**
		 * Should calculator output be formatted for LaTeX input?
		 */
		bool gLaTeXFormatCalc{ false };

		/**
		 * Precision of calculator outputs.
		 */
		int gPrecision{ -1 };

	public: //~ Ctors ---------------------------------------------------------
		Program(int argc, char** argv, ostream& out, ostream& err);

	public: //~ Accessors -----------------------------------------------------
		std::streamsize precision() const
		{
			// Set max precision if gPrecision is negative:
			return (gPrecision < 0) ? numeric_limits<double>::digits10 + 1 : gPrecision;
		}

		string xpath_prefix(string worksheet_name);

		string xpath_prefix(const int worksheet_number);

		string xpath_prefix();

	public: //~ Overrides -----------------------------------------------------
		void setup_option_descriptions() override;
		void perform_requested_operation() override;
		// void save() override{};

	public: //~ XML operations ------------------------------------------------
		/**
		 * Load and parse the given XML file.
		 */
		void load_xml_file(const boost::filesystem::path xml_path);

	public: //~ XPath operations ----------------------------------------------
		using Grade = pseudo_xpath::Grade;

		/**
		 * Parse the given pseudo XPath expression.
		 *
		 * @return The root node of the parsed pseudo XPath.
		 */
		Grade::SP parse_xpath_text(const string xpath_text);

		/**
		 * Find and return a single node's text as specified by the precise pseudo
		 * XPath.
		 */
		string extract_single_text(Grade::SP xpath_root);

		/**
		 * Find and return a single node's text converted to a double.  The exact
		 * XML node is specified precisely by the pseudo XPath.
		 */
		double extract_single_number(Grade::SP xpath_root);

		/**
		 * Write to std output every worksheet name in the workbook.
		 */
		void write_worksheet_titles();

		/**
		 * Write to std output the column titles in every worksheet.
		 */
		void write_column_titles();

		/**
		 * Write to std output all the row titles in every worksheet.
		 */
		void write_row_titles();

		/**
		 * Write to std output the Cartesian product of all possible cell refs.
		 */
		void write_cell_refs();

	private: //~ -----------------------------------------------------
		/**
		 * The parsed XML document represented in memory as a tree of nodes.
		 */
		vector<simple_xml::Document> m_documents;

		int m_visited_row = 0;
		int m_visited_col = 0;

		void compute_xpath_and_write_results();
		void compute_calc();
		void compute_calc_file();
		void write_calc_results();
		bool constrain_each_calc(int current_row_idx, int current_col_idx);

		/**
		 * Visit all XML nodes and print their text only.  No attributes are
		 * printed.
		 */
		bool write_text_visit(simple_xml::Element_Visitor&);
	};
} // namespace operations
