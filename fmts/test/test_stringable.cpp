
#ifndef DISABLE_FMTS_STRINGABLE_TEST

#include <array>
#include <list>
#include <vector>
#include <unordered_map>

#include "gtest/gtest.h"

#include "gmock/gmock.h"

#include "fmts/fmts.hpp"


using ::testing::Return;


struct MockStringable : public fmts::iStringable
{
	virtual ~MockStringable (void) = default;

	MOCK_CONST_METHOD0(to_string, std::string(void));
};


static const std::string nil = "<nil>";


TEST(STRINGABLE, NullRawStream)
{
	fmts::iStringable* shared = nullptr;

	std::stringstream ss;
	ss << shared;
	EXPECT_STREQ(nil.c_str(), ss.str().c_str());
}


TEST(STRINGABLE, NullSharedStream)
{
	std::shared_ptr<fmts::iStringable> shared = nullptr;

	std::stringstream ss;
	ss << shared;
	EXPECT_STREQ(nil.c_str(), ss.str().c_str());
}


TEST(STRINGABLE, ReferenceStream)
{
	MockStringable str;
	std::stringstream ss;

	EXPECT_CALL(str, to_string()).Times(1).WillOnce(Return("abcdefg"));
	ss << str;
	EXPECT_STREQ("abcdefg", ss.str().c_str());
}


TEST(STRINGABLE, RawPointerStream)
{
	MockStringable str;
	std::stringstream ss;

	EXPECT_CALL(str, to_string()).Times(1).WillOnce(Return("hijklm"));
	ss << &str;
	EXPECT_STREQ("hijklm", ss.str().c_str());
}


TEST(STRINGABLE, SharedPointerStream)
{
	auto str = std::make_shared<MockStringable>();
	std::stringstream ss;

	EXPECT_CALL(*str, to_string()).Times(1).WillOnce(Return("nopqrst"));
	ss << str;
	EXPECT_STREQ("nopqrst", ss.str().c_str());
}


#endif // DISABLE_FMTS_STRINGABLE_TEST
