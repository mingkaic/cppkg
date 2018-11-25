#include "gtest/gtest.h"

#include "logs/logs.hpp"


struct TestLogger : public logs::iLogger
{
	static std::string latest_warning_;
	static std::string latest_error_;
	static std::string latest_fatal_;

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
		latest_fatal_ = msg;
	}
};


std::string TestLogger::latest_warning_;

std::string TestLogger::latest_error_;

std::string TestLogger::latest_fatal_;

std::shared_ptr<TestLogger> tlogger = std::make_shared<TestLogger>();


int main (int argc, char** argv)
{
	logs::set_logger(std::static_pointer_cast<logs::iLogger>(tlogger));
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_LOGS_TEST


struct LOGS : public ::testing::Test
{
protected:
	void TearDown (void) override
	{
		TestLogger::latest_warning_ = "";
		TestLogger::latest_error_ = "";
		TestLogger::latest_fatal_ = "";
	}
};


TEST_F(LOGS, Default)
{
	logs::DefLogger log;
	log.warn("warning message");
	log.error("error message");
	try
	{
		log.fatal("fatal message");
		FAIL() << "log.fatal failed to throw error";
	}
	catch (std::runtime_error& e)
	{
		const char* msg = e.what();
		EXPECT_STREQ("fatal message", msg);
	}
	catch (...)
	{
		FAIL() << "expected to throw runtime_error";
	}
}


TEST_F(LOGS, Warn)
{
	logs::warn("warning message");
	const char* cmsg = TestLogger::latest_warning_.c_str();
	EXPECT_STREQ("warning message", cmsg);
}


TEST_F(LOGS, WarnFmt)
{
	logs::warnf("warning %.2f message %d with format %s", 4.15, 33,
		"applepie");
	const char* cmsg = TestLogger::latest_warning_.c_str();
	EXPECT_STREQ("warning 4.15 message 33 with format applepie", cmsg);
}


TEST_F(LOGS, Error)
{
	logs::error("erroring message");
	const char* emsg = TestLogger::latest_error_.c_str();
	EXPECT_STREQ("erroring message", emsg);
}


TEST_F(LOGS, ErrorFmt)
{
	logs::errorf("erroring %.3f message %d with format %s", 0.31, 7, "orange");
	const char* emsg = TestLogger::latest_error_.c_str();
	EXPECT_STREQ("erroring 0.310 message 7 with format orange", emsg);
}


TEST_F(LOGS, Fatal)
{
	logs::fatal("fatal message");
	const char* fmsg = TestLogger::latest_fatal_.c_str();
	EXPECT_STREQ("fatal message", fmsg);
}


TEST_F(LOGS, FatalFmt)
{
	logs::fatalf("fatal %.4f message %d with format %s", 3.1415967, -1,
		"plum");
	const char* fmsg = TestLogger::latest_fatal_.c_str();
	EXPECT_STREQ("fatal 3.1416 message -1 with format plum", fmsg);
}


#endif // DISABLE_LOGS_TEST
