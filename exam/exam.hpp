///
/// exam.hpp
/// exam
///
/// Purpose:
/// Define test utility functions
///

#ifndef PKG_EXAM_HPP
#define PKG_EXAM_HPP

#include "logs/logs.hpp"

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

/// Test Logger to assign in order to use
/// EXPECT_FATAL, EXPECT_ERROR, or EXPECT_WARN
struct TestLogger : public logs::iLogger
{
	/// Latest message logged
	static std::string latest_msg_;

	/// Latest log level value
	static size_t latest_lvl_;

	static std::string set_llevel;

	static std::string get_llevel;

	/// Returns logs::TRACE
	std::string get_log_level (void) const override
	{
		return get_llevel;
	}

	/// Quitely ignores set level, since test logger should record every message
	void set_log_level (const std::string& log_level) override
	{
		// does actually set anything
		set_llevel = log_level;
	}

	bool supports_level (size_t msg_level) const override
	{
		return msg_level < logs::NOT_SET;
	}

	bool supports_level (const std::string& msg_level) const override
	{
		return logs::enum_log(msg_level) < logs::NOT_SET;
	}

	void log (const std::string& msg_level, const std::string& msg,
		const logs::SrcLocT& location = logs::SrcLocT::current()) override
	{
		log(logs::enum_log(msg_level), msg);
	}

	/// Log both level and message
	void log (size_t log_level, const std::string& msg,
		const logs::SrcLocT& location = logs::SrcLocT::current()) override
	{
		if (log_level <= logs::THROW_ERR)
		{
			fatal(msg);
		}
		latest_lvl_ = log_level;
		latest_msg_ = msg;
	}

	/// Logs message at log::WARN
	void warn (const std::string& msg) const
	{
		latest_lvl_ = logs::WARN;
		latest_msg_ = msg;
	}

	/// Logs message at log::ERROR
	void error (const std::string& msg) const
	{
		latest_lvl_ = logs::ERROR;
		latest_msg_ = msg;
	}

	/// Logs message at log::FATAL
	void fatal (const std::string& msg) const
	{
		throw TestException(msg);
	}
};

/// Global test logger
extern std::shared_ptr<TestLogger> tlogger;


#define _LOG(EVENT, MSG, LEVEL) EVENT;\
	EXPECT_STREQ(MSG, exam::TestLogger::latest_msg_.c_str()) <<\
		"failed to elicit " << MSG << " from " << #EVENT <<\
		", instead got " << exam::TestLogger::latest_msg_.c_str();\
	EXPECT_EQ(LEVEL, exam::TestLogger::latest_lvl_) <<\
		"failed to log at level " << #LEVEL << " from " << #EVENT <<\
		", instead got level " << exam::TestLogger::latest_lvl_;\
	exam::TestLogger::latest_msg_ = "";\
	exam::TestLogger::latest_lvl_ = logs::FATAL;
#define _ARRCHECK(ARR, ARR2, GBOOL) { std::stringstream arrs, arrs2;\
	fmts::to_stream(arrs, ARR.begin(), ARR.end());\
	fmts::to_stream(arrs2, ARR2.begin(), ARR2.end());\
	GBOOL(std::equal(ARR.begin(), ARR.end(), ARR2.begin())) <<\
		"expect list " << arrs.str() << ", got " << arrs2.str() << " instead"; }
#define _VECCHECK(VEC, VEC2, GBOOL) { std::stringstream vecs, vecs2;\
	fmts::to_stream(vecs, VEC.begin(), VEC.end());\
	fmts::to_stream(vecs2, VEC2.begin(), VEC2.end());\
	GBOOL(VEC.size() == VEC2.size() &&\
		std::equal(VEC.begin(), VEC.end(), VEC2.begin())) <<\
		"expect list " << vecs.str() << ", got " << vecs2.str() << " instead"; }
#define _INSET(SET, CONTENT, GBOOL, PREFIX_MSG) {\
	GBOOL(SET.end() != SET.find(CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #SET; }
#define _INARR(ARR, CONTENT, GBOOL, PREFIX_MSG) { auto et = ARR.end();\
	GBOOL(et != std::find(ARR.begin(), et, CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #ARR; }

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
#define EXPECT_ERROR(EVENT, MSG) _LOG(EVENT, MSG, logs::ERROR)
#define EXPECT_WARN(EVENT, MSG) _LOG(EVENT, MSG, logs::WARN)
#define EXPECT_INFO(EVENT, MSG) _LOG(EVENT, MSG, logs::INFO)
#define EXPECT_DEBUG(EVENT, MSG) _LOG(EVENT, MSG, logs::DEBUG)
#define EXPECT_TRACE(EVENT, MSG) _LOG(EVENT, MSG, logs::TRACE)

}

#endif // PKG_EXAM_HPP
