
#ifndef CPPKG_ARRS_LIST_H
#define CPPKG_ARRS_LIST_H

#include <numeric>
#include <list>

namespace cppkg
{

namespace arrs
{

template <typename T>
std::list<T> list_range (T begin, T end)
{
	std::list<T> out(end - begin);
	std::iota(out.begin(), out.end(), begin);
	return out;
}

template <typename T>
void lconcat (std::list<T>& l, const std::list<T>& r)
{
	l.insert(l.end(), r.begin(), r.end());
}

template <typename T>
void rconcat (const std::list<T>& l, std::list<T>& r)
{
	r.insert(r.begin(), l.begin(), l.end());
}

template <typename T>
std::list<T> concat (const std::list<T>& l, const std::list<T>& r)
{
	std::list<T> out = l;
	lconcat(out, r);
	return out;
}

template <typename T, typename ...ARGS>
static void concat_helper (std::list<T>& source, const std::list<T>& r)
{
	lconcat(source, r);
}

template <typename T, typename ...ARGS>
static void concat_helper (std::list<T>& source, const std::list<T>& r,
	ARGS... other)
{
	lconcat(source, r);
	concat_helper(source, other...);
}

template <typename T, typename ...ARGS>
std::list<T> concat (const std::list<T>& l, const std::list<T>& r,
	ARGS... other)
{
	std::list<T> out = l;
	concat_helper(out, r, other...);
	return out;
}

}

}

#endif // CPPKG_ARRS_LIST_H
