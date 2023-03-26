///
/// msg.h
/// diff
///
/// Purpose:
/// Define diff message generators
///

#ifndef CPPKG_DIFF_MSG_H
#define CPPKG_DIFF_MSG_H

#include <cstring>
#include <istream>
#include <limits>

#include "cisab/fmts/format.h"

#include "cppkg/diff/format.h"

namespace cppkg
{

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
	const cisab::types::StringsT& expected,
	const cisab::types::StringsT& got);

/// Same as diff_msg, except it diffs the message in batches
std::string safe_diff_msg (
	const cisab::types::StringsT& expected,
	const cisab::types::StringsT& got,
	size_t batch_limit = std::numeric_limits<IndexT>::max());

std::string diff_lines (
	std::istream& expect, std::istream& got,
	bool ignore_empty_lines = true,
	bool trim_spaces = true);

}

}

#endif // CPPKG_DIFF_MSG_H
