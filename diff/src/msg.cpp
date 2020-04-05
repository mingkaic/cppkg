#include "diff/msg.hpp"

#ifdef PKG_DIFF_MSG_HPP

namespace diff
{

std::string diff_msg (
	const fmts::StringsT& expect,
	const fmts::StringsT& got)
{
	auto diffs = myers_diff(expect, got);
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

static void process_lines (fmts::StringsT& lines, std::istream& str,
	bool ignore_empty_lines, bool trim_spaces)
{
	std::string line;
	while (std::getline(str, line))
	{
		if (trim_spaces)
		{
			fmts::trim(line);
		}
		if (false == ignore_empty_lines || line.empty())
		{
			// consider line
			lines.push_back(line);
		}
	}
}

const size_t batch_limit = std::numeric_limits<IndexT>::max();

std::string safe_diff_msg (
	const fmts::StringsT& expect,
	const fmts::StringsT& got)
{
	size_t nexpect = expect.size();
	size_t ngot = got.size();

	if (nexpect > batch_limit && ngot > batch_limit)
	{
		return diff_msg(expect, got);
	}

	auto exit = expect.begin();
	auto goit = got.begin();
	fmts::StringsT exbatch(exit, exit + std::min(batch_limit, nexpect));
	fmts::StringsT gobatch(goit, goit + std::min(batch_limit, ngot));

	auto diffs = myers_diff(exbatch, gobatch);

	IndexT expect_begin = -1, got_begin = -1, diff_end = -1;
	// accept all contiguous non-EQ batches before the end of the line
	for (IndexT i = 0, ndiffs = diffs.size(); i < ndiffs; ++i)
	{
		if (diffs[i].action_ != EQ)
		{
			if (expect_begin < 0)
			{
				expect_begin = diffs[i].orig_;
			}
			if (got_begin < 0)
			{
				got_begin = diffs[i].updated_;
			}
		}
		else
		{
			expect_begin = got_begin = -1;
			diff_end = i;
		}
	}

	std::stringstream out;
	if (diff_end > -1)
	{
		bool show[diff_end + 1];
		std::memset(show, false, sizeof(bool) * (diff_end + 1));
		// mark exact line to show
		for (IndexT i = 0; i < diff_end + 1; ++i)
		{
			if (diffs[i].action_ != EQ)
			{
				IndexT begin = std::max(0, i - lines_before);
				IndexT end = std::min(diff_end, IndexT(i + lines_after));
				std::memset(show + begin, true, sizeof(bool) * (end - begin + 1));
			}
		}

		for (IndexT i = 0; i < diff_end + 1; ++i)
		{
			if (show[i])
			{
				diff_line_format(out, diffs[i]);
			}
		}
	}

	// revalidate contiguous non-EQ that continues pass the end of line
	// and affix to existing messages
	return out.str() + safe_diff_msg(
		fmts::StringsT(exit + expect_begin, expect.end()),
		fmts::StringsT(goit + got_begin, got.end()));
}

std::string diff_lines (
	std::istream& expect, std::istream& got,
	bool ignore_empty_lines, bool trim_spaces)
{
	fmts::StringsT exlines;
	fmts::StringsT golines;
	process_lines(exlines, expect, ignore_empty_lines, trim_spaces);
	process_lines(golines, got, ignore_empty_lines, trim_spaces);

	return safe_diff_msg(exlines, golines);
}

}

#endif
