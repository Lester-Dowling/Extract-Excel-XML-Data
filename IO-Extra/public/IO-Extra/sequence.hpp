/**
 * @file        IO-Extra/sequence.hpp
 * @date        Started Sun Jul  9 10:40:51 EST 2006
 * @author      Lester J. Dowling
 * @par         Copyright &copy; Lester J. Dowling.
 */
#pragma once
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "IO-Extra/ostream_ww_iterator.hpp"

namespace io {

	/**
	 * Write the elements between the given begin and end iterators to the
	 * specified output stream.
	 *
	 * @param[in] os__ The output stream.
	 *
	 * @param[in] begin__ An iterator at the beginning of the sequence.
	 *
	 * @param[in] end__ A iterator at the end of the sequence.
	 *
	 * @param[in] name__ An optional name to prefix to the sequence.  A typical
	 * usage of this option is to prefix the sequence with the sequence name.
	 * When the sequence is read back again, the prefixed name of the sequence can
	 * be asserted to match the name of the sequence variable.
	 *
	 * @param[in] width__ Fixed width specifier of each output value of the
	 * sequence.
	 *
	 * @param[in] prec__ The precision specification for floating point output.
	 *
	 * @param[in] sep__ A string that separates each output value of the sequence.
	 *
	 * @param[in] eol__ The string to write at the end of the written sequence.
	 *
	 * An example output looks like this:
	 *
	 * <code><pre>
	 *     list_of_samples = 1 2 3 4 5
	 * </pre></code>
	 */
	template<class Input_Iterator>
	void write_sequence(
	  std::ostream& os__,
	  Input_Iterator begin__,
	  Input_Iterator end__,
	  const std::string& name__ = "",
	  size_t width__ = 0,
	  int prec__ = -1,
	  const std::string& sep__ = "   ",
	  const std::string& eol__ = "\n")
	{
		using namespace std;
		typedef typename std::iterator_traits<Input_Iterator>::value_type Value_Type;

		// Prefix the sequence with its name:
		if (name__ != "") {
			os__ << name__;
			if (sep__ != "\n")
				os__ << " = ";
			else
				os__ << endl;
		}

		const size_t prev_precision = os__.precision();
		os__.precision(prec__);
		const char* const sep = sep__.c_str();
		if (width__ == 0)
			std::copy(begin__, end__, ostream_iterator<Value_Type>(os__, sep));
		else
			std::copy(begin__, end__, ostream_ww_iterator<Value_Type>(os__, width__, sep));
		if (eol__ == "\n")
			os__ << endl;
		else
			os__ << eol__;
		os__.precision(prev_precision);
	}

	/**
	 * A convenience option for @c write_sequence that writes all of the specified
	 * sequence to the specified file.
	 *
	 * Use this function call to avoid spelling out the @c begin and @c end of a
	 * sequence to @c write_sequence.
	 *
	 * @param[in] os__ The output stream.
	 *
	 * @param[in] seq__ The sequence to output and which has @c begin and @c end
	 * member functions.
	 *
	 * @param[in] name__ An optional name to prefix to the sequence.  A typical
	 * usage of this option is to prefix the sequence with the sequence name.
	 * When the sequence is read back again, the prefixed name of the sequence can
	 * be asserted to match the name of the sequence variable.
	 *
	 * @param[in] width__ Fixed width specifier of each output value of the
	 * sequence.
	 *
	 * @param[in] prec__ The precision specification for floating point output.
	 *
	 * @param[in] sep__ A string that separates each output value of the sequence.
	 *
	 * @param[in] eol__ The string to write at the end of the written sequence.
	 */
	template<class Sequence_Type>
	void write_sequence(
	  std::ostream& os__,
	  const Sequence_Type& seq__,
	  const std::string& name__ = "",
	  size_t width__ = 0,
	  int prec__ = -1,
	  const std::string& sep__ = "   ",
	  const std::string& eol__ = "\n")
	{
		write_sequence<typename Sequence_Type::const_iterator>(
		  os__, seq__.begin(), seq__.end(), name__, width__, prec__, sep__, eol__);
	}

