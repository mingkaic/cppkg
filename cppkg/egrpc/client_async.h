
#ifndef CPPKG_EGRPC_CLIENT_ASYNC_H
#define CPPKG_EGRPC_CLIENT_ASYNC_H

#include "cppkg/egrpc/iclient_async.h"

namespace cppkg
{

namespace egrpc
{

template <typename REQ, typename RES>
struct AsyncClientHandler final : public iClientHandler
{
	using ReadptrT = std::shared_ptr<
		grpc::ClientAsyncResponseReaderInterface<RES>>;

	using HandleResF = std::function<void(RES&)>;

	using InitF = std::function<void(REQ&,AsyncClientHandler<REQ,RES>*)>;

	AsyncClientHandler (ErrPromiseptrT promise,
		std::shared_ptr<cisab::logs::iLogger> logger,
		HandleResF cb, InitF init, size_t nretries) :
		iClientHandler(promise), logger_(logger),
		nretries_(nretries), init_(init), cb_(cb)
	{
		init_(request_, this);
	}

	void handle (bool) override
	{
		if (status_.ok())
		{
			logger_->log(cisab::logs::info_level, cisab::fmts::sprintf(
				"call %p completed successfully", this));
			if (cb_)
			{
				cb_(reply_);
			}
		}
		else
		{
			if (nretries_ > 0)
			{
				logger_->log(cisab::logs::error_level, cisab::fmts::sprintf(
					"call %p (%d attempts remaining) failed: %s",
					this, nretries_, status_.error_message().c_str()));
				new AsyncClientHandler<REQ,RES>(
					complete_promise_, logger_, cb_, init_, nretries_ - 1);
				complete_promise_ = nullptr;
			}
			else
			{
				error_ = error::error(status_.error_message());
				logger_->log(cisab::logs::error_level, cisab::fmts::sprintf(
					"call %p failed: %s",
					this, status_.error_message().c_str()));
			}
		}
		delete this;
	}

	ReadptrT reader_;

	grpc::Status status_;

	// ctx_ and reader_ need to be kept in memory
	grpc::ClientContext ctx_;

	RES reply_;

private:
	std::shared_ptr<cisab::logs::iLogger> logger_;

	REQ request_;

	size_t nretries_;

	InitF init_;

	HandleResF cb_;
};

}

}

#endif // CPPKG_EGRPC_CLIENT_ASYNC_H
