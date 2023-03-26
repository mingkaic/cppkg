#include <regex>

#include "gtest/gtest.h"

#include "verum/exam/macros.h"

#include "cppkg/estd/cast.h"

#include "cppkg/egrpc/client_async.h"
#include "cppkg/egrpc/client_async_stream.h"
#include "cppkg/egrpc/iclient.h"
#include "cppkg/egrpc/iclient_async.h"
#include "cppkg/egrpc/icqueue.h"
#include "cppkg/egrpc/iserver_async.h"
#include "cppkg/egrpc/server_async.h"
#include "cppkg/egrpc/server_async_stream.h"

#include "cppkg/egrpc/pbtest/test.grpc.pb.h"


using ::testing::_;
using ::testing::SaveArg;
using ::testing::Return;


using namespace verum::exam;


using namespace cisab::fmts;
using namespace cisab::logs;
using namespace cisab::types;


using namespace cppkg::estd;
using namespace cppkg::egrpc;
using namespace cppkg::error;


int main (int argc, char** argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	google::protobuf::ShutdownProtobufLibrary();
	return ret;
}


#ifndef DISABLE_EGRPC_TEST


TEST(GRPC_EXT, GrpcCQueue)
{
	GrpcCQueue cq;
	cq.shutdown();
	auto realq = cq.get_cq();
	ASSERT_NE(nullptr, realq);
	void* tag;
	bool ok;
	EXPECT_FALSE(realq->Next(&tag, &ok));
}


TEST(ASYNC, ClientRequestSuccess)
{
	auto promise = std::make_shared<ErrPromiseT>();
	auto logger = std::make_shared<MockLogger>();

	bool cb_called = false;
	using MockHandlerT = AsyncClientHandler<mock::MockRequest,mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&cb_called](mock::MockResponse& res)
	{
		cb_called = true;
	},
	[](mock::MockRequest& req, MockHandlerT* handler)
	{
		handler->status_ = grpc::Status(grpc::OK, "ok");
	}, 3);

	auto expect_msg = sprintf("call %p completed successfully", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg, _)).Times(1);

	EXPECT_FALSE(cb_called);

	ptr->handle(true);

	EXPECT_TRUE(cb_called);

	bool promise_cb_called = false;
	wait_for(*promise,
	[&promise_cb_called](ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);
}


TEST(ASYNC, ClientRequestFail)
{
	auto promise = std::make_shared<ErrPromiseT>();
	auto logger = std::make_shared<MockLogger>();
	size_t nretries = 3;

	std::string firstmsg;
	std::string lastmsg;
	EXPECT_CALL(*logger, log(error_level, _, _)).Times(nretries + 1).
		WillOnce(SaveArg<1>(&firstmsg)).
		WillOnce(Return()).
		WillOnce(Return()).
		WillOnce(SaveArg<1>(&lastmsg));

	bool cb_called = false;
	using MockHandlerT = AsyncClientHandler<mock::MockRequest,mock::MockResponse>;
	MockHandlerT* latest_handler = nullptr;
	size_t num_init = 0;
	auto first_handler = new MockHandlerT(promise, logger,
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
	}, nretries);

	EXPECT_FALSE(cb_called);
	EXPECT_EQ(4, num_init);

	auto expect_msg = sprintf("call %p (3 attempts remaining) failed: error", first_handler);
	EXPECT_STREQ(expect_msg.c_str(), firstmsg.c_str());

	auto expect_msg2 = sprintf("call %p failed: error", latest_handler);
	EXPECT_STREQ(expect_msg2.c_str(), lastmsg.c_str());

	bool promise_cb_called = false;
	ErrptrT got_err = nullptr;
	wait_for(*promise,
	[&got_err, &promise_cb_called](ErrptrT err)
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
	auto promise = std::make_shared<ErrPromiseT>();
	auto logger = std::make_shared<MockLogger>();

	std::string address = "0.0.0.0:12333";

	MockServer service(2);
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	GrpcCQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq.cq_);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	auto expect_msg1 = sprintf("call %p created... processing", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg1, _)).Times(1);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(true);
	EXPECT_EQ(0, num_cb_called);

	auto expect_msg2 = sprintf("call %p received... handling", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg2, _)).Times(2);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(1, num_cb_called);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(2, num_cb_called);

	auto expect_msg3 = sprintf("call %p received... finishing", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg3, _)).Times(1);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(false);
	EXPECT_EQ(2, num_cb_called);

	auto expect_msg4 = sprintf("call %p completed successfully", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg4, _)).Times(1);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);

	bool promise_cb_called = false;
	wait_for(*promise,
	[&promise_cb_called](ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);

	server->Shutdown();
}


