
#ifndef PKG_EGRPC_SERVER_ASYNC_HPP
#define PKG_EGRPC_SERVER_ASYNC_HPP

#include <future>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/async_stream.h>

#include "logs/logs.hpp"

namespace egrpc
{

// Detached server calls
struct iServerCall
{
	virtual ~iServerCall (void) = default;

	virtual void serve (void) = 0;

	virtual void shutdown (void) = 0;
};

// Async server request response call
template <typename REQ, typename WRITER>
struct AsyncServerCall final : public iServerCall
{
	using RequestF = std::function<void(grpc::ServerContext*,REQ*,
		WRITER*,grpc::CompletionQueue*,grpc::ServerCompletionQueue*,void*)>;

	using WriteF = std::function<void(const REQ&,WRITER&,iServerCall*)>;

	AsyncServerCall (std::shared_ptr<logs::iLogger> logger,
		RequestF req_call, WriteF write_call,
		grpc::ServerCompletionQueue* cq) : logger_(logger),
		req_call_(req_call), write_call_(write_call),
		cq_(cq), responder_(&ctx_), status_(PROCESS)
	{
		req_call_(&ctx_, &req_, &responder_, cq_, cq_, (void*) this);
		logger_->log(logs::info_level, fmts::sprintf("rpc %p created", this));
	}

	void serve (void) override
	{
		switch (status_)
		{
		case PROCESS:
		{
			new AsyncServerCall(logger_, req_call_, write_call_, cq_);
			status_ = FINISH;
			logger_->log(logs::info_level, fmts::sprintf("rpc %p writing", this));
			write_call_(req_, responder_, this);
		}
			break;
		case FINISH:
			logger_->log(logs::info_level, fmts::sprintf("rpc %p completed", this));
			shutdown();
		}
	}

	void shutdown (void) override
	{
		delete this;
	}

private:
	std::shared_ptr<logs::iLogger> logger_;

	REQ req_;

	grpc::ServerContext ctx_;

	RequestF req_call_;

	WriteF write_call_;

	grpc::ServerCompletionQueue* cq_;

	WRITER responder_;

	enum CallStatus { PROCESS, FINISH };

	CallStatus status_;
};

template <typename R>
struct iWriter
{
	virtual void Write (const R& res, iServerCall* tag) = 0;

	virtual void Finish (grpc::Status status, iServerCall* tag) = 0;
};

template <typename R>
using WriterptrT = std::shared_ptr<iWriter<R>>;

template <typename R>
struct GrpcWriter final : public iWriter<R>
{
	GrpcWriter (grpc::ServerContext& ctx) :
		responder_(&ctx) {}

	void Write (const R& res, iServerCall* tag) override
	{
		responder_.Write(res, tag);
	}

	void Finish (grpc::Status status, iServerCall* tag) override
	{
		responder_.Finish(status, tag);
	}

	grpc::ServerAsyncWriter<R> responder_;
};

// Async server request stream call
template <typename REQ, typename RES, typename RANGE,
	typename IT = typename RANGE::iterator>
struct AsyncServerStreamCall final : public iServerCall
{
	using RequestF = std::function<void(grpc::ServerContext*,REQ*,
		WriterptrT<RES>&,grpc::CompletionQueue*,
		grpc::ServerCompletionQueue*,void*)>;

	using InitF = std::function<grpc::Status(RANGE&,const REQ&)>;

	using WriteF = std::function<bool(const REQ&,IT&,RES&)>;

	AsyncServerStreamCall (std::shared_ptr<logs::iLogger> logger,
		RequestF req_call, InitF init_call, WriteF write_call,
		grpc::ServerCompletionQueue* cq) : logger_(logger),
		req_call_(req_call), init_call_(init_call), write_call_(write_call),
		cq_(cq), status_(STARTUP)
	{
		req_call_(&ctx_, &req_, responder_, cq_, cq_, (void*) this);
		if (nullptr == responder_)
		{
			logger_->log(logs::fatal_level,
				"failed to create server stream caller with non-null writer");
		}
		logger_->log(logs::info_level, fmts::sprintf("rpc %p created", this));
	}

	void serve (void) override
	{
		switch (status_)
		{
		case STARTUP:
		{
			new AsyncServerStreamCall(
				logger_, req_call_, init_call_, write_call_, cq_);
			logger_->log(logs::info_level, fmts::sprintf("rpc %p initializing", this));
			auto out_status = init_call_(ranges_, req_);
			if (false == out_status.ok())
			{
				status_ = FINISH;
				responder_->Finish(out_status, this);
				return;
			}
			it_ = ranges_.begin();
		}
			[[fallthrough]];
		case PROCESS:
		{
			status_ = PROCESS;
			if (it_ != ranges_.end())
			{
				RES reply;
				logger_->log(logs::info_level, fmts::sprintf("rpc %p writing", this));
				bool wrote = false;
				while (false == wrote && it_ != ranges_.end())
				{
					wrote = write_call_(req_, it_, reply);
					++it_;
				}
				if (wrote)
				{
					responder_->Write(reply, this);
					return;
				} // else it_ == ranges_.end()
			}

			if (it_ == ranges_.end())
			{
				status_ = FINISH;
				responder_->Finish(grpc::Status::OK, this);
			}
		}
			break;
		case FINISH:
			logger_->log(logs::info_level, fmts::sprintf("rpc %p completed", this));
			shutdown();
		}
	}

	void shutdown (void) override
	{
		if (status_ == PROCESS)
		{
			responder_->Finish(grpc::Status::CANCELLED, this);
		}
		delete this;
	}

private:
	std::shared_ptr<logs::iLogger> logger_;

	RANGE ranges_;

	IT it_;

	REQ req_;

	grpc::ServerContext ctx_;

	RequestF req_call_;

	InitF init_call_;

	WriteF write_call_;

	grpc::ServerCompletionQueue* cq_;

	WriterptrT<RES> responder_;

	enum CallStatus { STARTUP, PROCESS, FINISH };

	CallStatus status_;
};

}

#endif // PKG_EGRPC_SERVER_ASYNC_HPP
