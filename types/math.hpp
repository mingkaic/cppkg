
#ifndef PKG_TYPES_MATH_HPP
#define PKG_TYPES_MATH_HPP

#include <complex>
#include <type_traits>

namespace types
{

template<typename T>
struct is_complex : public std::false_type {};

template<typename T>
struct is_complex<std::complex<T>> : public std::true_type {};

}

#endif // PKG_TYPES_MATH_HPP
