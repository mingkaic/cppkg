
#ifndef PKG_TYPES_ITERATORS_HPP
#define PKG_TYPES_ITERATORS_HPP

#include <iterator>
#include <type_traits>

namespace types
{

// source: https://stackoverflow.com/questions/13830158/check-if-a-variable-type-is-iterable
template <typename T>
auto is_iterable_impl(int)
-> decltype (
	std::begin(std::declval<T&>()) != std::end(std::declval<T&>()), // begin/end and operator !=
	void(), // Handle evil operator ,
	++std::declval<decltype(std::begin(std::declval<T&>()))&>(), // operator ++
	void(*std::begin(std::declval<T&>())), // operator*
	std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

template <typename T>
using is_iterable = decltype(is_iterable_impl<T>(0));

// source: https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream
template<typename T, typename S>
auto is_streamable_impl(int)
-> decltype( std::declval<S&>() << std::declval<T>(), std::true_type{});

template<typename T, typename S>
static auto is_streamable_impl(...) -> std::false_type;

template<typename T, typename S=std::ostream>
using is_streamable = decltype(is_streamable_impl<T,S>(0));

template <typename ITER>
using IterValT = typename std::iterator_traits<ITER>::value_type;

template <typename ARR, typename std::enable_if<is_iterable<ARR>::value>::type* = nullptr>
using ArrItT = decltype(std::begin(std::declval<ARR&>()));

template <typename ARR, typename std::enable_if<is_iterable<ARR>::value>::type* = nullptr>
using ArrCstItT = decltype(std::cbegin(std::declval<ARR&>()));

template <typename ARR, typename std::enable_if<is_iterable<ARR>::value>::type* = nullptr>
using ArrValT = IterValT<ArrItT<ARR>>;

}

#endif // PKG_TYPES_ITERATORS_HPP
