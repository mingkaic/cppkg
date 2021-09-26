///
/// readable.hpp
/// fmts
///
/// Purpose:
/// Define human readable wrappers
///

#ifndef PKG_FMTS_READABLE_HPP
#define PKG_FMTS_READABLE_HPP

#include <algorithm>
#include <sstream>
#include <memory>

#include "types/iterators.hpp"

#include "fmts/istreamable.hpp"

namespace fmts
{

/// Symbol for the start of an array as string
const char arr_begin = '[';

/// Symbol for the end of an array as string
const char arr_end = ']';

/// Symbol for the delimiter between elements of an array as string
const char arr_delim = '\\';

const char pair_delim = ':';

/// Stream values between iterators as an array delimited by delim input
template <typename IT>
void arr_to_stream (std::ostream& s, IT begin, IT end,
	std::string delim = std::string(1, arr_delim));

/// Stream pair p to s given specified delim between first and second elements
template <typename PL, typename PR>
void pair_to_stream (std::ostream& s, std::pair<PL,PR> p,
	std::string delim = std::string(1, pair_delim));

template <typename T, typename T2=void>
struct Humanize : public iStreamable
{
	Humanize (const T& e) : val_(e) {}

	void read (std::ostream& out) const override
	{
		out << val_;
	}

	T val_;
};

template <>
struct Humanize<int8_t> : public iStreamable
{
	Humanize (const int8_t& e) : val_(e) {}

	void read (std::ostream& out) const override
	{
		out << (int) val_;
	}

	int8_t val_;
};

template <>
struct Humanize<uint8_t> : public iStreamable
{
	Humanize (const uint8_t& e) : val_(e) {}

	void read (std::ostream& out) const override
	{
		out << (unsigned) val_;
	}

	uint8_t val_;
};

template <typename PL, typename PR>
struct Humanize<std::pair<PL,PR>> : public iStreamable
{
	Humanize (const std::pair<PL,PR>& e) : val_(e) {}

	/// Stream pair using default delim
	void read (std::ostream& out) const override
	{
		pair_to_stream(out, val_);
	}

	std::pair<PL,PR> val_;
};

template <typename T>
struct Humanize<T*> : public iStreamable
{
	Humanize (T* begin, T* end) : begin_(begin), end_(end) {}

	/// Stream collection using default delim
	void read (std::ostream& out) const override
	{
		out << arr_begin;
		arr_to_stream(out, begin_, end_);
		out << arr_end;
	}

	T* begin_;

	T* end_;
};

// todo: replace is_array with is_bounded_array
template <typename T>
struct Humanize<T, typename std::enable_if<std::is_array<T>::value &&
	types::is_streamable<T>::value>::type> : public iStreamable
{
	Humanize (T& e) : begin_(std::cbegin(e)), end_(std::cend(e)) {}

	Humanize (const T& e) : begin_(std::cbegin(e)), end_(std::cend(e)) {}

	/// Stream collection using default delim
	void read (std::ostream& out) const override
	{
		out << arr_begin;
		arr_to_stream(out, begin_, end_);
		out << arr_end;
	}

	types::ArrCstItT<T> begin_;

	types::ArrCstItT<T> end_;
};

template <typename T>
struct Humanize<T, typename std::enable_if<types::is_iterable<T>::value &&
	!types::is_streamable<T>::value>::type> : public iStreamable
{
	Humanize (T& e) : begin_(std::cbegin(e)), end_(std::cend(e)) {}

	Humanize (const T& e) : begin_(std::cbegin(e)), end_(std::cend(e)) {}

	Humanize (types::ArrCstItT<T> begin, types::ArrCstItT<T> end) :
		begin_(begin), end_(end) {}

	/// Stream collection using default delim
	void read (std::ostream& out) const override
	{
		out << arr_begin;
		arr_to_stream(out, begin_, end_);
		out << arr_end;
	}

	types::ArrCstItT<T> begin_;

	types::ArrCstItT<T> end_;
};

template <typename IT>
void arr_to_stream (std::ostream& s, IT begin, IT end,
	std::string delim)
{
	if (begin != end)
	{
		s << Humanize<types::IterValT<IT>>(*(begin++));
		while (begin != end)
		{
			s << delim << Humanize<types::IterValT<IT>>(*(begin++));
		}
	}
}

/// Stream pair p to s given specified delim between first and second elements
template <typename PL, typename PR>
void pair_to_stream (std::ostream& s, std::pair<PL,PR> p,
	std::string delim)
{
	s << Humanize<PL>(p.first) << delim << Humanize<PR>(p.second);
}

template <typename T>
std::string readable (const T& e)
{
	return Humanize<T>(e).to_string();
}

}

#endif // PKG_FMTS_READABLE_HPP
