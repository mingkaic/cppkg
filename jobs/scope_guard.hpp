///
/// scope_guard.hpp
/// jobs
///
/// Purpose:
/// Simulate golang defer statement
///

#include <functional>

#ifndef PKG_JOBS_SCOPE_GUARD_HPP
#define PKG_JOBS_SCOPE_GUARD_HPP

namespace jobs
{

/// Invoke held function upon destruction
/// Operates as C++ style of Golang's defer
struct ScopeGuard // todo: replace with a better option
{
	ScopeGuard (std::function<void(void)> f) : term_(f) {}

	virtual ~ScopeGuard (void)
	{
		term_();
	}

	ScopeGuard (const ScopeGuard&) = delete;

	ScopeGuard (ScopeGuard&&) = delete;

	ScopeGuard& operator = (const ScopeGuard&) = delete;

	ScopeGuard& operator = (ScopeGuard&&) = delete;

private:
	std::function<void(void)> term_;
};

}

#endif // PKG_JOBS_SCOPE_GUARD_HPP
