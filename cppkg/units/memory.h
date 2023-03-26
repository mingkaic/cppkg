
#ifndef CPPKG_UNIT_MEMORY_H
#define CPPKG_UNIT_MEMORY_H

#include "cisab/fmts/format.h"

namespace cppkg
{

namespace unit
{

using MemoryT = size_t; // bytes

inline MemoryT KB (MemoryT m)
{
	return m * 1e+3;
}

inline MemoryT MB (MemoryT m)
{
	return m * 1e+6;
}

inline MemoryT GB (MemoryT m)
{
	return m * 1e+9;
}

const MemoryT MEM_1KB = KB(1);

const MemoryT MEM_1MB = MB(1);

const MemoryT MEM_1GB = GB(1);

std::string human_readable_memory (MemoryT m);

}

}

#endif // CPPKG_UNIT_MEMORY_H
