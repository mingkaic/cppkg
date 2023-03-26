
#ifndef DISABLE_UNITS_TIME_TEST


#include "gtest/gtest.h"

#include "cppkg/units/time.h"
#include "cppkg/units/memory.h"


using namespace cppkg::unit;


TEST(TIME, NoConvertReadable)
{
	auto msg = human_readable_time(123, NANO);
	EXPECT_STREQ("123ns", msg.c_str());
}


TEST(TIME, ConvertedReadable)
{
	auto msg = human_readable_time(311244, MICRO);
	EXPECT_STREQ("3.1124ds", msg.c_str());
}


TEST(TIME, MaxSupportedUnitReadability)
{
	auto msg = human_readable_time(311244, ONE);
	EXPECT_STREQ("3.1124e+05s", msg.c_str());
}


#endif // DISABLE_EXAM_TEST
