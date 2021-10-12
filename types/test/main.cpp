#include "gtest/gtest.h"

#include "types/types.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_TYPES_TEST


TEST(ITERATORS, IsIterable)
{
	EXPECT_TRUE(types::is_iterable<int[3]>::value);
	EXPECT_TRUE(types::is_iterable<std::vector<float>>::value);
	EXPECT_TRUE(types::is_iterable<std::string>::value);
	EXPECT_FALSE(types::is_iterable<char>::value);
	EXPECT_FALSE(types::is_iterable<int>::value);
	EXPECT_FALSE(types::is_iterable<float*>::value);
	EXPECT_FALSE(types::is_iterable<double[]>::value);
	EXPECT_FALSE(types::is_iterable<std::function<int()>>::value);
}


TEST(ITERATORS, IsStreamable)
{
	EXPECT_TRUE(types::is_streamable<int[3]>::value);
	EXPECT_TRUE(types::is_streamable<std::string>::value);
	EXPECT_TRUE(types::is_streamable<char>::value);
	EXPECT_TRUE(types::is_streamable<int>::value);
	EXPECT_FALSE(types::is_streamable<std::vector<float>>::value);
	EXPECT_FALSE(types::is_streamable<std::set<float>>::value);
	EXPECT_FALSE(types::is_streamable<std::function<int()>>::value);
}


TEST(MATH, IsComplex)
{
	EXPECT_TRUE(types::is_complex<std::complex<float>>::value);
	EXPECT_FALSE(types::is_complex<float>::value);
}


#endif // DISABLE_TYPES_TEST
