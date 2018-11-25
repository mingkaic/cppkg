#include <boost/filesystem.hpp>

#include "flag/flag.hpp"

#ifdef PKG_FLAG_HPP

namespace flag
{

namespace fs = boost::filesystem;

static std::string make_usage_string (const std::string& program_name,
	const opt::options_description& desc,
	opt::positional_options_description& pos)
{
	std::stringstream usage;
	usage << "usage: ";
	usage << program_name << ' ';
	size_t N = pos.max_total_count();
	if(N == std::numeric_limits<unsigned>::max())
	{
		std::string arg;
		std::string last = pos.name_for_position(
			std::numeric_limits<unsigned>::max());
		for(size_t i = 0; arg != last; ++i)
		{
			arg = pos.name_for_position(i);
			usage << arg << ' ';
		}
		usage << '[' << arg << "] ";
		usage << "... ";
	}
	else
	{
		for(size_t i = 0; i < N; ++i)
		{
			usage << pos.name_for_position(i) << ' ';
		}
	}
	if(desc.options().size() > 0)
	{
		usage << "[options]";
	}
	usage << '\n' << desc;
	return usage.str();
}

FlagSet::FlagSet (const char* prog) :
	args_(fmts::sprintf("%s options", prog)) {}

bool FlagSet::parse(int argc, char** argv)
{
	std::vector<std::string> config_fnames;
	args_.add_options()
		("help", "Display help message");

	opt::options_description all_options;
	all_options.add(args_);

	try
	{
		opt::variables_map vars;
		opt::positional_options_description pos;
		opt::store(opt::command_line_parser(argc, argv).
			options(all_options).positional(pos).run(), vars);

		if (vars.count("help"))
		{
			logs::warn(make_usage_string(boost::filesystem::path(
				argv[0]).stem().string(), args_, pos));
			return false;
		}

		opt::notify(vars);
	}
	catch (opt::error& e)
	{
		logs::error(e.what());
		return false;
	}

	return true;
}

}

#endif
