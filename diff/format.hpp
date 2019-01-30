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

const char ADD_TOKEN = '+';
const char DEL_TOKEN = '-';

template <typename T>
void diff_line_format (std::ostream& out, Diff<T>& line)
{
	switch (line.action_)
	{
		case EQ:
			out << "  " << line.orig_ << "\t" << line.updated_;
			break;
		case ADD:
			out << ADD_TOKEN << "  \t" << line.updated_;
			break;
		case DEL:
			out << DEL_TOKEN << " " << line.orig_ << "\t ";
			break;
	}
	out << "\t" << line.val_ << "\n";
}

}
