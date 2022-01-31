///
/// macros.hpp
/// exam
///
/// Purpose:
/// Define test utility macros
///

#ifndef PKG_EXAM_MACROS_HPP
#define PKG_EXAM_MACROS_HPP

#include <limits>
#include <type_traits>

#include "gtest/gtest.h"

#include "fmts/fmts.hpp"

#include "types/types.hpp"

#include "exam/mock_log.hpp"
#include "exam/nosupport_log.hpp"

namespace exam
{

struct TestException final : public std::exception
{
	TestException (const std::string& msg) : msg_(msg) {}

	const char* what () const throw ()
	{
		return msg_.c_str();
	}

	std::string msg_;
};

template <typename T, typename std::enable_if<!types::is_complex<T>::value>::type* = nullptr>
inline T relative_error (const T& l, const T& r)
{
	return std::abs(l - r) / (
		std::max(std::abs(l), std::abs(r)) +
		std::numeric_limits<T>::epsilon()
	);
}

template <typename T>
inline T relative_error (const std::complex<T>& l, const std::complex<T>& r)
{
	std::complex<T> out = std::abs(l - r) / (
		std::max(std::abs(l), std::abs(r)) +
		std::numeric_limits<T>::epsilon()
	);
	return out.real();
}

#define _ARRCHECK(ARR, ARR2, GBOOL) {\
	GBOOL(std::equal(ARR.begin(), ARR.end(), ARR2.begin())) <<\
		"expect list " << fmts::readable(ARR) <<\
		", got " << fmts::readable(ARR2) << " instead"; }
#define _VECCHECK(VEC, VEC2, GBOOL) {\
	GBOOL(VEC.size() == VEC2.size() &&\
		std::equal(VEC.begin(), VEC.end(), VEC2.begin())) <<\
		"expect list " << fmts::readable(VEC) <<\
		", got " << fmts::readable(VEC2) << " instead"; }
#define _INSET(SET, CONTENT, GBOOL, PREFIX_MSG) {\
	GBOOL(SET.end() != SET.find(CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #SET; }
#define _INARR(ARR, CONTENT, GBOOL, PREFIX_MSG) { auto et = ARR.end();\
	GBOOL(et != std::find(ARR.begin(), et, CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #ARR; }

#define EXPECT_STRHAS(SUPER, SUB) EXPECT_THAT(SUPER.c_str(), ::testing::HasSubstr(SUB))
#define EXPECT_STRHASNOT(SUPER, SUB) EXPECT_THAT(SUPER.c_str(), Not(::testing::HasSubstr(SUB)))

#define ASSERT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_TRUE)
#define EXPECT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_TRUE)
#define ASSERT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_FALSE)
#define EXPECT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_FALSE)

#define ASSERT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_TRUE)
#define EXPECT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_TRUE)
#define ASSERT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_FALSE)
#define EXPECT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_FALSE)

#define ASSERT_HAS(SET, KEY) _INSET(SET, KEY, ASSERT_TRUE, "expecting to")
#define EXPECT_HAS(SET, KEY) _INSET(SET, KEY, EXPECT_TRUE, "expecting to")
#define ASSERT_HASNOT(SET, KEY) _INSET(SET, KEY, ASSERT_FALSE, "cannot")
#define EXPECT_HASNOT(SET, KEY) _INSET(SET, KEY, EXPECT_FALSE, "cannot")

#define ASSERT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_TRUE, "expecting to")
#define EXPECT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_TRUE, "expecting to")
#define ASSERT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_FALSE, "cannot")
#define EXPECT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_FALSE, "cannot")

#define EXPECT_FATAL(EVENT, MSG) try { EVENT; FAIL() <<\
	"did not expect " << #EVENT << " to succeed"; }\
	catch (exam::TestException& e) { EXPECT_STREQ(MSG, e.what()); }\
	catch (std::exception& e) { FAIL() << "unexpected throw " << e.what(); }

#define EXPECT_REL_CLOSE(L, R, ERR) EXPECT_LT(exam::relative_error(L, R), ERR)

#define EXPECT_NOT_REL_CLOSE(L, R, ERR) EXPECT_GE(exam::relative_error(L, R), ERR)

#define EXPECT_CLOSE(L, R) EXPECT_REL_CLOSE(L, R, 0.02)

#define EXPECT_NOT_CLOSE(L, R) EXPECT_NOT_REL_CLOSE(L, R, 0.02)

}

#endif // PKG_EXAM_HPP
