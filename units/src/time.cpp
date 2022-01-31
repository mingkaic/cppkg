#include "units/time.hpp"

#ifdef PKG_UNIT_TIME_HPP

namespace unit
{

static const std::vector<std::string> time_unit_fmt = {
	"ns",
	"μs",
	"ms",
	"cs",
	"ds",
	"s",
};

static const std::vector<int> time_incr = {
	1000, // NANO -> MICRO
	1000, // MICRO -> MILLI
	10, // MILLI -> CENTI
	10, // CENTI -> DECI
	10, // DECI -> ONE
};

std::string human_readable_time (unsigned long time, TimeUnit unit)
{
	unsigned long denom = 1;
	size_t u = unit;
	for (; u < TimeUnit::_MAX_SUPPORTED-1 && (time / denom) >= time_incr[u]; ++u)
	{
		denom *= time_incr[u];
	}
	double d;
	if (denom >= 10000)
	{
		d = unsigned(time / (denom / 10000));
		d /= 10000;
	}
	else
	{
		d = time;
	}
	return fmts::sprintf("%.5g%s", d, time_unit_fmt[u].c_str());
}

}

#endif
