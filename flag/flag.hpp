///
/// flag.hpp
/// flag
///
/// Purpose:
/// Define flag option parsing with builtin help option
///

#include <boost/program_options.hpp>

#include "logs/logs.hpp"

#ifndef PKG_FLAG_HPP
#define PKG_FLAG_HPP

namespace flag
{

namespace opt = boost::program_options;

/// Set of flag description and variable maps
struct FlagSet
{
	FlagSet (const char* prog);

	/// Return true if arguments are successfully parsed according to args_
	/// --help flag will logs::warn with help message and return false
	/// Any error will logs::error and return false
	bool parse(int argc, char** argv);

	/// Boost options
	/// [FlagSet instance].args_.add_options()
	///		("[flag name]", flag::opt::value<[flat-type]>([output pointer])->
	///			default_value([default value]), "[description]")...
	/// See Boost Documentation for more information:
	/// https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
	opt::options_description args_;
};

}

#endif // PKG_FLAG_HPP
