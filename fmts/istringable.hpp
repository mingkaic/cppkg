///
/// istringable.hpp
/// fmts
///
/// Purpose:
/// Implement iStringable convenience abstract class writing stream from string
///

#ifndef PKG_FMTS_ISTRINGABLE_HPP
#define PKG_FMTS_ISTRINGABLE_HPP

#include "fmts/ireadable.hpp"

namespace fmts
{

struct iStringable : public iReadable
{
	virtual ~iStringable (void) = default;

	void read (std::ostream& out) const override
	{
		out << this->to_string();
	}
};

}

#endif // PKG_FMTS_ISTRINGABLE_HPP
