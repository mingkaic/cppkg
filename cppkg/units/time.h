#ifndef CPPKG_UNIT_TIME_H
#define CPPKG_UNIT_TIME_H

namespace cppkg
{

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

}

#endif // CPPKG_UNIT_TIME_H
