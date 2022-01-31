
#ifndef DISABLE_UNITS_TIME_TEST


#include "gtest/gtest.h"

#include "units/units.hpp"


TEST(TIME, NoConvertReadable)
{
	auto msg = unit::human_readable_time(123, unit::NANO);
	EXPECT_STREQ("123ns", msg.c_str());
}


TEST(TIME, ConvertedReadable)
{
	auto msg = unit::human_readable_time(311244, unit::MICRO);
	EXPECT_STREQ("3.1124ds", msg.c_str());
}


TEST(TIME, MaxSupportedUnitReadability)
{
	auto msg = unit::human_readable_time(311244, unit::ONE);
	EXPECT_STREQ("3.1124e+05s", msg.c_str());
}


#endif // DISABLE_EXAM_TEST
