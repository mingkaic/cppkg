#include "gtest/gtest.h"

#include "flag/flag.hpp"


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


#ifndef DISABLE_FLAG_TEST


struct FLAG : public ::testing::Test
{
protected:
	void TearDown (void) override
	{
		TestLogger::latest_warning_ = "";
		TestLogger::latest_error_ = "";
		TestLogger::latest_fatal_ = "";
	}
};


TEST_F(FLAG, Help)
{
	flag::FlagSet f("program");

	std::string str;
	int i;
	double d;
	bool b;
	bool s;
	f.args_.add_options()
		("string", flag::opt::value<std::string>(&str)->
			default_value("default string"),
			"string description")
		("ints", flag::opt::value<int>(&i)->default_value(2134),
			"ints description")
		("doubs", flag::opt::value<double>(&d)->default_value(33.72294),
			"doubs description")
		("bools", flag::opt::value<bool>(&b)->default_value(true),
			"bools description")
		("switch", flag::opt::bool_switch(&s)->default_value(false),
			"switch description");

	char* args[] = {"program", "--help"};
	EXPECT_FALSE(f.parse(2, args));

	std::string expectmsg =
		"usage: program [options]\n"
		"program options:\n"
		"  --string arg (=default string)    string description\n"
		"  --ints arg (=2134)                ints description\n"
		"  --doubs arg (=33.722940000000001) doubs description\n"
		"  --bools arg (=1)                  bools description\n"
		"  --switch                          switch description\n"
		"  --help                            Display help message\n";
	EXPECT_STREQ(expectmsg.c_str(), TestLogger::latest_warning_.c_str());
}


TEST_F(FLAG, Parse)
{
	std::string str;
	int i;
	double d;
	bool b;
	bool s;

	flag::FlagSet f("program");
	f.args_.add_options()
		("string", flag::opt::value<std::string>(&str)->
			default_value("default string"),
			"string description")
		("ints", flag::opt::value<int>(&i)->default_value(2134),
			"ints description")
		("doubs", flag::opt::value<double>(&d)->default_value(33.72294),
			"doubs description")
		("bools", flag::opt::value<bool>(&b)->default_value(true),
			"bools description")
		("switch", flag::opt::bool_switch(&s)->default_value(false),
			"switch description");

	char* args[] = {"program"};
	ASSERT_TRUE(f.parse(1, args));
	EXPECT_STREQ("default string", str.c_str());
	EXPECT_EQ(2134, i);
	EXPECT_DOUBLE_EQ(33.72294, d);
	EXPECT_EQ(true, b);
	EXPECT_EQ(false, s);

	char* args2[] = {"program",
		"--string", "special string",
		"--ints", "4567",
		"--doubs", "9.7",
		"--bools", "false",
		"--switch"};
	ASSERT_TRUE(f.parse(10, args2));
	EXPECT_STREQ("special string", str.c_str());
	EXPECT_EQ(4567, i);
	EXPECT_DOUBLE_EQ(9.7, d);
	EXPECT_EQ(false, b);
	EXPECT_EQ(true, s);

	char* args3[] = {"program", "--whoami"};
	ASSERT_FALSE(f.parse(2, args3));
	EXPECT_STREQ("unrecognised option '--whoami'",
		TestLogger::latest_error_.c_str());
}


#endif // DISABLE_FLAG_TEST
