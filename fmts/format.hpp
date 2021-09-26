///
/// format.hpp
/// fmts
///
/// Purpose:
/// Define string formatting for various types
///

#ifndef PKG_FMTS_FORMAT_HPP
#define PKG_FMTS_FORMAT_HPP

#include <algorithm>
#include <memory>
#include <sstream>

#include "types/strs.hpp"
#include "types/iterators.hpp"

#include "fmts/istringable.hpp"
#include "fmts/readable.hpp"

namespace fmts
{

/// Wrap string types so that default readable symbols are escaped with '\'
struct ExactString final : public iStringable
{
	ExactString (const char* cstr) : val_(cstr) {}

	ExactString (const std::string& sstr) : val_(sstr) {}

	/// Return string representation by breaking array/pair symbols
	std::string to_string (void) const override
	{
		std::string modified = val_;
		for (size_t i = 0, n = modified.size(); i < n; ++i)
		{
			switch (modified[i])
			{
				case arr_begin:
				case arr_end:
				case arr_delim:
				case pair_delim:
					modified.insert(modified.begin() + i, arr_delim);
					++i;
					++n;
			}
		}
		return modified;
	}

	/// Raw string containing unbroken array/pair symbols
	std::string val_;
};

/// Stream generic value to s applied to array types
template <typename Iterator>
std::string join (std::string delim, Iterator begin, Iterator end)
{
	std::stringstream ss;
	arr_to_stream(ss, begin, end, delim);
	return ss.str();
}

template <typename T, typename std::enable_if<
	std::is_array<T>::value ||
	types::is_iterable<T>::value
>::type* = nullptr>
std::string join (std::string delim, T arr)
{
	return join(delim, std::begin(arr), std::end(arr));
}

/// Return std::string with snprintf formatting
template <typename... ARGS>
std::string sprintf (const std::string& format, ARGS... args)
{
	size_t n = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	char buf[n];
	std::snprintf(buf, n, format.c_str(), args...);
	return std::string(buf, buf + n - 1);
}

/// Trim all white-space symbols on the left side of string s
void ltrim(std::string& s);

/// Trim all white-space symbols on the right side of string s
void rtrim(std::string& s);

/// Trim all white-space symbols surrounding string s
void trim(std::string& s);

void lstrip (std::string& s, const std::unordered_set<char>& cset);

void rstrip (std::string& s, const std::unordered_set<char>& cset);

void strip (std::string& s, const std::unordered_set<char>& cset);

/// Return string s split into all substrings separated by delim as a vector
types::StringsT split (std::string s, std::string delim);

}

#endif // PKG_FMTS_FORMAT_HPP
