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

#include "diff/format.hpp"

#ifndef DIFF_MSG_HPP
#define DIFF_MSG_HPP

namespace diff
{

/// Number of lines to display before lines where differences occured
const uint8_t lines_before = 3;

/// Number of lines to display after lines where differences occured
const uint8_t lines_after = 3;

/// Return diff message of multiple lines
/// Message is empty if the lines are the same
std::string diff_msg (
	std::vector<std::string> expected, std::vector<std::string> got);

}

#endif // DIFF_MSG_HPP
