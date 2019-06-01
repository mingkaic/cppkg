///
/// format.hpp
/// diff
///
/// Purpose:
/// Define diff structure (defined in myers_diff.hpp) string representation
///

#include <ostream>

#include "diff/myers_diff.hpp"

namespace diff
{

/// Symbol for added lines
const char add_token = '+';

/// Symbol for removed lines
const char del_token = '-';

/// Format specified difference lines and stream to out
template <typename T>
void diff_line_format (std::ostream& out, Diff<T>& line)
{
	switch (line.action_)
	{
		case EQ:
			out << "  " << line.orig_ << "\t" << line.updated_;
			break;
		case ADD:
			out << add_token << "  \t" << line.updated_;
			break;
		case DEL:
			out << del_token << " " << line.orig_ << "\t ";
			break;
	}
	out << "\t" << line.val_ << "\n";
}

}
