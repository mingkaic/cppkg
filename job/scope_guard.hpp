#include <functional>

#ifndef PKG_JOB_SCOPE_GUARD_HPP
#define PKG_JOB_SCOPE_GUARD_HPP

namespace job
{

struct ScopeGuard final // todo: replace with a better option
{
	ScopeGuard (std::function<void(void)> f) : term_(f) {}

	~ScopeGuard (void)
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

#endif // PKG_JOB_SCOPE_GUARD_HPP
