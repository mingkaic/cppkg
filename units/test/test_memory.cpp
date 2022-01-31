
#ifndef DISABLE_UNITS_MEMORY_TEST


#include "gtest/gtest.h"

#include "units/units.hpp"


TEST(MEMORY, RelativeSizes)
{
	auto size = unit::KB(3);
	auto ksize = unit::MB(3);
	auto msize = unit::GB(3);
	EXPECT_EQ(1000 * size, ksize);
	EXPECT_EQ(1000 * ksize, msize);
}


TEST(MEMORY, BaseUnitReadability)
{
	unit::MemoryT bytes = 311;
	auto msg = unit::human_readable_memory(bytes);
	EXPECT_STREQ("311 Byte", msg.c_str());
}


TEST(MEMORY, ConvertedUnitReadability)
{
	unit::MemoryT bytes = 311244;
	auto msg = unit::human_readable_memory(bytes);
	EXPECT_STREQ("311 KB", msg.c_str());
}


TEST(MEMORY, BiggerUnitReadability)
{
	unit::MemoryT bytes = 31124422;
	auto msg = unit::human_readable_memory(bytes);
	EXPECT_STREQ("31.1 MB", msg.c_str());
}


TEST(MEMORY, MaxSupportedUnitReadability)
{
	unit::MemoryT bytes = unit::GB(3112442211);
	auto msg = unit::human_readable_memory(bytes);
	EXPECT_STREQ("3.11e+09 GB", msg.c_str());
}


#endif // DISABLE_EXAM_TEST
