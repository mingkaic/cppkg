
#ifndef CPPKG_ESTD_ALGORITHM_H
#define CPPKG_ESTD_ALGORITHM_H

#include <algorithm>

namespace estd
{

template <typename VEC, typename UNARY>
void remove_if (VEC& vec, UNARY pred)
{
	auto pend = std::remove_if(vec.begin(), vec.end(), pred);
	vec.erase(pend, vec.end());
}

}

#endif // CPPKG_ESTD_ALGORITHM_H