	/**
	 * @brief Write the given sequence to the specified file path, over-writing it
	 * in the process.
	 *
	 * @param[in] path__ The @c boost::filesystem path to the file that will be
	 * over-written with the sequence's data.
	 *
	 * @param[in] begin__ An iterator at the beginning of the sequence.
	 *
	 * @param[in] end__ A iterator at the end of the sequence.
	 *
	 * @param[in] name__ An optional name to prefix to the sequence.  A typical
	 * usage of this option is to prefix the sequence with the sequence name.
	 * When the sequence is read back again, the prefixed name of the sequence can
	 * be asserted to match the name of the sequence variable.
	 *
	 * @param[in] width__ Fixed width specifier of each output value of the
	 * sequence.
	 *
	 * @param[in] prec__ The precision specification for floating point output.
	 *
	 * @param[in] sep__ A string that separates each output value of the sequence.
	 *
	 * @param[in] eol__ The string to write at the end of the written sequence.
	 */
	template<class Input_Iterator>
	void write_sequence(
	  const boost::filesystem::path& path__,
	  Input_Iterator begin__,
	  Input_Iterator end__,
	  const std::string& name__ = "",
	  size_t width__ = 0,
	  int prec__ = -1,
	  const std::string& sep__ = "\t",
	  const std::string& eol__ = "\n")
	{
		boost::filesystem::ofstream os(path__);
		os.exceptions(
		  std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);
		if (!os.good())
			throw std::runtime_error{ "Failed to open output file" };
		write_sequence(os, begin__, end__, name__, width__, prec__, sep__, eol__);
	}

	/**
	 * A convenience option for @c write_sequence that writes all of the specified
	 * sequence to the specified file path.
	 *
	 * Use this function call to avoid spelling out the @c begin and @c end of a
	 * sequence to @c write_sequence.
	 *
	 * @param[in] path__ The path to the file that will be over-written with the
	 * sequence's data.
	 *
	 * @param[in] seq__ The sequence to output and which has @c begin and @c end
	 * member functions.
	 *
	 * @param[in] name__ An optional name to prefix to the sequence.  A typical
	 * usage of this option is to prefix the sequence with the sequence name.
	 * When the sequence is read back again, the prefixed name of the sequence can
	 * be asserted to match the name of the sequence variable.
	 *
	 * @param[in] width__ Fixed width specifier of each output value of the
	 * sequence.
	 *
	 * @param[in] prec__ The precision specification for floating point output.
	 *
	 * @param[in] sep__ A string that separates each output value of the sequence.
	 *
	 * @param[in] eol__ The string to write at the end of the written sequence.
	 */
	template<class Sequence_Type>
	void write_sequence(
	  const boost::filesystem::path& path__,
	  const Sequence_Type& seq__,
	  const std::string& name__ = "",
	  size_t width__ = 0,
	  int prec__ = -1,
	  const std::string& sep__ = "\t",
	  const std::string& eol__ = "\n")
	{
		write_sequence<typename Sequence_Type::const_iterator>(
		  path__, seq__.begin(), seq__.end(), name__, width__, prec__, sep__, eol__);
	}

	/**
	 * Append the given sequence to the specified file.
	 *
	 * @param[in] path__ The Posix pathname to the file that will be appended with
	 * the sequence's data.
	 *
	 * @param[in] begin__ An iterator at the beginning of the sequence.
	 *
	 * @param[in] end__ A iterator at the end of the sequence.
	 *
	 * @param[in] name__ An optional name to prefix to the sequence.  A typical
	 * usage of this option is to prefix the sequence with the sequence name.
	 * When the sequence is read back again, the prefixed name of the sequence can
	 * be asserted to match the name of the sequence variable.
	 *
	 * @param[in] width__ Fixed width specifier of each output value of the
	 * sequence.
	 *
	 * @param[in] prec__ The precision specification for floating point output.
	 *
	 * @param[in] sep__ A string that separates each output value of the sequence.
	 *
	 * @param[in] eol__ The string to write at the end of the written sequence.
	 */
	template<class Input_Iterator>
	void append_write_sequence(
	  const boost::filesystem::path& path__,
	  Input_Iterator begin__,
	  Input_Iterator end__,
	  std::string name__ = "",
	  size_t width__ = 0,
	  int prec__ = -1,
	  const std::string& sep__ = "\t",
	  const std::string& eol__ = "\n")
	{
		boost::filesystem::ofstream os(path__, std::ios_base::ate);
		os.exceptions(
		  std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);
		if (!os.good())
			throw std::runtime_error{ "Failed to open output file" };
		write_sequence(os, begin__, end__, name__, width__, prec__, sep__, eol__);
	}

} // end namespace io
// EOF
