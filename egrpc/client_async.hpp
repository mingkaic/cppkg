
#include <future>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/async_stream.h>

#include "logs/logs.hpp"

#ifndef EGRPC_CLIENT_ASYNC_HPP
#define EGRPC_CLIENT_ASYNC_HPP

namespace egrpc
{

// Detached client response handlers
struct iClientHandler
{
	virtual ~iClientHandler  (void) = default;

	virtual void handle (bool event_status) = 0;
};

template <typename RES>
struct AsyncClientHandler final : public iClientHandler
{
	using ReadptrT = std::unique_ptr<
		grpc::ClientAsyncResponseReader<RES>>;

	using HandleResF = std::function<void(RES&)>;

	using InitF = std::function<void(AsyncClientHandler<RES>*)>;

	AsyncClientHandler (std::shared_ptr<logs::iLogger> logger,
		HandleResF cb, InitF init, size_t nretries) :
		logger_(logger), cb_(cb), init_(init), nretries_(nretries)
	{
		init_(this);
	}

	~AsyncClientHandler (void) { complete_promise_.set_value(); }

	void handle (bool event_status) override
	{
		if (status_.ok())
		{
			logger_->log(logs::info_level, fmts::sprintf(
				"call %p completed successfully", this));
			cb_(reply_);
		}
		else
		{
			logger_->log(logs::error_level, fmts::sprintf(
				"call %p (%d attempts remaining) failed: %s",
				this, nretries_, status_.error_message().c_str()));
			if (nretries_ > 0)
			{
				auto next = new AsyncClientHandler<RES>(logger_, cb_, init_, nretries_ - 1);
				next->complete_promise_.swap(complete_promise_);
			}
		}
		delete this;
	}

	std::shared_ptr<logs::iLogger> logger_;

	HandleResF cb_;

	InitF init_;

	size_t nretries_;

	RES reply_;

	ReadptrT reader_;

	grpc::Status status_;

	// ctx_ and reader_ need to be kept in memory
	grpc::ClientContext ctx_;

	std::promise<void> complete_promise_;
};

using ErrPromiseT = std::promise<error::ErrptrT>;

using ErrFutureT = std::future<error::ErrptrT>;

template <typename DATA>
struct AsyncClientStreamHandler final : public iClientHandler
{
	using ReadptrT = std::unique_ptr<
		grpc::ClientAsyncReaderInterface<DATA>>;

	using HandlerF = std::function<void(DATA&)>;

	AsyncClientStreamHandler (std::shared_ptr<logs::iLogger> logger, HandlerF handler) :
		logger_(logger), handler_(handler), call_status_(STARTUP) {}

	~AsyncClientStreamHandler (void) { complete_promise_.set_value(error_); }

	void handle (bool event_status) override
	{
		assert(nullptr != reader_);
		switch (call_status_)
		{
		case STARTUP:
			if (event_status)
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p created... processing", this));
				call_status_ = PROCESS;
				reader_->Read(&reply_, (void*) this);
			}
			else
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p created... finishing", this));
				call_status_ = FINISH;
				reader_->Finish(&status_, (void*)this);
			}
			break;
		case PROCESS:
			if (event_status)
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p received... handling", this));
				handler_(reply_);
				reader_->Read(&reply_, (void*)this);
			}
			else
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p received... finishing", this));
				call_status_ = FINISH;
				reader_->Finish(&status_, (void*)this);
			}
			break;
		case FINISH:
			if (status_.ok())
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p completed successfully", this));
			}
			else
			{
				error_ = error::error(status_.error_message());
				logger_->log(logs::error_level, fmts::sprintf(
					"call %p failed: %s",
					this, status_.error_message().c_str()));
			}
			delete this;
		}
	}

	std::shared_ptr<logs::iLogger> logger_;

	grpc::ClientContext ctx_;

	ReadptrT reader_;

	HandlerF handler_;

	DATA reply_;

	grpc::Status status_;

	ErrPromiseT complete_promise_;

	error::ErrptrT error_ = nullptr;

private:
	enum CallStatus { STARTUP, PROCESS, FINISH };

	CallStatus call_status_;
};

}

#endif // EGRPC_CLIENT_ASYNC_HPP
