
#ifndef CPPKG_EGRPC_ICLIENT_ASYNC_H
#define CPPKG_EGRPC_ICLIENT_ASYNC_H

#include <future>
#include <cassert>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/async_stream.h>

#include "cisab/logs/logger.h"

#include "cppkg/error/error.h"

namespace cppkg
{

namespace egrpc
{

using HandleErrF = std::function<void(error::ErrptrT)>;

using ErrPromiseT = std::promise<error::ErrptrT>;

using ErrPromiseptrT = std::shared_ptr<ErrPromiseT>;

using ErrPromisesT = std::list<ErrPromiseptrT>;

// Detached client response handlers
struct iClientHandler
{
	iClientHandler (ErrPromiseptrT promise) : complete_promise_(promise) {}

	virtual ~iClientHandler  (void)
	{
		if (complete_promise_)
		{
			complete_promise_->set_value(error_);
		}
	}

	virtual void handle (bool event_status) = 0;

protected:
	ErrPromiseptrT complete_promise_;

	error::ErrptrT error_ = nullptr;
};

void wait_for (ErrPromiseT& promise, HandleErrF err_handle);

void wait_for (ErrPromisesT& promises, HandleErrF err_handle);

}

}

#endif // CPPKG_EGRPC_ICLIENT_ASYNC_H
