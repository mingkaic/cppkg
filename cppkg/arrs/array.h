
#ifndef CPPKG_ARRS_ARRAY_H
#define CPPKG_ARRS_ARRAY_H

#include <numeric>
#include <array>

namespace cppkg
{

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

}

#endif // CPPKG_ARRS_ARRAY_H