TEST(ASYNC, ClientStreamEarlyStop)
{
	auto promise = std::make_shared<ErrPromiseT>();
	auto logger = std::make_shared<MockLogger>();

	std::string address = "0.0.0.0:12334";

	MockServer service(0);
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	GrpcCQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq.cq_);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	auto expect_msg = sprintf("call %p created... finishing", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg, _)).Times(1);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(false);
	EXPECT_EQ(0, num_cb_called);

	auto expect_msg2 = sprintf("call %p completed successfully", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg2, _)).Times(1);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);

	bool promise_cb_called = false;
	wait_for(*promise,
	[&promise_cb_called](ErrptrT err)
	{
		promise_cb_called = true;
	});
	EXPECT_FALSE(promise_cb_called);

	server->Shutdown();
}


TEST(ASYNC, ClientStreamFail)
{
	auto promise = std::make_shared<ErrPromiseT>();
	auto logger = std::make_shared<MockLogger>();

	std::string address = "0.0.0.0:12335";

	MockServer service(2, grpc::Status{grpc::INTERNAL, "oh no"});
	grpc::ServerBuilder builder;
	builder.AddListeningPort(address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
		grpc::InsecureChannelCredentials()));

	size_t num_cb_called = 0;
	using MockHandlerT = AsyncClientStreamHandler<mock::MockResponse>;
	auto ptr = new MockHandlerT(promise, logger,
	[&num_cb_called](mock::MockResponse& res)
	{
		++num_cb_called;
	});

	mock::MockRequest req;
	GrpcCQueue cq;
	grpc::ClientContext ctx;
	ptr->reader_ = stub->PrepareAsyncMockStreamOut(
		&ctx, req, &cq.cq_);
	ptr->reader_->StartCall((void*) &ctx);

	EXPECT_EQ(0, num_cb_called);

	auto expect_msg = sprintf("call %p created... processing", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg, _)).Times(1);

	void* got_tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, &ctx);
	ptr->handle(true);

	EXPECT_EQ(0, num_cb_called);

	auto expect_msg2 = sprintf("call %p received... handling", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg2, _)).Times(2);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(1, num_cb_called);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);
	EXPECT_EQ(2, num_cb_called);

	auto expect_msg3 = sprintf("call %p received... finishing", ptr);
	EXPECT_CALL(*logger, log(info_level, expect_msg3, _)).Times(1);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(false);
	EXPECT_EQ(2, num_cb_called);

	auto expect_msg4 = sprintf("call %p failed: oh no", ptr);
	EXPECT_CALL(*logger, log(error_level, expect_msg4, _)).Times(1);

	EXPECT_TRUE(cq.next(&got_tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(got_tag, ptr);
	ptr->handle(true);

	bool promise_cb_called = false;
	ErrptrT got_err = nullptr;
	wait_for(*promise,
	[&got_err, &promise_cb_called](ErrptrT err)
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
	auto promise1 = std::make_shared<ErrPromiseT>();
	auto promise2 = std::make_shared<ErrPromiseT>();
	auto promise3 = std::make_shared<ErrPromiseT>();

	promise1->set_value(::cppkg::error::error("hello"));
	promise2->set_value(nullptr);
	promise3->set_value(::cppkg::error::error("world"));

	ErrPromisesT promises = {promise1, promise2, promise3};

	std::unordered_set<ErrptrT> errors;
	wait_for(promises,
	[&errors](ErrptrT err)
	{
		errors.emplace(err);
	});
	ASSERT_EQ(2, errors.size());
	StrUSetT strs;
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
	auto logger = std::make_shared<MockLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	GrpcServerCQueue cq(builder.AddCompletionQueue());

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	std::string msg;
	std::string msg2;
	EXPECT_CALL(*logger, log(info_level, _, _)).Times(3).
		WillOnce(SaveArg<1>(&msg)).
		WillOnce(Return()). // ignore creation message for next handler
		WillOnce(SaveArg<1>(&msg2));

	using ServerCallT = AsyncServerCall<mock::MockRequest,mock::MockResponse>;
	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool serve_call = false;
	auto call = new ServerCallT(logger,
	[&last_tag, &num_calls, &mock_service](grpc::ServerContext* ctx,
		mock::MockRequest* req,
		iResponder<mock::MockResponse>& writer,
		iCQueue& cq, void* tag)
	{
		auto& grpc_res = static_cast<GrpcResponder<
			mock::MockResponse>&>(writer);
		auto grpc_cq = cq.get_cq();
		mock_service.RequestMockRPC(ctx, req,
			&grpc_res.responder_, grpc_cq, must_cast<
			grpc::ServerCompletionQueue>(grpc_cq), tag);
		last_tag = tag;
		++num_calls;
	},
	[&serve_call](
		const mock::MockRequest& req,
		mock::MockResponse& res)
	{
		serve_call = true;
		return grpc::Status{grpc::OK, "hello"};
	}, cq);

	auto expect_msg = sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg.c_str(), msg.c_str());

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
		auto status = stub->MockRPC(&ctx, req, &res);
		EXPECT_TRUE(status.ok());
	}).detach();

	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(serve_call);

	auto expect_msg2 = sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg2.c_str(), msg2.c_str());

	auto expect_msg3 = sprintf("rpc %p completed", call);
	EXPECT_CALL(*logger, log(info_level, expect_msg3, _)).Times(1);

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	server->Shutdown();

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<ServerCallT*>(tag_name));
	tag_name->shutdown();

	cq.shutdown();
}


