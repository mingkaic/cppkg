#include "diff/msg.hpp"

#ifdef DIFF_MSG_HPP

namespace diff
{

std::string diff_msg (
	std::vector<std::string> expected, std::vector<std::string> got)
{
	auto diffs = myers_diff(expected, got);
	IndexT ndiffs = diffs.size();
	bool show[ndiffs];
	std::memset(show, false, sizeof(bool) * ndiffs);
	// mark exact line to show
	for (IndexT i = 0; i < ndiffs; ++i)
	{
		if (diffs[i].action_ != EQ)
		{
			IndexT begin = std::max(0, i - lines_before);
			IndexT end = std::min(ndiffs - 1, i + lines_after);
			std::memset(show + begin, true, sizeof(bool) * (end - begin + 1));
		}
	}
	std::stringstream out;
	for (IndexT i = 0; i < ndiffs; ++i)
	{
		if (show[i])
		{
			diff_line_format(out, diffs[i]);
		}
	}
	return out.str();
}

}

#endif
