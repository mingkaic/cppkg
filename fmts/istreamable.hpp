///
/// istreamable.hpp
/// fmts
///
/// Purpose:
/// Implement iStreamable convenience abstract class taking strings from stream
///

#ifndef PKG_FMTS_ISTREAMABLE_HPP
#define PKG_FMTS_ISTREAMABLE_HPP

#include <sstream>

#include "fmts/ireadable.hpp"

namespace fmts
{

struct iStreamable : public iReadable
{
	virtual ~iStreamable (void) = default;

	std::string to_string () const override
	{
		std::stringstream ss;
		this->read(ss);
		return ss.str();
	}
};

}

#endif // PKG_FMTS_ISTREAMABLE_HPP
