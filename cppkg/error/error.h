
#ifndef CPPKG_ERROR_IERROR_H
#define CPPKG_ERROR_IERROR_H

#include <memory>

#include "cisab/fmts/format.h"

namespace cppkg
{

namespace error
{

struct iError
{
	virtual ~iError (void) = default;

	virtual std::string to_string (void) const = 0;
};

struct ErrMsg final : public iError
{
	ErrMsg (const std::string& msg) : msg_(msg) {}

	template <typename ...ARGS>
	ErrMsg (const std::string& format, ARGS... args) :
		ErrMsg(cisab::fmts::sprintf(format, args...)) {}

	std::string to_string (void) const override
	{
		return msg_;
	}

private:
	std::string msg_;
};

using ErrptrT = std::shared_ptr<iError>;

ErrptrT error (const std::string& msg);

template <typename... ARGS>
ErrptrT errorf (const std::string& format, ARGS... args)
{
	return std::make_shared<ErrMsg>(format, args...);
}

}

}

#endif // CPPKG_ERROR_IERROR_H
