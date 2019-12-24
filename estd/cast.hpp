#include <memory>

#include "logs/logs.hpp"

#ifndef PKG_ESTD_CAST_HPP
#define PKG_ESTD_CAST_HPP

namespace estd
{

/// Safely cast shared pointer U to corresponding T ensuring output is not null,
/// and emit fatal error upon cast failure. This function neither replaces
/// std::static_pointer_cast nor std::dynamic_pointer_cast
/// since cast failure is possible but (expectedly) rare
template <typename T, typename U>
std::shared_ptr<T> must_ptr_cast (const std::shared_ptr<U>& ptr)
{
	if (nullptr == ptr)
	{
		logs::fatalf("cannot cast null %s to %s",
			typeid(U).name(), typeid(T).name());
	}
	auto out = std::dynamic_pointer_cast<T>(ptr);
	if (nullptr == out)
	{
		logs::fatalf("failed to cast %s to %s",
			typeid(U).name(), typeid(T).name());
	}
	return out;
}

}

#endif // PKG_ESTD_CAST_HPP
