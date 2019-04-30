/**
 * @file        IO-Extra/ostream_ww_iterator.hpp
 * @date        Started Sun Jul  9 10:40:51 EST 2006
 * @author      Lester J. Dowling
 * @par         Copyright &copy; Lester J. Dowling.
 */
#pragma once
#include <iostream>
#include <iterator>

namespace io {

	/**
	 * @class ostream_ww_iterator
	 *
	 * The @c std::ostream_iterator modified to accept a field width.
	 *
	 * The @c _ww_ in the middle of this class's name is the initials for "with width".
	 * That is, this is an @c ostream_iterator "with width".
	 */
	template<typename Tp, typename CharT = char, typename Traits = std::char_traits<CharT>>
	class ostream_ww_iterator
	  : public std::iterator<std::output_iterator_tag, void, void, void, void> {
	public:
		typedef CharT char_type;
		typedef Traits traits_type;
		typedef std::basic_ostream<CharT, Traits> ostream_type;

	private:
		ostream_type* _M_stream;
		const CharT* _M_string;
		size_t _M_width;

	public:
		ostream_ww_iterator(ostream_type& __s)
		  : _M_stream(&__s)
		  , _M_string(0)
		  , _M_width(0)
		{
		}

		ostream_ww_iterator(ostream_type& __s, const CharT* __c)
		  : _M_stream(&__s)
		  , _M_string(__c)
		  , _M_width(0)
		{
		}

		ostream_ww_iterator(ostream_type& __s, size_t __w)
		  : _M_stream(&__s)
		  , _M_string(0)
		  , _M_width(__w)
		{
		}

		ostream_ww_iterator(ostream_type& __s, size_t __w, const CharT* __c)
		  : _M_stream(&__s)
		  , _M_string(__c)
		  , _M_width(__w)
		{
		}

		ostream_ww_iterator(const ostream_ww_iterator& __obj)
		  : _M_stream(__obj._M_stream)
		  , _M_string(__obj._M_string)
		  , _M_width(__obj._M_width)
		{
		}

		ostream_ww_iterator& operator=(const Tp& value)
		{
			_M_stream->width(_M_width);
			*_M_stream << value;
			if (_M_string)
				*_M_stream << _M_string;
			return *this;
		}

		ostream_ww_iterator& operator*() { return *this; }

		ostream_ww_iterator& operator++() { return *this; }

		ostream_ww_iterator& operator++(int) { return *this; }
	};
} // end namespace io
// EOF
