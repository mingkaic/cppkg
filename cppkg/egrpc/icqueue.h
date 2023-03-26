
#include <grpcpp/grpcpp.h>

#ifndef CPPKG_EGRPC_ICQUEUE_H
#define CPPKG_EGRPC_ICQUEUE_H

namespace cppkg
{

namespace egrpc
{

struct iCQueue
{
	virtual ~iCQueue (void) = default;

	virtual bool next (void** ptr, bool* ok) = 0;

	virtual void shutdown (void) = 0;

	virtual grpc::CompletionQueue* get_cq (void) = 0;
};

}

}

#endif // CPPKG_EGRPC_ICQUEUE_H
