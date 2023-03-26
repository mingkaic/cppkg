
#ifndef DISABLE_ARRS_LIST_TEST


#include "gtest/gtest.h"

#include "verum/exam/macros.h"

#include "cppkg/arrs/list.h"


using namespace cppkg::arrs;


TEST(LIST, Range)
{
	std::list<int> lr = list_range<int>(2, 5);
	std::vector<int> expect = {2, 3, 4};
	ASSERT_VECEQ(expect, lr);
}


TEST(LIST, LeftConcat)
{
	std::list<char> l = {'a', 'b', 'c'};
	std::list<char> r = {'d', 'e', 'f'};
	lconcat(l, r);
	std::vector<char> expectl = {'a', 'b', 'c', 'd', 'e', 'f'};
	std::vector<char> expectr = {'d', 'e', 'f'};
	EXPECT_VECEQ(expectl, l);
	ASSERT_VECEQ(expectr, r);
}


TEST(LIST, RightConcat)
{
	std::list<char> l = {'a', 'b', 'c'};
	std::list<char> r = {'d', 'e', 'f'};
	rconcat(l, r);
	std::vector<char> expectl = {'a', 'b', 'c'};
	std::vector<char> expectr = {'a', 'b', 'c', 'd', 'e', 'f'};
	EXPECT_VECEQ(expectl, l);
	ASSERT_VECEQ(expectr, r);
}


TEST(LIST, MultiConcat)
{
	std::list<char> first = {'a', 'b', 'c'};
	std::list<char> second = {'d', 'e', 'f'};
	std::list<char> third = {'g', 'h', 'i'};
	auto result = concat(first, second, third);
	std::vector<char> expect = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
	ASSERT_VECEQ(expect, result);
}


#endif // DISABLE_ARRS_LIST_TEST