TEST(ASYNC, ServerRequestFailed)
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12345";
	auto logger = std::make_shared<MockLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	GrpcServerCQueue cq(builder.AddCompletionQueue());

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	std::string msg;
	std::string msg1;
	EXPECT_CALL(*logger, log(info_level, _, _)).Times(3).
		WillOnce(SaveArg<1>(&msg)).
		WillOnce(Return()). // ignore creation message for next handler
		WillOnce(SaveArg<1>(&msg1));

	using ServerCallT = AsyncServerCall<mock::MockRequest,mock::MockResponse>;
	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool serve_call = false;
	auto call = new ServerCallT(logger,
	[&last_tag, &num_calls, &mock_service](grpc::ServerContext* ctx,
		mock::MockRequest* req,
		iResponder<mock::MockResponse>& writer,
		iCQueue& cq, void* tag)
	{
		auto& grpc_res = static_cast<GrpcResponder<mock::MockResponse>&>(writer);
		auto grpc_cq = cq.get_cq();
		mock_service.RequestMockRPC(ctx, req,
			&grpc_res.responder_, grpc_cq, must_cast<
			grpc::ServerCompletionQueue>(grpc_cq), tag);
		last_tag = tag;
		++num_calls;
	},
	[&serve_call](
		const mock::MockRequest& req,
		mock::MockResponse& res)
	{
		serve_call = true;
		return grpc::Status{grpc::NOT_FOUND, "oh nos"};
	}, cq);

	auto expect_msg = sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg.c_str(), msg.c_str());

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
		auto status = stub->MockRPC(&ctx, req, &res);
		EXPECT_FALSE(status.ok());
	}).detach();

	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	auto expect_msg1 = sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg1.c_str(), msg1.c_str());

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(serve_call);

	auto expect_msg2 = sprintf("rpc %p completed", call);
	EXPECT_CALL(*logger, log(info_level, expect_msg2, _)).Times(1);

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	server->Shutdown();

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<ServerCallT*>(tag_name));
	tag_name->shutdown();

	cq.shutdown();
}


