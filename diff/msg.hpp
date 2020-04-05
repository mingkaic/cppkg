///
/// msg.hpp
/// diff
///
/// Purpose:
/// Define diff message generators
///

#include <cstring>
#include <string>
#include <sstream>
#include <istream>

#include "fmts/fmts.hpp"

#include "diff/format.hpp"

#ifndef PKG_DIFF_MSG_HPP
#define PKG_DIFF_MSG_HPP

namespace diff
{

/// Number of lines to display before lines where differences occured
const uint8_t lines_before = 3;

/// Number of lines to display after lines where differences occured
const uint8_t lines_after = 3;

/// Return diff message of multiple lines
/// Message is empty if the lines are the same
/// Caveat: the product of size of the vectors is roughly limited to 2^32,
/// this function does not perform any optimization to diff long messages
std::string diff_msg (
	const fmts::StringsT& expected,
	const fmts::StringsT& got);

/// Same as diff_msg, except it diffs the message in batches
std::string safe_diff_msg (
	const fmts::StringsT& expected,
	const fmts::StringsT& got);

std::string diff_lines (
	std::istream& expect, std::istream& got,
	bool ignore_empty_lines = true,
	bool trim_spaces = true);

}

#endif // PKG_DIFF_MSG_HPP
