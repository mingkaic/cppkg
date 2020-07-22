
#include <memory>

#include "fmts/fmts.hpp"

#ifndef ERROR_IERROR_HPP
#define ERROR_IERROR_HPP

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
		ErrMsg(fmts::sprintf(fmt, args...)) {}

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

#endif // ERROR_IERROR_HPP
