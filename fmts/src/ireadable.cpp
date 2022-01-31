#include "fmts/ireadable.hpp"

#ifdef PKG_FMTS_IREADABLE_HPP

namespace fmts
{

std::ostream& operator << (std::ostream& os, const iReadable& reader)
{
	reader.read(os);
	return os;
}

std::ostream& operator << (std::ostream& os, const iReadable* reader)
{
	if (nullptr == reader)
	{
		os << "<nil>";
	}
	else
	{
		os << *reader;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, std::shared_ptr<iReadable> reader)
{
	os << reader.get();
	return os;
}

}

#endif
