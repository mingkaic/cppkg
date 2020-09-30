#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "egrpc/egrpc.hpp"

#include "egrpc/test/test.grpc.pb.h"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_EGRPC_TEST


TEST(ASYNC, ClientRequestSuccess)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	bool cb_called = false;
	using MockHandlerT = egrpc::AsyncClientHandler<mock::MockRequest,mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&cb_called](mock::MockResponse& res)
	{
		cb_called = true;
	},
	[](mock::MockRequest& req, MockHandlerT* handler)
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
	using MockHandlerT = egrpc::AsyncClientHandler<mock::MockRequest,mock::MockResponse>;
	MockHandlerT* latest_handler = nullptr;
	size_t num_init = 0;
	new MockHandlerT(promise, logger,
	[&cb_called](mock::MockResponse& res)
	{
		cb_called = true;
	},
	[&latest_handler, &num_init](mock::MockRequest& req, MockHandlerT* handler)
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
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12345";
	auto logger = std::make_shared<exam::TestLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool serve_call = false;
	auto call = new egrpc::AsyncServerCall<mock::MockRequest,
	mock::MockResponse>(logger,
	[&last_tag, &num_calls, &mock_service](grpc::ServerContext* ctx,
		mock::MockRequest* req,
		grpc::ServerAsyncResponseWriter<mock::MockResponse>* writer,
		grpc::CompletionQueue* cq,
		grpc::ServerCompletionQueue* ccq, void* tag)
	{
		mock_service.RequestMockRPC(ctx, req, writer, cq, ccq, tag);
		last_tag = tag;
		++num_calls;
	},
	[&serve_call](const mock::MockRequest& req, mock::MockResponse& res)
	{
		serve_call = true;
		return grpc::Status{grpc::OK, "hello"};
	}, cq.get());

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(serve_call);

	std::thread(
	[address]()
	{
		auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
			grpc::InsecureChannelCredentials()));

		grpc::ClientContext ctx;
		mock::MockRequest req;
		mock::MockResponse res;
		stub->MockRPC(&ctx, req, &res);
	}).detach();

	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(serve_call);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg2 = fmts::sprintf("rpc %p completed", call);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	server->Shutdown();

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<egrpc::AsyncServerCall<
		mock::MockRequest,mock::MockResponse>*>(last_tag);
	tag_name->shutdown();

	cq->Shutdown();
}


TEST(ASYNC, ServerStream)
{
	//
}


TEST(CLIENT, Context)
{
	//
}


TEST(ASYNC, WaitUtility)
{
	//
}


#endif // DISABLE_EGRPC_TEST
