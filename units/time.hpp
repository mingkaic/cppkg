
#ifndef PKG_UNIT_TIME_HPP
#define PKG_UNIT_TIME_HPP

#include "fmts/fmts.hpp"

namespace unit
{

enum TimeUnit
{
	NANO = 0,
	MICRO,
	MILLI,
	CENTI,
	DECI,
	ONE,
	_MAX_SUPPORTED
};

std::string human_readable_time (unsigned long time, TimeUnit unit);

}

#endif // PKG_UNIT_TIME_HPP
