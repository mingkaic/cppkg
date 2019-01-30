#include <cstring>
#include <string>
#include <sstream>

#include "diff/format.hpp"

#ifndef DIFF_MSG_HPP
#define DIFF_MSG_HPP

namespace diff
{

const uint8_t LINES_BEFORE = 3;
const uint8_t LINES_AFTER = 3;

/// Return diff message of multiple lines
/// Message is empty if the lines are the same
std::string diff_msg (
	std::vector<std::string> expected, std::vector<std::string> got);

}

#endif // DIFF_MSG_HPP
