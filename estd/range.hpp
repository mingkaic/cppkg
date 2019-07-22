#include <algorithm>
#include <type_traits>

#ifndef PKG_ESTD_RANGE_HPP
#define PKG_ESTD_RANGE_HPP

namespace estd
{

template <typename T, typename = typename std::enable_if<
	std::is_arithmetic<T>::value, T>::type>
struct NumRange final
{
	NumRange (void) : lower_(0), upper_(0) {}

	NumRange (T bound1, T bound2) :
		lower_(std::min(bound1, bound2)),
		upper_(std::max(bound1, bound2)) {}

	T lower_;

	T upper_;
};

}

#endif // PKG_ESTD_RANGE_HPP
