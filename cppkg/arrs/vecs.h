
#ifndef CPPKG_ARRS_VECS_H
#define CPPKG_ARRS_VECS_H

#include <numeric>
#include <vector>

namespace cppkg
{

namespace arrs
{

template <typename T>
std::vector<T> range (T begin, T end)
{
	std::vector<T> out(end - begin);
	std::iota(out.begin(), out.end(), begin);
	return out;
}

template <typename T>
void lconcat (std::vector<T>& l, const std::vector<T>& r)
{
	l.insert(l.end(), r.begin(), r.end());
}

template <typename T>
void rconcat (const std::vector<T>& l, std::vector<T>& r)
{
	r.insert(r.begin(), l.begin(), l.end());
}

template <typename T>
std::vector<T> concat (const std::vector<T>& l, const std::vector<T>& r)
{
	std::vector<T> out = l;
	lconcat(out, r);
	return out;
}

template <typename T, typename ...ARGS>
static void concat_helper (std::vector<T>& source, const std::vector<T>& r)
{
	lconcat(source, r);
}

template <typename T, typename ...ARGS>
static void concat_helper (std::vector<T>& source, const std::vector<T>& r,
	ARGS... other)
{
	lconcat(source, r);
	concat_helper(source, other...);
}

template <typename T, typename ...ARGS>
std::vector<T> concat (const std::vector<T>& l, const std::vector<T>& r,
	ARGS... other)
{
	std::vector<T> out = l;
	concat_helper(out, r, other...);
	return out;
}

}

}

#endif // CPPKG_ARRS_VECS_H
