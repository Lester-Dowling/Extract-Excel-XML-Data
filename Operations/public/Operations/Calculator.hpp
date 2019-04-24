/**
 * @file        Operations/Calculator.hpp
 * @date        Started Thu Jan 10 15:47:21 EST 2008
 * @author      Lester J. Dowling
 * @par         Copyright &copy; Lester J. Dowling.
 *
 * @brief A calculator class that takes an arithmetic expression and computes its
 * value.
 */
#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <variant>
#include <utility>
#include <memory>
namespace operations {
	/**
	 * @class Calculator
	 *
	 * An evaluator of arithmetic expressions which computes its numerical value.
	 *
	 * The evaluator is implemented with a recursive algorithm.  The intent for
	 * this class is to evaluate small-scale adhoc user input.
	 */
	class Calculator {
		std::istream* _input = nullptr;
		std::unique_ptr<std::istringstream> _sinput;

		typedef std::map<std::string, double> sym_table_t;
		sym_table_t _sym_table;
		std::string _sym;
		static const size_t kSymSizeLimit = 256; // Limit symbol names to this many chars.
		double _number_value = 0;
		bool _assignment = false;
		typedef std::map<std::string, std::string> sym_text_table_t;
		sym_text_table_t _sym_text_table;
		std::string _date_format{ "%d/%m/%Y" };

		/**
		 * The type of items stored in the history list.
		 */
	public:
		using Assignment_Pair = std::pair<std::string, double>;
		using History_Item = std::variant<double, Assignment_Pair>;

	private:
		std::list<History_Item> _history;

		enum token_value {
			NAME,
			NUMBER,
			END,
			PLUS = '+',
			MINUS = '-',
			MUL = '*',
			DIV = '/',
			POW = '^',
			NOPRINT = ';',
			PRINT = '\n',
			ASSIGN = '=',
			LP = '(',
			RP = ')',
			COMMA = ','
		};

		token_value _curr_tok{};

		/**
		 * Private method to parse addition and subtraction expressions.
		 */
		double expr();

		/**
		 * Private method to parse multiplication and division expressions.
		 */
		double term();

		/**
		 * Private method to parse power expressions.  Note that power expressions
		 * are right-associative.  For example "4 ^ 2 ^ -1" means "4 ^ 0.5" which
		 * is the square root of 4 and equal to 2.  In particular, it does @e not
		 * mean "8 ^ -1" which would evaluate to 1/8.
		 */
		double power();

		/**
		 * Private method to parse the primaries: numbers, names, unary negation,
		 * parentheses or end.
		 */
		double prim();

		/**
		 * Private method to scan the input for the next token.
		 */
		token_value get_token();

	public: //~ Ctors -------------------------------------------------------
		/**
		 * Construct a new @c Calculator object which is ready to evaluate
		 * arithmetic expressions.
		 */
		Calculator();

	public: //~ Accessors ---------------------------------------------------
		/**
		 * Set the value of the specified symbol in the symbol table.
		 */
		void set_symbol(std::string const& symbol, double value);

		/**
		 * Set the value of the specified symbol in the text symbol table.
		 */
		void set_symbol_text(std::string const& symbol, std::string const& value);

		/**
		 * Return the value of the specified symbol in the symbol table.
		 */
		double get_symbol(std::string const& symbol);

		/**
		 * Return the list of operations computed by the most recent call to @c evaluate.
		 */
		std::list<History_Item> const& history() const { return _history; }

	public: //~ Methods
		/**
		 * Evaluate each expression in the specified string and return the result
		 * of the last expression.
		 *
		 * For example, given the following arithmetic expression:
		 *
		 * <code><pre>
		 *     k = 1; k = k + 1; k * 2
		 * </pre></code>
		 *
		 * then, the result that is returned is: 4.
		 *
		 * @param[in] expression__ An arithmetic expression to be evaluated.
		 *
		 * @return The result of evaluating the final arithmetic expression.
		 */
		double evaluate(std::string const& expression);
	};
} // namespace operations
