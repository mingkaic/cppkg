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
	f.add_flags()
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
	std::string arg;
	f.add_arg("arg", flag::opt::value<std::string>(&arg),
		"first argument", 1);

	char* args[] = {"program", "--help"};

	std::string expectmsg =
		"usage: program [flags] arg\n"
		"program flags:\n"
		"  --string arg (=default string)    string description\n"
		"  --ints arg (=2134)                ints description\n"
		"  --doubs arg (=33.722940000000001) doubs description\n"
		"  --bools arg (=1)                  bools description\n"
		"  --switch                          switch description\n"
		"  --help                            Display help message\n";
	EXPECT_FALSE(f.parse(2, args));
	EXPECT_STREQ(expectmsg.c_str(), TestLogger::latest_warning_.c_str());

	// unlimited args
	flag::FlagSet f2("program");
	f2.add_arg("arg", flag::opt::value<std::string>(&arg),
		"first argument", -1);

	std::string expectmsg2 =
		"usage: program [flags] arg [arg...]\n"
		"program flags:\n"
		"  --help                Display help message\n";
	EXPECT_FALSE(f2.parse(2, args));
	EXPECT_STREQ(expectmsg2.c_str(), TestLogger::latest_warning_.c_str());
}


TEST_F(FLAG, Parse)
{
	std::string str;
	int i;
	double d;
	bool b;
	bool s;

	flag::FlagSet f("program");
	f.add_flags()
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

	std::string arg;
	f.add_arg("arg", flag::opt::value<std::string>(&arg)->required(),
		"first argument", 1);

	char* args[] = {"program", "argument value"};
	ASSERT_TRUE(f.parse(2, args));
	EXPECT_STREQ("argument value", arg.c_str());
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
		"--switch", "argument value2"};
	ASSERT_TRUE(f.parse(11, args2));
	EXPECT_STREQ("argument value2", arg.c_str());
	EXPECT_STREQ("special string", str.c_str());
	EXPECT_EQ(4567, i);
	EXPECT_DOUBLE_EQ(9.7, d);
	EXPECT_EQ(false, b);
	EXPECT_EQ(true, s);

	char* badargs[] = {"program"};
	ASSERT_FALSE(f.parse(1, badargs));
	EXPECT_STREQ("the option '--arg' is required but missing", TestLogger::latest_error_.c_str());

	char* badargs2[] = {"program", "--whoami", "argument value"};
	ASSERT_FALSE(f.parse(3, badargs2));
	EXPECT_STREQ("unrecognised option '--whoami'",
		TestLogger::latest_error_.c_str());
}


#endif // DISABLE_FLAG_TEST
