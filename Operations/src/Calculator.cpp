#include "pch-operations.hpp"
#include "Operations/Calculator.hpp"

/*
 * Debugging macros for tracing function entry.
 */
#ifdef DO_TRACE_ljd_numeric_Calculator
#define TRACE_CALCULATOR_METHOD std::cout << __FUNCTION__ << std::endl
#define TRACE_CALCULATOR_METHOD_PARAM(p) std::cout << __FUNCTION__ << ':' << p << std::endl
#else
#define TRACE_CALCULATOR_METHOD
#define TRACE_CALCULATOR_METHOD_PARAM(p)
#endif // DO_TRACE_ljd_numeric_Calculator

namespace operations {

	Calculator::Calculator()
	{
		_sym.reserve(kSymSizeLimit);
		// Insert some common constants:
		_sym_table["pi"] = 3.1415926535897932385;
		_sym_table["e"] = 2.7182818284590452354;
	}

	void Calculator::set_symbol(const std::string& symbol__, double value__)
	{
		_sym_table[symbol__] = value__;
	}

	double Calculator::get_symbol(const std::string& symbol__)
	{
		return _sym_table[symbol__];
	}

	double Calculator::expr()
	{
		TRACE_CALCULATOR_METHOD;
		double left = term();

		for (;;)
			switch (_curr_tok) {
			case PLUS:
				get_token(); // eat '+'
				left += term();
				break;
			case MINUS:
				get_token(); // eat '-'
				left -= term();
				break;
			default:
				return left;
			}
	}

	double Calculator::term()
	{
		TRACE_CALCULATOR_METHOD;
		double left = power();

		for (;;)
			switch (_curr_tok) {
			case MUL:
				get_token(); // eat '*'
				left *= power();
				break;
			case DIV: {
				get_token(); // eat '/'
				double d = power();
				if (d == 0)
					throw std::runtime_error{ "Divide by 0" };
				left /= d;
				break;
			}
			default:
				return left;
			}
	}

	double Calculator::power()
	{
		TRACE_CALCULATOR_METHOD;
		double left = prim();

		for (;;)
			switch (_curr_tok) {
			case POW: {
				get_token(); // eat '^'
				double d = power();
				left = std::pow(left, d);
			}
			default:
				return left;
			}
	}

	double Calculator::prim()
	{
		TRACE_CALCULATOR_METHOD;
		switch (_curr_tok) {
		case NUMBER: // floating point constant
			get_token();
			return _number_value;
		case NAME: {
			token_value nt = get_token();
			if (nt == ASSIGN) {
				std::string s = _sym; // symbolic name to assign to
				get_token();
				const double v = expr(); // value to assign to symbolic name
				_sym_table[s] = v;
				// History_Item hi = std::make_pair(s, v);
				_history.emplace_back(std::make_pair(s, v));
				_assignment = true;
				return v;
			}
			else if (nt == LP) {	   // function call
				double result = 0;	   // function result
				std::string fn = _sym; // function name
				get_token();
				const double e1 = expr();
				if (_curr_tok == RP) {
					// Unary Functions:
					if (fn == "cos")
						result = std::cos(e1);
					else if (fn == "sin")
						result = std::sin(e1);
					else if (fn == "tan")
						result = std::tan(e1);
					else if (fn == "acos")
						result = std::acos(e1);
					else if (fn == "asin")
						result = std::asin(e1);
					else if (fn == "atan")
						result = std::atan(e1);
					// else if ( fn == "acosh" ) result = precise_math::acoshf(e);
					// else if ( fn == "asinh" ) result = precise_math::asinhf(e);
					// else if ( fn == "atanh" ) result = precise_math::atanhf(e);
					else if (fn == "cosh")
						result = std::cosh(e1);
					else if (fn == "sinh")
						result = std::sinh(e1);
					else if (fn == "tanh")
						result = std::tanh(e1);
					else if (fn == "exp")
						result = std::exp(e1);
					// else if ( fn == "exp2" ) result = precise_math::exp2f(e);
					else if (fn == "log")
						result = std::log(e1);
					else if (fn == "log10")
						result = std::log10(e1);
					// else if ( fn == "log2" ) result = precise_math::log2f(e);
					else if (fn == "abs")
						result = std::abs(e1);
					else if (fn == "sqrt")
						result = std::sqrt(e1);
					// else if ( fn == "erf" ) result = precise_math::erff(e);
					// else if ( fn == "lgamma" ) result = precise_math::lgammaf(e);
					// else if ( fn == "tgamma" ) result = precise_math::tgammaf(e);
					else if (fn == "ceil")
						result = std::ceil(e1);
					else if (fn == "floor")
						result = std::floor(e1);
					// else if ( fn == "round" ) result = precise_math::roundf(e);
					else
						throw std::runtime_error{ "Unknown unary function: " + fn };
				}
				else if (_curr_tok == COMMA) {
					get_token();
					const double e2 = expr();
					if (_curr_tok == RP) {
						// Binary Functions:
						if (fn == "deltapercent")
							result = 100.0 * ((e2 - e1) / e1);
						else
							throw std::runtime_error{ "Unknown binary function: " + fn };
					}
				}

				if (_curr_tok != RP)
					throw std::runtime_error{ ") expected" };
				get_token();
				return result;
			}
			else {
				return _sym_table[_sym];
			}
		}
		case MINUS: // unary minus
			get_token();
			return -prim();
		case LP: {
			get_token();
			double e = expr();
			if (_curr_tok != RP)
				throw std::runtime_error{ ") expected" };
			get_token();
			return e;
		}
		case END:
			return 1;
		default:
			throw std::runtime_error{ "A number, name or parenthesis was expected" };
		}
	}

	Calculator::token_value Calculator::get_token()
	{
		TRACE_CALCULATOR_METHOD;
		char ch;

		do { // skip whitespace except '\n'
			if (!_input->get(ch))
				return _curr_tok = END;
		} while (ch != '\n' && isspace(ch));

		switch (ch) {
		case ';':
			return _curr_tok = NOPRINT;
		case '\n':
			return _curr_tok = PRINT;
		case '*':
		case '/':
		case '+':
		case '-':
		case '(':
		case ')':
		case '=':
		case '^':
		case ',':
			return _curr_tok = token_value(ch);
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			_input->putback(ch);
			*_input >> _number_value;
			return _curr_tok = NUMBER;
		default: // NAME, NAME=, or error
			if (isalpha(ch)) {
				int lim = kSymSizeLimit;
				_sym.clear();
				_sym += ch;
				while (_input->get(ch) && isalnum(ch) && 0 < --lim)
					_sym += ch;
				_input->putback(ch);
				if (lim <= 0)
					throw std::runtime_error{ "Symbol name too long" };
				return _curr_tok = NAME;
			}
			throw std::runtime_error{ "Bad character for arithmetic expression" };
		}
	}

	double Calculator::evaluate(const std::string& expression__)
	{
		TRACE_CALCULATOR_METHOD_PARAM(expression__);
		_history.clear();
		_sinput.reset(new std::istringstream(expression__));
		_input = _sinput.get();
		double result = 0;

		while (*_input) {
			get_token();
			if (_curr_tok == END)
				break;
			if (_curr_tok == NOPRINT) {
				_history.pop_back();
				continue;
			}
			if (_curr_tok == PRINT) 
				continue;
			result = expr();
			if (!_assignment)
				_history.emplace_back(result);
			_assignment = false;
		}
		return result;
	}

} // namespace operations
