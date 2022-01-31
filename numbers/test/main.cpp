#include "gtest/gtest.h"

#include "numbers/numbers.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_NUMBERS_TEST


TEST(NUMBERS, ZeroFraction)
{
	numbers::Fraction z;
	EXPECT_DOUBLE_EQ(0, double(z));
}


TEST(NUMBERS, SmallFraction)
{
	numbers::Fraction f(24.2, 1223);
	double expect = 24.2 / 1223;
	EXPECT_DOUBLE_EQ(expect, double(f));
}


TEST(NUMBERS, BigFraction)
{
	numbers::Fraction f(24.2, 3.14159265358e+12);
	EXPECT_DOUBLE_EQ(24.2/3.14159265358e+12, double(f));
	EXPECT_DOUBLE_EQ(24.2, f.num_);
	EXPECT_EQ(3.14159265358e+12, f.denom_);
}


TEST(NUMBERS, OneFraction)
{
	numbers::Fraction f(233, 233);
	EXPECT_DOUBLE_EQ(1, double(f));
}


TEST(NUMBERS, SmallFractionMultiply)
{
	numbers::Fraction f(1224.2, 2312);
	numbers::Fraction f2(2.5, 372);

	auto result = f * f2;
	EXPECT_DOUBLE_EQ(1224.2 * 2.5, result.num_);
	EXPECT_EQ(372 * 2312, result.denom_);

	double expect = double(f) * double(f2);
	EXPECT_DOUBLE_EQ(expect, double(result));
}


TEST(NUMBERS, BigFractionMultiply)
{
	double numerator = 3.7;
	uint64_t denominator = 2914159261;
	numbers::Fraction f(1224.2, 3.141592653e+10);
	numbers::Fraction f2(numerator, denominator);

	auto result = f * f2;
	EXPECT_DOUBLE_EQ(double(f) * numerator, result.num_);
	EXPECT_EQ(denominator, result.denom_);

	double expect = (double(f) * numerator) / denominator;
	EXPECT_DOUBLE_EQ(expect, double(result));
}


TEST(NUMBERS, HugeNumeratorMultiply)
{
	double numerator = 11122.2;
	uint64_t denominator = 21175;
	numbers::Fraction f(1224.2, 1415);
	numbers::Fraction f2(numerator, denominator);

	auto result = f * f2;
	EXPECT_DOUBLE_EQ(double(f) * numerator, result.num_);
	EXPECT_EQ(denominator, result.denom_);

	double expect = (double(f) * numerator) / denominator;
	EXPECT_DOUBLE_EQ(expect, double(result));
}


TEST(NUMBERS, ZeroPower)
{
	numbers::Fraction f(1222.7, 23122);

	auto result = numbers::pow(f, 0);
	EXPECT_EQ(1, double(result));
}


TEST(NUMBERS, OnePower)
{
	numbers::Fraction f(1222.7, 23122);

	auto result = numbers::pow(f, 1);
	EXPECT_EQ(double(f), double(result));
}


TEST(NUMBERS, EvenPower)
{
	numbers::Fraction f(1222.7, 23122);

	auto result = numbers::pow(f, 4);
	double expect = 7.819483389285647e-06;
	EXPECT_EQ(expect, double(result));
}


TEST(NUMBERS, OddPower)
{
	numbers::Fraction f(1222.7, 23122);

	auto result = numbers::pow(f, 5);
	double expect = double(f) * 7.819483389285647e-06;
	EXPECT_EQ(expect, double(result));
}


TEST(NUMBERS, Reverse)
{
	numbers::Fraction f(1222.7, 23122);

	auto result = numbers::reverse(f);
	EXPECT_EQ(1. - double(f), double(result));
	EXPECT_EQ(23122 - 1222.7, result.num_);
	EXPECT_EQ(f.denom_, result.denom_);
}


#endif // DISABLE_NUMBERS_TEST
