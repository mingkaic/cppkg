
#ifndef DISABLE_ARRS_ARRAY_TEST


#include "gtest/gtest.h"

#include "verum/exam/macros.h"

#include "cppkg/arrs/array.h"


using namespace cppkg::arrs;


TEST(ARRAY, Range)
{
	std::array<int,3> lr = array_range<int,3>(2);
	std::array<int,3> expect = {2, 3, 4};
	ASSERT_ARREQ(expect, lr);
}


#endif // DISABLE_ARRS_ARRAY_TEST
