
#ifndef PKG_EGRPC_SERVER_ASYNC_STREAM_HPP
#define PKG_EGRPC_SERVER_ASYNC_STREAM_HPP

#include "egrpc/iserver_async.hpp"

namespace egrpc
{

template <typename R>
struct GrpcWriter final : public iWriter<R>
{
	GrpcWriter (grpc::ServerContext& ctx) :
		responder_(&ctx) {}

	void write (const R& res, void* tag) override
	{
		responder_.Write(res, tag);
	}

	void finish (grpc::Status status, void* tag) override
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
		iWriter<RES>&,grpc::CompletionQueue*,
		grpc::ServerCompletionQueue*,void*)>;

	using InitF = std::function<grpc::Status(RANGE&,const REQ&)>;

	using WriteF = std::function<bool(const REQ&,IT&,RES&)>;

	AsyncServerStreamCall (std::shared_ptr<logs::iLogger> logger,
		RequestF req_call, InitF init_call, WriteF write_call,
		grpc::ServerCompletionQueue* cq,
		BuildWriterF<RES> make_writer =
		[](grpc::ServerContext& ctx) -> WriterptrT<RES>
		{
			return std::make_unique<GrpcWriter<RES>>(ctx);
		}) : logger_(logger),
		cq_(cq), responder_(make_writer(ctx_)), status_(STARTUP),
		req_call_(req_call), init_call_(init_call), write_call_(write_call)
	{
		req_call_(&ctx_, &req_, *responder_, cq_, cq_, (void*) this);
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
				responder_->finish(out_status, this);
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
					responder_->write(reply, this);
					return;
				} // else it_ == ranges_.end()
			}

			if (it_ == ranges_.end())
			{
				status_ = FINISH;
				responder_->finish(grpc::Status::OK, this);
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
			responder_->finish(grpc::Status::CANCELLED, this);
		}
		delete this;
	}

private:
	enum CallStatus { STARTUP, PROCESS, FINISH };

	std::shared_ptr<logs::iLogger> logger_;

	grpc::ServerContext ctx_;

	grpc::ServerCompletionQueue* cq_;

	REQ req_;

	WriterptrT<RES> responder_;

	RANGE ranges_;

	IT it_;

	CallStatus status_;

	RequestF req_call_;

	InitF init_call_;

	WriteF write_call_;
};

}

#endif // PKG_EGRPC_SERVER_ASYNC_STREAM_HPP
