
#ifndef PKG_ARRS_ARRAY_HPP
#define PKG_ARRS_ARRAY_HPP

#include <numeric>
#include <array>

namespace arrs
{

template <typename T, size_t N>
std::array<T,N> array_range (T begin)
{
	std::array<T,N> out;
	std::iota(out.begin(), out.end(), begin);
	return out;
}

}

#endif // PKG_ARRS_ARRAY_HPP
