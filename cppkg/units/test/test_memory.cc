
#ifndef DISABLE_UNITS_MEMORY_TEST


#include "gtest/gtest.h"

#include "cppkg/units/time.h"
#include "cppkg/units/memory.h"


using namespace cppkg::unit;


TEST(MEMORY, RelativeSizes)
{
	auto size = KB(3);
	auto ksize = MB(3);
	auto msize = GB(3);
	EXPECT_EQ(1000 * size, ksize);
	EXPECT_EQ(1000 * ksize, msize);
}


TEST(MEMORY, BaseUnitReadability)
{
	MemoryT bytes = 311;
	auto msg = human_readable_memory(bytes);
	EXPECT_STREQ("311 Byte", msg.c_str());
}


TEST(MEMORY, ConvertedUnitReadability)
{
	MemoryT bytes = 311244;
	auto msg = human_readable_memory(bytes);
	EXPECT_STREQ("311 KB", msg.c_str());
}


TEST(MEMORY, BiggerUnitReadability)
{
	MemoryT bytes = 31124422;
	auto msg = human_readable_memory(bytes);
	EXPECT_STREQ("31.1 MB", msg.c_str());
}


TEST(MEMORY, MaxSupportedUnitReadability)
{
	MemoryT bytes = GB(3112442211);
	auto msg = human_readable_memory(bytes);
	EXPECT_STREQ("3.11e+09 GB", msg.c_str());
}


#endif // DISABLE_EXAM_TEST
