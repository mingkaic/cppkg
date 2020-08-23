
#ifndef PKG_EGRPC_CLIENT_ASYNC_HPP
#define PKG_EGRPC_CLIENT_ASYNC_HPP

#include <future>
#include <cassert>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/async_stream.h>

#include "error/error.hpp"
#include "logs/logs.hpp"

namespace egrpc
{

using ErrPromiseT = std::promise<error::ErrptrT>;

using ErrPromiseptrT = std::shared_ptr<ErrPromiseT>;

// Detached client response handlers
struct iClientHandler
{
	iClientHandler (ErrPromiseptrT promise) : complete_promise_(promise) {}

	virtual ~iClientHandler  (void)
	{
		complete_promise_->set_value(error_);
	}

	virtual void handle (bool event_status) = 0;

protected:
	ErrPromiseptrT complete_promise_;

	error::ErrptrT error_ = nullptr;
};

template <typename RES>
struct AsyncClientHandler final : public iClientHandler
{
	using ReadptrT = std::unique_ptr<
		grpc::ClientAsyncResponseReader<RES>>;

	using HandleResF = std::function<void(RES&)>;

	using InitF = std::function<void(AsyncClientHandler<RES>*)>;

	AsyncClientHandler (ErrPromiseptrT promise,
		std::shared_ptr<logs::iLogger> logger,
		HandleResF cb, InitF init, size_t nretries) :
		iClientHandler(promise), logger_(logger),
		cb_(cb), init_(init), nretries_(nretries)
	{
		init_(this);
	}

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
				new AsyncClientHandler<RES>(
					complete_promise_, logger_, cb_, init_, nretries_ - 1);
			}
			else
			{
				error_ = error::error(status_.error_message());
				logger_->log(logs::error_level, fmts::sprintf(
					"call %p failed: %s",
					this, status_.error_message().c_str()));
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
};

template <typename DATA>
struct AsyncClientStreamHandler final : public iClientHandler
{
	using ReadptrT = std::unique_ptr<
		grpc::ClientAsyncReaderInterface<DATA>>;

	using HandlerF = std::function<void(DATA&)>;

	AsyncClientStreamHandler (ErrPromiseptrT promise,
		std::shared_ptr<logs::iLogger> logger, HandlerF handler) :
		iClientHandler(promise), logger_(logger),
		handler_(handler), call_status_(STARTUP) {}

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

private:
	enum CallStatus { STARTUP, PROCESS, FINISH };

	CallStatus call_status_;
};

}

#endif // PKG_EGRPC_CLIENT_ASYNC_HPP
