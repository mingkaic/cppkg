#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "egrpc/egrpc.hpp"

#include "egrpc/test/test.grpc.pb.h"


int main (int argc, char** argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	google::protobuf::ShutdownProtobufLibrary();
	return ret;
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

	bool promise_cb_called = false;
	egrpc::wait_for(*promise,
	[&promise_cb_called](error::ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);
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

	bool promise_cb_called = false;
	error::ErrptrT got_err = nullptr;
	egrpc::wait_for(*promise,
	[&got_err, &promise_cb_called](error::ErrptrT err)
	{
		got_err = err;
		promise_cb_called = true;
	});
	EXPECT_TRUE(promise_cb_called);
	ASSERT_NE(nullptr, got_err);
	EXPECT_STREQ("error", got_err->to_string().c_str());
}


struct MockServer final : public mock::MockService::Service
{
	MockServer (size_t nentries = 10, grpc::Status status = grpc::Status::OK) :
		nentries_(nentries), status_(status) {}

	grpc::Status MockRPC (grpc::ServerContext* context,
		const mock::MockRequest* req,
		mock::MockResponse* res)
	{
		return status_;
	}

	grpc::Status MockStreamOut (grpc::ServerContext* context,
		const mock::MockRequest* req,
		grpc::ServerWriter<mock::MockResponse>* writer)
	{
		for (size_t i = 0; i < nentries_; ++i)
		{
			mock::MockResponse res;
			writer->Write(res);
		}
		return status_;
	}

	size_t nentries_;

	grpc::Status status_;
};


TEST(ASYNC, ClientStreamSuccess)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	std::string address = "0.0.0.0:12333";

	MockServer service(2);
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = egrpc::AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	grpc::CompletionQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(true);

	EXPECT_EQ(0, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("call %p created... processing", ptr);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(1, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg2 = fmts::sprintf("call %p received... handling", ptr);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(2, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(false);
	EXPECT_EQ(2, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg3 = fmts::sprintf("call %p received... finishing", ptr);
	EXPECT_STREQ(expect_msg3.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg4 = fmts::sprintf("call %p completed successfully", ptr);
	EXPECT_STREQ(expect_msg4.c_str(), logger->latest_msg_.c_str());

	bool promise_cb_called = false;
	egrpc::wait_for(*promise,
	[&promise_cb_called](error::ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);

	server->Shutdown();
}


TEST(ASYNC, ClientStreamEarlyStop)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	std::string address = "0.0.0.0:12334";

	MockServer service(0);
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = egrpc::AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	grpc::CompletionQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(false);
	EXPECT_EQ(0, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("call %p created... finishing", ptr);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg2 = fmts::sprintf("call %p completed successfully", ptr);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	bool promise_cb_called = false;
	egrpc::wait_for(*promise,
	[&promise_cb_called](error::ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);

	server->Shutdown();
}


TEST(ASYNC, ClientStreamFail)
{
	auto promise = std::make_shared<egrpc::ErrPromiseT>();
	auto logger = std::make_shared<exam::TestLogger>();

	std::string address = "0.0.0.0:12335";

	MockServer service(2, grpc::Status{grpc::INTERNAL, "oh no"});
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = egrpc::AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	grpc::CompletionQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(true);

	EXPECT_EQ(0, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("call %p created... processing", ptr);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(1, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg2 = fmts::sprintf("call %p received... handling", ptr);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(2, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(false);
	EXPECT_EQ(2, num_cb_called);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg3 = fmts::sprintf("call %p received... finishing", ptr);
	EXPECT_STREQ(expect_msg3.c_str(), logger->latest_msg_.c_str());

	EXPECT_TRUE(cq.Next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(logs::ERROR, logger->latest_lvl_);
	auto expect_msg4 = fmts::sprintf("call %p failed: oh no", ptr);
	EXPECT_STREQ(expect_msg4.c_str(), logger->latest_msg_.c_str());

	bool promise_cb_called = false;
	error::ErrptrT got_err = nullptr;
	egrpc::wait_for(*promise,
	[&got_err, &promise_cb_called](error::ErrptrT err)
	{
		got_err = err;
		promise_cb_called = true;
	});
	EXPECT_TRUE(promise_cb_called);
	ASSERT_NE(nullptr, got_err);
	EXPECT_STREQ("oh no", got_err->to_string().c_str());

	server->Shutdown();
}


TEST(ASYNC, MultiWaitFor)
{
	auto promise1 = std::make_shared<egrpc::ErrPromiseT>();
	auto promise2 = std::make_shared<egrpc::ErrPromiseT>();
	auto promise3 = std::make_shared<egrpc::ErrPromiseT>();

	promise1->set_value(error::error("hello"));
	promise2->set_value(nullptr);
	promise3->set_value(error::error("world"));

	egrpc::ErrPromisesT promises = {promise1, promise2, promise3};

	std::unordered_set<error::ErrptrT> errors;
	egrpc::wait_for(promises,
	[&errors](error::ErrptrT err)
	{
		errors.emplace(err);
	});
	ASSERT_EQ(2, errors.size());
	types::StrUSetT strs;
	for (auto err : errors)
	{
		ASSERT_NE(nullptr, err);
		strs.emplace(err->to_string());
	}
	EXPECT_HAS(strs, "hello");
	EXPECT_HAS(strs, "world");
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

	using ServerCallT = egrpc::AsyncServerCall<mock::MockRequest,mock::MockResponse>;
	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool serve_call = false;
	auto call = new ServerCallT(logger,
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
	auto tag_name = static_cast<egrpc::iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<ServerCallT*>(tag_name));
	tag_name->shutdown();

	cq->Shutdown();
}


TEST(ASYNC, ServerStream)
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12346";
	auto logger = std::make_shared<exam::TestLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool init_call = false;
	size_t num_iterators = 0;
	using StreamCallT = egrpc::AsyncServerStreamCall<mock::MockRequest,
		mock::MockResponse,std::vector<size_t>>;

	auto call = new StreamCallT(logger,
	[&last_tag, &num_calls, &mock_service](
		grpc::ServerContext* ctx,
		mock::MockRequest* req,
		grpc::ServerAsyncWriter<mock::MockResponse>* writer,
		grpc::CompletionQueue* cq,
		grpc::ServerCompletionQueue* ccq, void* tag)
	{
		mock_service.RequestMockStreamOut(ctx, req, writer, cq, ccq, tag);
		last_tag = tag;
		++num_calls;
	},
	[&init_call](std::vector<size_t>& vec, const mock::MockRequest& req)
	{
		init_call = true;
		vec = {1, 2, 3, 4};
		return grpc::Status{grpc::OK, "hello"};
	},
	[&num_iterators](const mock::MockRequest& req,
		std::vector<size_t>::iterator& it, mock::MockResponse& res)
	{
		++num_iterators;
		return (*it % 2);
	}, cq.get());

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(init_call);
	EXPECT_EQ(0, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg0 = fmts::sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg0.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	std::thread(
	[address]()
	{
		auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
			grpc::InsecureChannelCredentials()));

		grpc::ClientContext ctx;
		ctx.set_deadline(
			std::chrono::system_clock::now() + std::chrono::minutes(1));
		mock::MockRequest req;
		mock::MockResponse res;
		std::unique_ptr<grpc::ClientReader<mock::MockResponse>>
		reader(stub->MockStreamOut(&ctx, req));
		size_t nresponses = 0;
		while (reader->Read(&res))
		{
			++nresponses;
		}
		auto status = reader->Finish();
		EXPECT_EQ(2, nresponses);
		EXPECT_TRUE(status.ok());
	}).detach();

	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(1, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(2, num_calls);
	EXPECT_EQ(3, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(2, num_calls);
	EXPECT_EQ(4, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
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
	auto tag_name = static_cast<egrpc::iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<StreamCallT*>(tag_name));
	tag_name->shutdown();

	cq->Shutdown();
}


TEST(ASYNC, ServerStreamStartupError)
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12347";
	auto logger = std::make_shared<exam::TestLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool init_call = false;
	size_t num_iterators = 0;
	using StreamCallT = egrpc::AsyncServerStreamCall<mock::MockRequest,
		mock::MockResponse,std::vector<size_t>>;

	auto call = new StreamCallT(logger,
	[&last_tag, &num_calls, &mock_service](grpc::ServerContext* ctx,
		mock::MockRequest* req,
		grpc::ServerAsyncWriter<mock::MockResponse>* writer,
		grpc::CompletionQueue* cq,
		grpc::ServerCompletionQueue* ccq, void* tag)
	{
		mock_service.RequestMockStreamOut(ctx, req, writer, cq, ccq, tag);
		last_tag = tag;
		++num_calls;
	},
	[&init_call](std::vector<size_t>& vec, const mock::MockRequest& req)
	{
		init_call = true;
		vec = {1, 2, 3, 4};
		return grpc::Status{grpc::INTERNAL, "what"};
	},
	[&num_iterators](const mock::MockRequest& req,
		std::vector<size_t>::iterator& it, mock::MockResponse& res)
	{
		++num_iterators;
		return true;
	}, cq.get());

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(init_call);
	EXPECT_EQ(0, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg0 = fmts::sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg0.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	std::thread(
	[address]()
	{
		auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
			grpc::InsecureChannelCredentials()));

		grpc::ClientContext ctx;
		ctx.set_deadline(
			std::chrono::system_clock::now() + std::chrono::minutes(1));
		mock::MockRequest req;
		mock::MockResponse res;
		std::unique_ptr<grpc::ClientReader<mock::MockResponse>>
		reader(stub->MockStreamOut(&ctx, req));
		EXPECT_FALSE(reader->Read(&res));
		auto status = reader->Finish();
		EXPECT_EQ(grpc::INTERNAL, status.error_code());
		EXPECT_STREQ("what", status.error_message().c_str());
	}).detach();

	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(0, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg = fmts::sprintf("rpc %p initializing", call);
	EXPECT_STREQ(expect_msg.c_str(), logger->latest_msg_.c_str());

	logger->latest_lvl_ = logs::WARN;

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(0, num_iterators);
	EXPECT_EQ(logs::INFO, logger->latest_lvl_);
	auto expect_msg2 = fmts::sprintf("rpc %p completed", call);
	EXPECT_STREQ(expect_msg2.c_str(), logger->latest_msg_.c_str());

	server->Shutdown();

	EXPECT_TRUE(cq->Next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<egrpc::iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<StreamCallT*>(tag_name));
	tag_name->shutdown();

	cq->Shutdown();
}


struct MockGrpcClient final : public egrpc::GrpcClient
{
	MockGrpcClient (const egrpc::ClientConfig& cfg) : GrpcClient(cfg) {}

	void public_ctx (grpc::ClientContext& ctx, bool is_request)
	{
		build_ctx(ctx, is_request);
	}
};


TEST(CLIENT, Context)
{
	egrpc::ClientConfig cfg(std::chrono::milliseconds(420), std::chrono::milliseconds(42000), 4);
	MockGrpcClient client(cfg);

	grpc::ClientContext ctx;
	client.public_ctx(ctx, true);
	auto when = ctx.deadline();

	// expect within next 1 second
	EXPECT_GT(std::chrono::system_clock::now() + std::chrono::seconds(1), when);

	grpc::ClientContext ctx2;
	client.public_ctx(ctx2, false);
	auto when2 = ctx2.deadline();

	// expect within next 1 minute
	EXPECT_GT(std::chrono::system_clock::now() + std::chrono::minutes(1), when2);
}


#endif // DISABLE_EGRPC_TEST
