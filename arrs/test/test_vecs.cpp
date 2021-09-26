
#ifndef DISABLE_ARRS_VECS_TEST


#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "arrs/arrs.hpp"


TEST(VECS, Range)
{
	std::vector<int> lr = arrs::range<int>(2, 5);
	std::vector<int> expect = {2, 3, 4};
	ASSERT_VECEQ(expect, lr);
}


TEST(VECS, LeftConcat)
{
	std::vector<char> l = {'a', 'b', 'c'};
	std::vector<char> r = {'d', 'e', 'f'};
	arrs::lconcat(l, r);
	std::vector<char> expectl = {'a', 'b', 'c', 'd', 'e', 'f'};
	std::vector<char> expectr = {'d', 'e', 'f'};
	EXPECT_VECEQ(expectl, l);
	ASSERT_VECEQ(expectr, r);
}


TEST(VECS, RightConcat)
{
	std::vector<char> l = {'a', 'b', 'c'};
	std::vector<char> r = {'d', 'e', 'f'};
	arrs::rconcat(l, r);
	std::vector<char> expectl = {'a', 'b', 'c'};
	std::vector<char> expectr = {'a', 'b', 'c', 'd', 'e', 'f'};
	EXPECT_VECEQ(expectl, l);
	ASSERT_VECEQ(expectr, r);
}


TEST(VECS, MultiConcat)
{
	std::vector<char> first = {'a', 'b', 'c'};
	std::vector<char> second = {'d', 'e', 'f'};
	std::vector<char> third = {'g', 'h', 'i'};
	auto result = arrs::concat(first, second, third);
	std::vector<char> expect = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
	ASSERT_VECEQ(expect, result);
}


#endif // DISABLE_ARRS_VECS_TEST
