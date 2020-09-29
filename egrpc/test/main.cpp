#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "egrpc/egrpc.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_EGRPC_TEST


struct MockRequest
{};


struct MockResponse
{};


TEST(ASYNC, ClientRequestSuccess)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	bool cb_called = false;
	using MockHandlerT = egrpc::AsyncClientHandler<MockRequest,MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&cb_called](MockResponse& res)
	{
		cb_called = true;
	},
	[](MockRequest& req, MockHandlerT* handler)
	{
		handler->status_ = grpc::Status(grpc::OK, "ok");
	}, 3);

	EXPECT_FALSE(cb_called);

	ptr->handle(true);

	EXPECT_TRUE(cb_called);

	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("call %p completed successfully", ptr);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());
}


TEST(ASYNC, ClientRequestFail)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	bool cb_called = false;
	using MockHandlerT = egrpc::AsyncClientHandler<MockRequest,MockResponse>;
	MockHandlerT* latest_handler = nullptr;
	size_t num_init = 0;
	new MockHandlerT(promise, logger,
	[&cb_called](MockResponse& res)
	{
		cb_called = true;
	},
	[&latest_handler, &num_init](MockRequest& req, MockHandlerT* handler)
	{
		handler->status_ = grpc::Status(grpc::INTERNAL, "error");
		latest_handler = handler;
		++num_init;
		handler->handle(true);
	}, 3);

	EXPECT_FALSE(cb_called);
	EXPECT_EQ(4, num_init);

	EXPECT_EQ(logs::ERROR, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("call %p failed: error", latest_handler);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());
}


TEST(ASYNC, ClientStream)
{
	//
}


TEST(ASYNC, ServerRequest)
{
	//
}


TEST(ASYNC, ServerStream)
{
	//
}


TEST(CLIENT, Context)
{
	//
}


#endif // DISABLE_EGRPC_TEST
