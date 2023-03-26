///
/// scope_guard.h
/// jobs
///
/// Purpose:
/// Simulate golang defer statement
///

#ifndef CPPKG_JOBS_SCOPE_GUARD_H
#define CPPKG_JOBS_SCOPE_GUARD_H

#include <functional>

namespace cppkg
{

namespace jobs
{

using GuardOpF = std::function<void(void)>;

/// Invoke held function upon destruction
/// Operates as C++ style of Golang's defer
struct ScopeGuard // todo: replace with a better option
{
	ScopeGuard (GuardOpF f) : term_(f) {}

	virtual ~ScopeGuard (void)
	{
		if (term_)
		{
			term_();
		}
	}

	ScopeGuard (const ScopeGuard&) = delete;

	ScopeGuard (ScopeGuard&& other) :
		term_(std::move(other.term_))
	{
		other.term_ = GuardOpF(); // explicitly clear other to avoid duplicate term_ calls
	}

	ScopeGuard& operator = (const ScopeGuard&) = delete;

	ScopeGuard& operator = (ScopeGuard&& other)
	{
		if (this != &other)
		{
			if (term_)
			{
				term_();
			}
			term_ = std::move(other.term_);
			other.term_ = GuardOpF();
		}
		return *this;
	}

private:
	GuardOpF term_;
};

}

}

#endif // CPPKG_JOBS_SCOPE_GUARD_H
