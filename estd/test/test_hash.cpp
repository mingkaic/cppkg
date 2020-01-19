
#ifndef DISABLE_ESTD_HASH_TEST

#include <array>
#include <list>
#include <vector>

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/hash.hpp"


TEST(HASH, Has)
{
	std::map<std::string,size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::set<std::string> ssets = {
		"abcdef", "defghi", "ghijkl",
	};

	std::unordered_map<std::string,size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::unordered_set<std::string> ussets = {
		"abcdef", "defghi", "ghijkl",
	};

	EXPECT_TRUE(estd::has(stuff, "abcdef"));
	EXPECT_TRUE(estd::has(stuff, "defghi"));
	EXPECT_TRUE(estd::has(stuff, "ghijkl"));
	EXPECT_FALSE(estd::has(stuff, "ghi"));
	EXPECT_FALSE(estd::has(stuff, "mno"));

	EXPECT_TRUE(estd::has(ssets, "abcdef"));
	EXPECT_TRUE(estd::has(ssets, "defghi"));
	EXPECT_TRUE(estd::has(ssets, "ghijkl"));
	EXPECT_FALSE(estd::has(ssets, "ghi"));
	EXPECT_FALSE(estd::has(ssets, "mno"));

	EXPECT_TRUE(estd::has(ustuff, "abcdef"));
	EXPECT_TRUE(estd::has(ustuff, "defghi"));
	EXPECT_TRUE(estd::has(ustuff, "ghijkl"));
	EXPECT_FALSE(estd::has(ustuff, "ghi"));
	EXPECT_FALSE(estd::has(ustuff, "mno"));

	EXPECT_TRUE(estd::has(ussets, "abcdef"));
	EXPECT_TRUE(estd::has(ussets, "defghi"));
	EXPECT_TRUE(estd::has(ussets, "ghijkl"));
	EXPECT_FALSE(estd::has(ussets, "ghi"));
	EXPECT_FALSE(estd::has(ussets, "mno"));
}


TEST(HASH, Get)
{
	std::map<std::string,size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::unordered_map<std::string,size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	size_t id;
	ASSERT_TRUE(estd::get(id, stuff, "abcdef"));
	EXPECT_EQ(123, id);
	ASSERT_TRUE(estd::get(id, stuff, "defghi"));
	EXPECT_EQ(456, id);
	ASSERT_TRUE(estd::get(id, stuff, "ghijkl"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, stuff, "ghi"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, stuff, "mno"));

	ASSERT_TRUE(estd::get(id, ustuff, "abcdef"));
	EXPECT_EQ(123, id);
	ASSERT_TRUE(estd::get(id, ustuff, "defghi"));
	EXPECT_EQ(456, id);
	ASSERT_TRUE(estd::get(id, ustuff, "ghijkl"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, ustuff, "ghi"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, ustuff, "mno"));
	EXPECT_EQ(789, id);
}


TEST(HASH, TryGet)
{
	std::map<std::string,size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::unordered_map<std::string,size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	EXPECT_EQ(123, estd::try_get(stuff, "abcdef", 147));
	EXPECT_EQ(456, estd::try_get(stuff, "defghi", 147));
	EXPECT_EQ(789, estd::try_get(stuff, "ghijkl", 147));
	EXPECT_EQ(147, estd::try_get(stuff, "ghi", 147));
	EXPECT_EQ(147, estd::try_get(stuff, "mno", 147));

	EXPECT_EQ(123, estd::try_get(ustuff, "abcdef", 147));
	EXPECT_EQ(456, estd::try_get(ustuff, "defghi", 147));
	EXPECT_EQ(789, estd::try_get(ustuff, "ghijkl", 147));
	EXPECT_EQ(147, estd::try_get(ustuff, "ghi", 147));
	EXPECT_EQ(147, estd::try_get(ustuff, "mno", 147));
}


TEST(HASH, MustGet)
{
	std::map<std::string,size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::unordered_map<std::string,size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::string ohnos = "oh nos %d";

	EXPECT_EQ(123, estd::must_getf(stuff, "abcdef", ohnos, 1));
	EXPECT_EQ(456, estd::must_getf(stuff, "defghi", ohnos, 2));
	EXPECT_EQ(789, estd::must_getf(stuff, "ghijkl", ohnos, 3));

	EXPECT_EQ(123, estd::must_getf(ustuff, "abcdef", ohnos, 4));
	EXPECT_EQ(456, estd::must_getf(ustuff, "defghi", ohnos, 5));
	EXPECT_EQ(789, estd::must_getf(ustuff, "ghijkl", ohnos, 6));

	EXPECT_FATAL(estd::must_getf(stuff, "ghi", ohnos, 7),
		fmts::sprintf(ohnos, 7).c_str());
	EXPECT_FATAL(estd::must_getf(stuff, "mno", ohnos, 8),
		fmts::sprintf(ohnos, 8).c_str());
	EXPECT_FATAL(estd::must_getf(ustuff, "ghi", ohnos, 9),
		fmts::sprintf(ohnos, 9).c_str());
	EXPECT_FATAL(estd::must_getf(ustuff, "mno", ohnos, 0),
		fmts::sprintf(ohnos, 0).c_str());
}


TEST(HASH, ArrHas)
{
	std::array<size_t,3> arr = {123, 456, 789};
	std::vector<size_t> vec = {123, 456, 789};
	std::list<size_t> lst = {123, 456, 789};

	EXPECT_TRUE(estd::arr_has(arr, 123));
	EXPECT_TRUE(estd::arr_has(vec, 123));
	EXPECT_TRUE(estd::arr_has(lst, 123));

	EXPECT_TRUE(estd::arr_has(arr, 456));
	EXPECT_TRUE(estd::arr_has(vec, 456));
	EXPECT_TRUE(estd::arr_has(lst, 456));

	EXPECT_TRUE(estd::arr_has(arr, 789));
	EXPECT_TRUE(estd::arr_has(vec, 789));
	EXPECT_TRUE(estd::arr_has(lst, 789));

	EXPECT_FALSE(estd::arr_has(arr, 270));
	EXPECT_FALSE(estd::arr_has(vec, 270));
	EXPECT_FALSE(estd::arr_has(lst, 270));
}


#endif // DISABLE_ESTD_HASH_TEST
