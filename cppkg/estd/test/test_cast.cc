
#ifndef DISABLE_ESTD_CAST_TEST

#include "gtest/gtest.h"

#include "verum/exam/macros.h"

#include "cppkg/estd/cast.h"


using ::testing::_;
using ::testing::Return;
using ::testing::Throw;


using namespace verum::exam;


using namespace cisab::fmts;
using namespace cisab::logs;


using namespace cppkg::estd;


struct iBase
{
	virtual ~iBase (void) = default;

	virtual int foo (void) const = 0;
};


struct Child1 final : public iBase
{
	int foo (void) const override
	{
		return 1;
	}
};


struct Child2 final : public iBase
{
	int foo (void) const override
	{
		return 2;
	}
};


TEST(CAST, MustCast)
{
	auto& logger = static_cast<MockLogger&>(get_logger());

	std::shared_ptr<iBase> n;
	std::shared_ptr<iBase> b = std::make_shared<Child1>();

	std::string basename = typeid(iBase).name();
	std::string c1name = typeid(Child1).name();
	std::string c2name = typeid(Child2).name();
	std::string expect_msg1 = sprintf("cannot cast null %s to %s", basename.c_str(), c1name.c_str());
	std::string expect_msg2 = sprintf("cannot cast null %s to %s", basename.c_str(), c2name.c_str());
	EXPECT_CALL(logger, log(FATAL, expect_msg1, _)).Times(1).WillOnce(Throw(TestException(expect_msg1)));
	EXPECT_CALL(logger, log(FATAL, expect_msg2, _)).Times(1).WillOnce(Throw(TestException(expect_msg2)));
	EXPECT_FATAL(must_cast<Child1>(n.get()), expect_msg1.c_str());
	EXPECT_FATAL(must_cast<Child2>(n.get()), expect_msg2.c_str());

	std::string expect_msg3 = sprintf("failed to cast %s to %s", basename.c_str(), c2name.c_str());
	auto c = must_cast<Child1>(b.get());
	ASSERT_NE(nullptr, c);
	EXPECT_CALL(logger, log(FATAL, expect_msg3, _)).Times(1).WillOnce(Throw(TestException(expect_msg3)));
	EXPECT_FATAL(must_cast<Child2>(b.get()), expect_msg3.c_str());
}


TEST(CAST, MustPtrCast)
{
	auto& logger = static_cast<MockLogger&>(get_logger());

	std::shared_ptr<iBase> n;
	std::shared_ptr<iBase> b = std::make_shared<Child1>();

	std::string basename = typeid(iBase).name();
	std::string c1name = typeid(Child1).name();
	std::string c2name = typeid(Child2).name();
	std::string expect_msg1 = sprintf("cannot cast null %s to %s", basename.c_str(), c1name.c_str());
	std::string expect_msg2 = sprintf("cannot cast null %s to %s", basename.c_str(), c2name.c_str());
	EXPECT_CALL(logger, log(FATAL, expect_msg1, _)).Times(1).WillOnce(Throw(TestException(expect_msg1)));
	EXPECT_CALL(logger, log(FATAL, expect_msg2, _)).Times(1).WillOnce(Throw(TestException(expect_msg2)));
	EXPECT_FATAL(must_ptr_cast<Child1>(n), expect_msg1.c_str());
	EXPECT_FATAL(must_ptr_cast<Child2>(n), expect_msg2.c_str());

	std::string expect_msg3 = sprintf("failed to cast %s to %s", basename.c_str(), c2name.c_str());
	auto c = must_ptr_cast<Child1>(b);
	ASSERT_NE(nullptr, c);
	EXPECT_CALL(logger, log(FATAL, expect_msg3, _)).Times(1).WillOnce(Throw(TestException(expect_msg3)));
	EXPECT_FATAL(must_ptr_cast<Child2>(b), expect_msg3.c_str());
}


#endif // DISABLE_ESTD_CAST_TEST
