
#include "cppkg/error/error.h"

#ifdef CPPKG_ERROR_IERROR_H

namespace cppkg
{

namespace error
{

ErrptrT error (const std::string& msg)
{
	return std::make_shared<ErrMsg>(msg);
}

}

}

#endif
