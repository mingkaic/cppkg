#include "gtest/gtest.h"

#include "diff/msg.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_DIFF_TEST


TEST(DIFF, MyersMinEdit)
{
	EXPECT_EQ(0, diff::myers_diff_min_edit<std::string>(
		"ABCABBA", "ABCABBA"));
	EXPECT_EQ(5, diff::myers_diff_min_edit<std::string>(
		"ABCABBA", "CBABAC"));
}


TEST(DIFF, MyersDiff)
{
	std::string s = "ABCABBA";
	auto same = diff::myers_diff<std::string>(s, "ABCABBA");
	for (size_t i = 0, n = same.size(); i < n; ++i)
	{
		EXPECT_EQ(s[i], same[i].val_);
		EXPECT_EQ(i, same[i].orig_);
		EXPECT_EQ(i, same[i].updated_);
		EXPECT_EQ(diff::EQ, same[i].action_);
	}

	std::string similar = "CBABAC";
	auto notsame = diff::myers_diff<std::string>(s, similar);
	std::vector<std::string> expectations = {
		"- 0\t \tA\n",
		"- 1\t \tB\n",
		"  2\t0\tC\n",
		"+  \t1\tB\n",
		"  3\t2\tA\n",
		"  4\t3\tB\n",
		"- 5\t \tB\n",
		"  6\t4\tA\n",
		"+  \t5\tC\n",
	};
	ASSERT_EQ(expectations.size(), notsame.size());
	for (size_t i = 0, n = notsame.size(); i < n; ++i)
	{
		std::stringstream ss;
		diff::diff_line_format(ss, notsame[i]);
		EXPECT_STREQ(expectations[i].c_str(), ss.str().c_str());
	}

	std::string other = "DEFEDFF";
	auto different = diff::myers_diff<std::string>(s, other);
	size_t n = s.size();
	for (size_t i = 0; i < n; ++i)
	{
		EXPECT_EQ(s[i], different[i].val_);
		EXPECT_EQ(i, different[i].orig_);
		EXPECT_EQ(-1, different[i].updated_);
		EXPECT_EQ(diff::DEL, different[i].action_);
	}
	for (size_t i = 0, m = other.size(); i < m; ++i)
	{
		EXPECT_EQ(other[i], different[n + i].val_);
		EXPECT_EQ(-1, different[n + i].orig_);
		EXPECT_EQ(i, different[n + i].updated_);
		EXPECT_EQ(diff::ADD, different[n + i].action_);
	}
}


TEST(DIFF, Msg)
{
	auto match = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	});

	EXPECT_STREQ("", match.c_str());

	auto same_prefix = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"mate",
		"head",
		"wine",
		"new",
		"tremble",
		"hop",
	});

	EXPECT_STREQ(
		"  3\t3\tgreasy\n"
		"  4\t4\ttoy\n"
		"  5\t5\tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t6\tmate\n"
		"+  \t7\thead\n"
		"+  \t8\twine\n"
		"+  \t9\tnew\n"
		"+  \t10\ttremble\n"
		"+  \t11\thop\n",
		same_prefix.c_str());

	auto same_postfix = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"  7\t7\tjuicy\n"
		"  8\t8\tbright\n"
		"  9\t9\tjam\n",
		same_postfix.c_str());

	auto small_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"obtain",
		"nosy",
		"juicy",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"  5\t4\tobtain\n"
		"  6\t5\tnosy\n"
		"  7\t6\tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t7\tcomplete\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		small_overlap.c_str());

	auto bigger_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"  3\t2\tgreasy\n"
		"  4\t3\ttoy\n"
		"  5\t4\tobtain\n"
		"  7\t6\tjuicy\n"
		"  8\t7\tbright\n"
		"  9\t8\tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		bigger_overlap.c_str());

	auto no_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"+  \t7\tcomplete\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		no_overlap.c_str());
}


#endif // DISABLE_DIFF_TEST
