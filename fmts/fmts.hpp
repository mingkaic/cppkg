///
/// fmts.hpp
/// fmts
///
/// Purpose:
/// Define string formatting for various types
///

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

#ifndef ERR_STRING_HPP
#define ERR_STRING_HPP

namespace fmts
{

/// Symbol for the start of an array as string
const char arr_begin = '[';

/// Symbol for the end of an array as string
const char arr_end = ']';

/// Symbol for the delimter between elements of an array as string
const char arr_delim = '\\';

struct string final
{
	string (const char* cstr) : val_(cstr) {}

	string (const std::string& sstr) : val_(sstr) {}

	operator std::string()
	{
		std::string modified = val_;
		for (size_t i = 0, n = modified.size(); i < n; ++i)
		{
			switch (modified[i]) {
				case arr_begin:
				case arr_end:
				case arr_delim:
					modified.insert(modified.begin() + i, arr_delim);
					++i;
					++n;
			}
		}
		return modified;
	}

	std::string val_;
};

std::ostream& operator << (std::ostream& os, string sstr);

/// Stream C-style strings to s
void to_stream (std::ostream& s, const char* str);

/// Stream byte-size integers and display as numbers to s
void to_stream (std::ostream& s, int8_t c);

/// Stream byte-size unsigned integers and display as numbers to s
void to_stream (std::ostream& s, uint8_t c);

/// Stream generic value to s
template <typename T, typename std::enable_if<!std::is_array<T>::value>::type* = nullptr>
void to_stream (std::ostream& s, T val)
{
	s << val;
}

/// Stream values between iterators as an array delimited by delim input
template <typename Iterator>
void arr_to_stream (std::ostream& s, Iterator begin, Iterator end,
	std::string delim = std::string(1, arr_delim))
{
	if (begin != end)
	{
		to_stream(s, *(begin++));
		while (begin != end)
		{
			s << delim;
			to_stream(s, *(begin++));
		}
	}
}

/// Stream values between iterators as an array
template <typename Iterator>
void to_stream (std::ostream& s, Iterator begin, Iterator end)
{
	s << arr_begin;
	arr_to_stream(s, begin, end);
	s << arr_end;
}

/// Stream generic value to s applied to array types
template <typename T, typename std::enable_if<std::is_array<T>::value>::type* = nullptr>
void to_stream (std::ostream& s, T val)
{
	to_stream(s, std::begin(val), std::end(val));
}

/// Return string representation for common arguments
template <typename T>
std::string to_string (T arg)
{
	std::stringstream ss;
	to_stream(ss, arg);
	return ss.str();
}

/// Return string representation of values between iterators
template <typename Iterator>
std::string to_string (Iterator begin, Iterator end)
{
	std::stringstream ss;
	to_stream(ss, begin, end);
	return ss.str();
}

/// Stream generic value to s applied to array types
template <typename Iterator>
std::string join (std::string delim, Iterator begin, Iterator end)
{
	std::stringstream ss;
	arr_to_stream(ss, begin, end, delim);
	return ss.str();
}

/// Return std::string with snprintf formatting
template <typename... ARGS>
std::string sprintf (std::string format, ARGS... args)
{
	size_t n = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	char buf[n];
	std::snprintf(buf, n, format.c_str(), args...);
	return std::string(buf, buf + n - 1);
}

void ltrim(std::string& s);

void rtrim(std::string& s);

void trim(std::string& s);

std::vector<std::string> split (std::string s, std::string delim);

}

#endif // ERR_STRING_HPP
