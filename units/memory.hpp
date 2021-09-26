
#ifndef PKG_UNIT_MEMORY_HPP
#define PKG_UNIT_MEMORY_HPP

#include "fmts/fmts.hpp"

namespace unit
{

using MemoryT = size_t; // bytes

inline MemoryT KB (MemoryT m)
{
	return m * 10e+2;
}

inline MemoryT MB (MemoryT m)
{
	return m * 10e+5;
}

inline MemoryT GB (MemoryT m)
{
	return m * 10e+8;
}

const MemoryT MEM_1KB = KB(1);

const MemoryT MEM_1MB = MB(1);

const MemoryT MEM_1GB = GB(1);

std::string human_readable_memory (MemoryT m);

}

#endif // PKG_UNIT_MEMORY_HPP
