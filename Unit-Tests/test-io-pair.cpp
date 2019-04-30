// Unit-Tests/test-io-pair.cpp
#include "pch-unit-tests.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#include "IO-Extra/pair.hpp"
#include "IO-Extra/sequence.hpp"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_pair_io_suite, *utf::enabled())

/*
 * Unit tests to verify the correct operation of pair I/O.
 */

typedef boost::mpl::list<int16_t, int32_t, float, double> signed_test_types;
typedef boost::mpl::list<uint16_t, uint32_t> unsigned_test_types;
typedef boost::mpl::list<int16_t, uint16_t, int32_t, uint32_t, float, double> test_types;
typedef boost::mpl::list<char, unsigned char, signed char> char_test_types;
typedef boost::mpl::list<float, double> real_test_types;

BOOST_AUTO_TEST_CASE(test_std_parser) // What does std do when parsing a list of doubles?
{
	using namespace std;
	istringstream doubles{ "   9.8   1.9   0.3   8.1  78.99     " };
	doubles.exceptions(ios_base::badbit | ios_base::failbit);
	vector<double> vd;
	vd.reserve(10);
	try {
		vd.insert(
		  vd.end(), istream_iterator<double>{ doubles }, std::istream_iterator<double>{});
	}
	catch (const std::ios_base::failure&) {
		if (doubles.fail()) {
			// BOOST_TEST_MESSAGE("doubles has failbit set");
		}
		if (doubles.bad()) {
			// BOOST_TEST_MESSAGE("doubles has badbit set");
			BOOST_FAIL("Failed to parse doubles");
		}
		if (doubles.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("doubles has eofbit set");
			BOOST_TEST(vd.size() == 5);
			return;
		}
		else
			BOOST_FAIL("Failed to parse doubles");
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_IO_of_string_in_pair, T, test_types)
{
	using namespace std;
	pair<string, T> p{ "George's \"Age\"", static_cast<T>(8) };
	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	oss << p;
	BOOST_REQUIRE_EQUAL(oss.str(), "(\"George's \\\"Age\\\"\", 8)");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_IO_of_signed_numbers, T, signed_test_types)
{
	using namespace std;
	typedef pair<T, T> Pair;
	istringstream iss{ "  (-9,8)   (1,9)   (0,3)   (-8,1)   (78,99) " };
	iss.exceptions(ios_base::badbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), istream_iterator<Pair>{ iss }, istream_iterator<Pair>{});
	}
	catch (const ios_base::failure& e__) {
		// The exception msg when EOF:
		const string eof_description("basic_ios::clear"), exception_msg(e__.what());
		// If this exception is not the result of EOF, then re-throw it:
		if (exception_msg.find(eof_description) == string::npos)
			throw;
		// Else, the exception was thrown as a result of EOF which is normal. Okay to
		// continue.
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", 0, -1, " ", "");
	BOOST_REQUIRE_EQUAL(oss.str(), "v = (-9, 8) (1, 9) (0, 3) (-8, 1) (78, 99) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_IO_of_unsigned_numbers, T, unsigned_test_types)
{
	using namespace std;
	typedef pair<T, T> Pair;
	istringstream iss("   (9,8)   (1,9)   (0,3)   (8,1)  (78,99)     ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;
	v.reserve(10);
	try {
		v.insert(v.end(), istream_iterator<Pair>{ iss }, std::istream_iterator<Pair>{});
	}
	catch (const std::ios_base::failure&) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 5);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", 0, -1, " ", "");
	BOOST_REQUIRE_EQUAL(oss.str(), "v = (9, 8) (1, 9) (0, 3) (8, 1) (78, 99) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_IO_of_char, T, char_test_types)
{
	using namespace std;
	typedef pair<T, T> Pair;
	istringstream iss("(a,b) ( A , B ) (c,d) (e,f) (g,h)");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
	}
	catch (const std::ios_base::failure&) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 5);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", 0, -1, " ", "");
	BOOST_REQUIRE_EQUAL(oss.str(), "v = (a, b) (A, B) (c, d) (e, f) (g, h) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_IO_of_real_numbers, T, real_test_types)
{
	using namespace std;
	typedef pair<T, T> Pair;
	istringstream iss("   ( 3.55 , 373.23)    ( 2.4 , 1.5)    ( 5.9 , -1e10 )    ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
		// ljd::io::write_sequence( cout, v, "v", 0 );
	}
	catch (const std::ios_base::failure&) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 3);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", /*width*/ 0, /*prec*/ -1, " ", "");
	BOOST_REQUIRE_EQUAL(oss.str(), "v = (3.55, 373.23) (2.4, 1.5) (5.9, -1e+10) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(
  test_IO_of_one_word_string_and_real_numbers,
  T,
  real_test_types)
{
	using namespace std;
	typedef pair<string, T> Pair;
	istringstream iss("   ( abc , 373.23)    ( def , 1.5)    ( ghi , -1e10 )    ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
	}
	catch (const std::ios_base::failure& ) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 3);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", /*width*/ 0, /*prec*/ -1, " ", "");
	BOOST_REQUIRE_EQUAL(
	  oss.str(), "v = (\"abc\", 373.23) (\"def\", 1.5) (\"ghi\", -1e+10) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(
  test_IO_of_multi_word_string_and_real_numbers,
  T,
  real_test_types)
{
	using namespace std;
	typedef pair<string, T> Pair;
	istringstream iss(
	  "   ( 'abc \"def' , 373.23)    ( \\\"def\\\" , 1.5)    ( ghi , -1e10 )    ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
	}
	catch (const std::ios_base::failure& ) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 3);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", /*width*/ 0, /*prec*/ -1, " ", "");
	BOOST_REQUIRE_EQUAL(
	  oss.str(), "v = (\"abc \\\"def\", 373.23) (\"\\\"def\\\"\", 1.5) (\"ghi\", -1e+10) ");
}


BOOST_AUTO_TEST_CASE_TEMPLATE(
  test_IO_of_real_numbers_and_multi_word_string,
  T,
  real_test_types)
{
	using namespace std;
	typedef pair<T, string> Pair;
	istringstream iss(
	  "   ( 373.23 , 'abc \"123' )    ( 1.5 , \\\"def\\\"  )    ( -1e10 , ghi )   ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;

	// Read iss into v:
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
	}
	catch (const std::ios_base::failure& ) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 3);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit);
	io::write_sequence(oss, v, "v", /*width*/ 0, /*prec*/ -1, " ", "");
	BOOST_REQUIRE_EQUAL(
	  oss.str(), "v = (373.23, \"abc \\\"123\") (1.5, \"\\\"def\\\"\") (-1e+10, \"ghi\") ");
}


BOOST_AUTO_TEST_CASE(test_IO_of_strings_and_comments)
{
	using namespace std;
	typedef pair<string, string> Pair;
	istringstream iss("   ( \"xyz\" , 'abc \"123' )    ( lmn , def )    ( /* in a comment "
					  "*/ qrs , ghi )    ");
	iss.exceptions(ios_base::badbit | ios_base::failbit);
	vector<Pair> v;
	try {
		v.insert(v.end(), std::istream_iterator<Pair>(iss), std::istream_iterator<Pair>());
	}
	catch (const std::ios_base::failure& ) {
		if (iss.fail()) {
			// BOOST_TEST_MESSAGE("iss has failbit set");
		}
		if (iss.bad()) {
			// BOOST_TEST_MESSAGE("iss has badbit set");
			BOOST_FAIL("Failed to parse pairs in istringstream");
		}
		if (iss.eof()) {
			// Okay.  This is expected: istream_iterator ends correctly by
			// setting the eofbit.
			// BOOST_TEST_MESSAGE("iss has eofbit set");
			BOOST_TEST(v.size() == 3);
		}
		else
			BOOST_FAIL("Failed to parse pairs in istringstream");
	}

	ostringstream oss;
	oss.exceptions(ios_base::badbit | ios_base::failbit | ios_base::eofbit);
	io::write_sequence(oss, v, "v", /*width*/ 0, /*prec*/ -1, " ", "");
	BOOST_REQUIRE_EQUAL(
	  oss.str(), "v = (\"xyz\", \"abc \\\"123\") (\"lmn\", \"def\") (\"qrs\", \"ghi\") ");
}

BOOST_AUTO_TEST_SUITE_END();