TEST(ASYNC, ServerStream)
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12346";
	auto logger = std::make_shared<MockLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	GrpcServerCQueue cq(builder.AddCompletionQueue());

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	std::string msg;
	std::string msg1;
	std::string msg2;
	std::string msg3;
	std::string msg4;
	EXPECT_CALL(*logger, log(info_level, _, _)).Times(6).
		WillOnce(SaveArg<1>(&msg)).
		WillOnce(Return()). // ignore creation message for next handler
		WillOnce(SaveArg<1>(&msg1)).
		WillOnce(SaveArg<1>(&msg2)).
		WillOnce(SaveArg<1>(&msg3)).
		WillOnce(SaveArg<1>(&msg4));

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool init_call = false;
	size_t num_iterators = 0;
	using StreamCallT = AsyncServerStreamCall<mock::MockRequest,
		mock::MockResponse,std::vector<size_t>>;

	auto call = new StreamCallT(logger,
	[&last_tag, &num_calls, &mock_service](
		grpc::ServerContext* ctx,
		mock::MockRequest* req,
		iWriter<mock::MockResponse>& writer,
		iCQueue& cq, void* tag)
	{
		auto& grpc_res = static_cast<GrpcWriter<
			mock::MockResponse>&>(writer);
		auto grpc_cq = cq.get_cq();
		mock_service.RequestMockStreamOut(ctx, req,
			&grpc_res.writer_, grpc_cq, must_cast<
			grpc::ServerCompletionQueue>(grpc_cq), tag);
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
	}, cq);

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(init_call);
	EXPECT_EQ(0, num_iterators);

	auto expect_msg0 = sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg0.c_str(), msg.c_str());

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
	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(1, num_iterators);

	auto expect_msg1 = sprintf("rpc %p initializing", call);
	EXPECT_STREQ(expect_msg1.c_str(), msg1.c_str());

	auto expect_msg = sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg.c_str(), msg2.c_str());

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_STREQ(expect_msg.c_str(), msg3.c_str());

	EXPECT_EQ(2, num_calls);
	EXPECT_EQ(3, num_iterators);
	EXPECT_CALL(*logger, log(info_level, expect_msg, _)).Times(1);

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(2, num_calls);
	EXPECT_EQ(4, num_iterators);

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	auto expect_msg2 = sprintf("rpc %p completed", call);
	EXPECT_STREQ(expect_msg2.c_str(), msg4.c_str());

	server->Shutdown();

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<StreamCallT*>(tag_name));
	tag_name->shutdown();

	cq.shutdown();
}


TEST(ASYNC, ServerStreamStartupError)
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12347";
	auto logger = std::make_shared<MockLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	GrpcServerCQueue cq(builder.AddCompletionQueue());

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	std::string msg;
	std::string msg1;
	std::string msg2;
	EXPECT_CALL(*logger, log(info_level, _, _)).Times(4).
		WillOnce(SaveArg<1>(&msg)).
		WillOnce(Return()). // ignore creation message for next handler
		WillOnce(SaveArg<1>(&msg1)).
		WillOnce(SaveArg<1>(&msg2));

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool init_call = false;
	size_t num_iterators = 0;
	using StreamCallT = AsyncServerStreamCall<mock::MockRequest,
		mock::MockResponse,std::vector<size_t>>;

	auto call = new StreamCallT(logger,
	[&last_tag, &num_calls, &mock_service](
		grpc::ServerContext* ctx,
		mock::MockRequest* req,
		iWriter<mock::MockResponse>& writer,
		iCQueue& cq, void* tag)
	{
		auto& grpc_res = static_cast<GrpcWriter<mock::MockResponse>&>(writer);
		auto grpc_cq = cq.get_cq();
		mock_service.RequestMockStreamOut(ctx, req,
			&grpc_res.writer_, grpc_cq, must_cast<
			grpc::ServerCompletionQueue>(grpc_cq), tag);
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
	}, cq);

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(init_call);
	EXPECT_EQ(0, num_iterators);
	auto expect_msg0 = sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg0.c_str(), msg.c_str());

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
	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(0, num_iterators);
	auto expect_msg = sprintf("rpc %p initializing", call);
	EXPECT_STREQ(expect_msg.c_str(), msg1.c_str());

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(0, num_iterators);
	auto expect_msg2 = sprintf("rpc %p completed", call);
	EXPECT_STREQ(expect_msg2.c_str(), msg2.c_str());

	server->Shutdown();

	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_FALSE(ok);
	EXPECT_EQ(last_tag, tag);
	EXPECT_NE(last_tag, call);
	auto tag_name = static_cast<iServerCall*>(last_tag);
	EXPECT_NE(nullptr, dynamic_cast<StreamCallT*>(tag_name));
	tag_name->shutdown();

	cq.shutdown();
}


