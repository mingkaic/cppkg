///
/// format.h
/// diff
///
/// Purpose:
/// Define diff structure (defined in myers_diff.h) string representation
///

#ifndef CPPKG_DIFF_FORMAT_H
#define CPPKG_DIFF_FORMAT_H

#include <ostream>

#include "cppkg/diff/myers_diff.h"

namespace cppkg
{

namespace diff
{

/// Symbol for added lines
const char add_token = '+';

/// Symbol for removed lines
const char del_token = '-';

/// Format specified difference lines and stream to out
template <typename T>
void diff_line_format (std::ostream& out, T val,
	Action action, int64_t orig, int64_t updated)
{
	switch (action)
	{
		case EQ:
			out << "  " << orig << "\t" << updated;
			break;
		case ADD:
			out << add_token << "  \t" << updated;
			break;
		case DEL:
			out << del_token << " " << orig << "\t ";
			break;
	}
	out << "\t" << val << "\n";
}

}

}

#endif // CPPKG_DIFF_FORMAT_H
