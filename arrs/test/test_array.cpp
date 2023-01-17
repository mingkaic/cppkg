
#ifndef DISABLE_ARRS_ARRAY_TEST


#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "arrs/arrs.hpp"


TEST(ARRAY, Range)
{
	std::array<int,3> lr = arrs::array_range<int,3>(2);
	std::array<int,3> expect = {2, 3, 4};
	ASSERT_ARREQ(expect, lr);
}


#endif // DISABLE_ARRS_ARRAY_TEST