TEST(ASYNC, DISABLED_ServerStreamForcedFinish) // FLAKY WITH RACE CONDITION
{
	mock::MockService::AsyncService mock_service;
	std::string address = "0.0.0.0:12347";
	auto logger = std::make_shared<MockLogger>();

	grpc::ServerBuilder builder;
	builder.AddListeningPort(address,
		grpc::InsecureServerCredentials());
	GrpcServerCQueue cq(builder.AddCompletionQueue());

	builder.RegisterService(&mock_service);
	std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

	std::string msg;
	std::string msg1;
	std::string msg2;
	std::string creation_msg;
	EXPECT_CALL(*logger, log(info_level, _, _)).Times(4).
		WillOnce(SaveArg<1>(&msg)).
		WillOnce(SaveArg<1>(&creation_msg)). // ignore creation message for next handler
		WillOnce(SaveArg<1>(&msg1)).
		WillOnce(SaveArg<1>(&msg2));

	void* last_tag = nullptr;
	size_t num_calls = 0;
	bool init_call = false;
	size_t num_iterators = 0;
	using StreamCallT = AsyncServerStreamCall<mock::MockRequest,
		mock::MockResponse,std::vector<size_t>>;

	auto call = new StreamCallT(logger,
	[&last_tag, &num_calls, &mock_service](
		grpc::ServerContext* ctx,
		mock::MockRequest* req,
		iWriter<mock::MockResponse>& writer,
		iCQueue& cq, void* tag)
	{
		auto& grpc_res = static_cast<GrpcWriter<mock::MockResponse>&>(writer);
		auto grpc_cq = cq.get_cq();
		mock_service.RequestMockStreamOut(ctx, req,
			&grpc_res.writer_, grpc_cq, must_cast<
			grpc::ServerCompletionQueue>(grpc_cq), tag);
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
		return true;
	}, cq);

	EXPECT_EQ(call, last_tag);
	EXPECT_EQ(1, num_calls);
	EXPECT_FALSE(init_call);
	EXPECT_EQ(0, num_iterators);
	auto expect_msg0 = sprintf("rpc %p created", call);
	EXPECT_STREQ(expect_msg0.c_str(), msg.c_str());

	std::condition_variable after_shutdown;
	std::mutex after_shutdown_m;
	bool after_shutdown_done = false;

	std::thread(
	[&]()
	{
		auto stub = mock::MockService::NewStub(grpc::CreateChannel(address,
			grpc::InsecureChannelCredentials()));
		grpc::ClientContext ctx;
		ctx.set_deadline(
			std::chrono::system_clock::now() + std::chrono::minutes(1));
		mock::MockRequest req;
		mock::MockResponse res;
		auto reader = stub->MockStreamOut(&ctx, req);

		// initial section
		ASSERT_TRUE(reader->Read(&res));

		// process section
		{
			std::unique_lock<std::mutex> lk(after_shutdown_m);
			after_shutdown.wait(lk, [&after_shutdown_done]{ return after_shutdown_done; });
		}

		EXPECT_FALSE(reader->Read(&res));
		auto status = reader->Finish();
		EXPECT_EQ(grpc::CANCELLED, status.error_code());
	}).detach();

	// initial section
	void* tag;
	bool ok = true;
	EXPECT_TRUE(cq.next(&tag, &ok));
	EXPECT_TRUE(ok);
	EXPECT_EQ(tag, call);
	call->serve();

	ASSERT_NE(call, last_tag);
	EXPECT_EQ(2, num_calls);
	EXPECT_TRUE(init_call);
	EXPECT_EQ(1, num_iterators);
	auto expect_msg = sprintf("rpc %p initializing", call);
	auto expect_msg1 = sprintf("rpc %p writing", call);
	EXPECT_STREQ(expect_msg.c_str(), msg1.c_str());
	EXPECT_STREQ(expect_msg1.c_str(), msg2.c_str());

	// early shutdown section
	call->shutdown();

	after_shutdown_done = true;
	after_shutdown.notify_all();

	server->Shutdown();

	std::smatch sm;
	std::regex_search(creation_msg, sm, std::regex("rpc 0x(.+) created"));
	std::stringstream ss;
	ss << std::hex << sm[1];
	size_t i;
	ss >> i;
	void* last_entry = (void*) i;
	EXPECT_NE(tag, last_entry);
	StreamCallT* last_handler = dynamic_cast<StreamCallT*>(static_cast<iServerCall*>(last_entry));
	ASSERT_NE(nullptr, last_handler);

	delete last_handler;
}


struct MockGrpcClient final : public GrpcClient
{
	MockGrpcClient (const ClientConfig& cfg) : GrpcClient(cfg) {}

	void public_ctx (grpc::ClientContext& ctx, bool is_request)
	{
		build_ctx(ctx, is_request);
	}
};


TEST(CLIENT, Context)
{
	ClientConfig cfg(std::chrono::milliseconds(420), std::chrono::milliseconds(42000), 4);
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
