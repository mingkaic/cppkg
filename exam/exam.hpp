#include "logs/logs.hpp"

#include "fmts/fmts.hpp"

#ifndef PKG_EXAM_HPP
#define PKG_EXAM_HPP

namespace exam
{

struct TestLogger : public logs::iLogger
{
	static std::string latest_warning_;
	static std::string latest_error_;

	enum LOG_LEVEL
	{
		NO = 0,
		YES,
	};

	void log (size_t log_level, std::string msg) const override
	{
		if (log_level <= log_level_)
		{
			switch (log_level_)
			{
				case NO:
					warn(msg);
					break;
				case YES:
				default:
					std::cout << msg << '\n';
			}
		}
	}

	size_t get_log_level (void) const override
	{
		return log_level_;
	}

	void set_log_level (size_t log_level) override
	{
		log_level_ = (LOG_LEVEL) log_level;
	}

	void warn (std::string msg) const override
	{
		latest_warning_ = msg;
	}

	void error (std::string msg) const override
	{
		latest_error_ = msg;
	}

	void fatal (std::string msg) const override
	{
		throw std::runtime_error(msg);
	}

	LOG_LEVEL log_level_;
};

extern std::shared_ptr<TestLogger> tlogger;


#define _ARRCHECK(ARR, ARR2, GBOOL) { std::stringstream arrs, arrs2;\
	fmts::to_stream(arrs, ARR.begin(), ARR.end());\
	fmts::to_stream(arrs2, ARR2.begin(), ARR2.end());\
	GBOOL(std::equal(ARR.begin(), ARR.end(), ARR2.begin())) <<\
		"expect list " << arrs.str() << ", got " << arrs2.str() << " instead"; }
#define _INSET(SET, CONTENT, GBOOL, PREFIX_MSG) {\
	GBOOL(SET.end() != SET.find(CONTENT)) <<\
		PREFIX_MSG + " find " << #CONTENT << " in " << #SET; }
#define _INARR(ARR, CONTENT, GBOOL, PREFIX_MSG) { auto et = ARR.end();\
	GBOOL(et != std::find(ARR.begin(), et, CONTENT)) <<\
		PREFIX_MSG + " find " << #CONTENT << " in " << #ARR; }

#define ASSERT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_TRUE)
#define EXPECT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_TRUE)
#define ASSERT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_FALSE)
#define EXPECT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_FALSE)

#define ASSERT_HAS(SET, KEY) _INSET(SET, KEY, ASSERT_TRUE, "expecting to")
#define EXPECT_HAS(SET, KEY) _INSET(SET, KEY, EXPECT_TRUE, "expecting to")
#define ASSERT_HASNOT(SET, KEY) _INSET(SET, KEY, ASSERT_FALSE, "cannot")
#define EXPECT_HASNOT(SET, KEY) _INSET(SET, KEY, EXPECT_FALSE, "cannot")

#define ASSERT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_TRUE, "expecting to")
#define EXPECT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_TRUE, "expecting to")
#define ASSERT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_FALSE, "cannot")
#define EXPECT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_FALSE, "cannot")


#define EXPECT_FATAL(EVENT, MSG) try { EVENT; FAIL() << \
	"did not expect " << #EVENT << " to succeed"; } \
	catch (std::runtime_error& e) { EXPECT_STREQ(MSG, e.what()); }\
	catch (std::exception& e) { FAIL() << "unexpected throw " << e.what(); }
#define EXPECT_ERROR(EVENT, MSG) EVENT;\
	EXPECT_STREQ(MSG, exam::TestLogger::latest_error_.c_str()) << \
		"failed to elicit " << MSG << " warning from " << #EVENT;\
	exam::TestLogger::latest_error_ = "";
#define EXPECT_WARN(EVENT, MSG) EVENT;\
	EXPECT_STREQ(MSG, exam::TestLogger::latest_warning_.c_str()) << \
		"failed to elicit " << MSG << " error from " << #EVENT;\
	exam::TestLogger::latest_warning_ = "";

}

#endif // PKG_EXAM_HPP