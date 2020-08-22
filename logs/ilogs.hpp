
#ifndef PKG_LOGS_ILOGS_HPP
#define PKG_LOGS_ILOGS_HPP

#include <string>

namespace logs
{

/// Interface of logger
struct iLogger
{
	virtual ~iLogger (void) = default;

	/// Get log level encoding specifying verbosity
	virtual std::string get_log_level (void) const = 0;

	/// Set log level encoding specifying verbosity
	virtual void set_log_level (const std::string& log_level) = 0;

	/// Return true if string level is supported
	virtual bool supports_level (size_t msg_level) const = 0;

	/// Return true if string level is supported
	virtual bool supports_level (const std::string& msg_level) const = 0;

	/// Log message at any specified enum level of verbosity
	virtual void log (size_t msg_level, const std::string& msg) = 0;

	/// Log message at any specified string level of verbosity
	virtual void log (const std::string& msg_level, const std::string& msg) = 0;
};

}

#endif // PKG_LOGS_ILOGS_HPP
