
#ifndef DISABLE_FMTS_TEST

#include <array>
#include <list>
#include <vector>
#include <unordered_map>

#include "gtest/gtest.h"

#include "fmts/fmts.hpp"


TEST(FORMAT, ExactString)
{
	EXPECT_STREQ("abcd\\\\efgh\\\\ijkl\\\\",
		fmts::ExactString("abcd\\efgh\\ijkl\\").to_string().c_str());
	EXPECT_STREQ("\\\\abcd\\\\efgh\\\\ijkl",
		fmts::ExactString("\\abcd\\efgh\\ijkl").to_string().c_str());
}


TEST(FORMAT, ExactStringStreamed)
{
	std::stringstream ss;
	ss << fmts::ExactString("abcd\\efgh\\ijkl\\");
	EXPECT_STREQ("abcd\\\\efgh\\\\ijkl\\\\",
		ss.str().c_str());
}


TEST(FMTS, Join)
{
	EXPECT_STREQ("hello world !",
		fmts::join(" ",
			std::vector<std::string>{"hello", "world", "!"}).c_str());
}


TEST(FMTS, Sprintf)
{
	const char* str = "string";
	const char* oth = "other";
	std::string s = fmts::sprintf("%% %s %d %.3f %% %%%% %s %d %.1f %% %%%%",
		str, 123, 5.689, oth, 77, 0.4, 12);
	EXPECT_STREQ("% string 123 5.689 % %% other 77 0.4 % %%", s.c_str());
}


TEST(FMTS, LeftTrim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::ltrim(content);
	EXPECT_STREQ("abce  dfgi hjk lm  ", content.c_str());
}


TEST(FMTS, RightTrim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::rtrim(content);
	EXPECT_STREQ("    abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Trim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::trim(content);
	EXPECT_STREQ("abce  dfgi hjk lm", content.c_str());
}

TEST(FMTS, LeftStrip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::lstrip(content, std::unordered_set<char>{'_', '*', '('});
	EXPECT_STREQ("abce  dfgi hjk lm)*", content.c_str());
}


TEST(FMTS, RightStrip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::rstrip(content, std::unordered_set<char>{'_', '*', ')'});
	EXPECT_STREQ("_*_(abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Strip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::strip(content, std::unordered_set<char>{'_', '*', '(', ')'});
	EXPECT_STREQ("abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Split)
{
	std::string content = ", ,abce, df,gi, hjk, lm, ";
	auto arrs = fmts::split(content, ", ");
	// expect ["", ",abce", "df,gi", "hjk", "lm", ""]
	ASSERT_EQ(6, arrs.size());
	EXPECT_STREQ("", arrs[0].c_str());
	EXPECT_STREQ(",abce", arrs[1].c_str());
	EXPECT_STREQ("df,gi", arrs[2].c_str());
	EXPECT_STREQ("hjk", arrs[3].c_str());
	EXPECT_STREQ("lm", arrs[4].c_str());
	EXPECT_STREQ("", arrs[5].c_str());
}


#endif // DISABLE_FMTS_TEST
