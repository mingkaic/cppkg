#include "units/memory.hpp"

#ifdef PKG_UNIT_MEMORY_HPP

namespace unit
{

std::string human_readable_memory (MemoryT m)
{
	double value;
	std::string unit;
	if (m < MEM_1KB)
	{
		value = m;
		unit = "Byte";
	}
	else if (m >= MEM_1GB)
	{
		value = m / double(MEM_1GB);
		unit = "GB";
	}
	else if (m >= MEM_1MB)
	{
		value = m / double(MEM_1MB);
		unit = "MB";
	}
	else
	{
		value = m / double(MEM_1KB);
		unit = "KB";
	}
	return fmts::sprintf("%.3g %s", value, unit.c_str());
}

}

#endif
