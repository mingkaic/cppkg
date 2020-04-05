///
/// logs.hpp
/// logs
///
/// Purpose:
/// Define log handling and interface
///

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "fmts/fmts.hpp"

#ifndef PKG_LOGS_HPP
#define PKG_LOGS_HPP

namespace logs
{

/// Interface of logger
struct iLogger
{
	virtual ~iLogger (void) = default;

	/// Return true if log level is supported
	virtual bool supports_level (const std::string& msg_level) const = 0;

	/// Log message at any specified string level of verbosity
	virtual void log (const std::string& msg_level, const std::string& msg) const = 0;

	/// Log message at any specified enum level of verbosity
	virtual void log (size_t msg_level, const std::string& msg) const = 0;

	/// Get log level encoding specifying verbosity
	virtual std::string get_log_level (void) const = 0;

	/// Set log level encoding specifying verbosity
	virtual void set_log_level (const std::string& log_level) = 0;

	/// Warn user of message regarding poor decisions
	virtual void warn (const std::string& msg) const = 0;

	/// Notify user of message regarding recoverable error
	virtual void error (const std::string& msg) const = 0;

	/// Notify user of message regarding fatal error, then finish him
	virtual void fatal (const std::string& msg) const = 0;
};

/// String tagged prepending a warning message in default logger
const std::string warn_tag = "[WARNING]:";

/// String tagged prepending an error message in default logger
const std::string err_tag = "[ERROR]:";

/// Log verbosity encoding
enum LOG_LEVEL
{
	FATAL = 0,
	ERROR,
	WARN,
	INFO,
	DEBUG,
	TRACE,
	NOT_SET,
};

static std::unordered_map<std::string,LOG_LEVEL> names2log =
{
	{"fatal", FATAL},
	{"error", ERROR},
	{"warn", WARN},
	{"info", INFO},
	{"debug", DEBUG},
	{"trace", TRACE},
};

static fmts::StringsT lognames = {
	"fatal", "error", "warn", "info", "debug", "trace"};

std::string name_log (const LOG_LEVEL& level);

LOG_LEVEL enum_log (const std::string& level);

/// Default implementation of iLogger used in ADE
struct DefLogger final : public iLogger
{
	bool supports_level (const std::string& msg_level) const override
	{
		std::string level = msg_level;
		std::transform(level.begin(), level.end(), level.begin(),
			[](unsigned char c){ return std::tolower(c); });
		auto it = names2log.find(level);
		return names2log.end() != it;
	}

	/// Implementation of iLogger
	void log (const std::string& msg_level, const std::string& msg) const override
	{
		LOG_LEVEL level = TRACE;
		auto it = names2log.find(msg_level);
		if (names2log.end() != it)
		{
			level = it->second;
		}
		if (level <= log_level_)
		{
			switch (level)
			{
				case FATAL:
					fatal(msg);
				case ERROR:
					error(msg);
					break;
				case WARN:
					warn(msg);
					break;
				default:
					std::cout << msg << '\n';
			}
		}
	}

	/// Implementation of iLogger
	void log (size_t msg_level, const std::string& msg) const override
	{
		if (msg_level <= log_level_)
		{
			switch (msg_level)
			{
				case FATAL:
					fatal(msg);
				case ERROR:
					error(msg);
					break;
				case WARN:
					warn(msg);
					break;
				default:
					std::cout << msg << '\n';
			}
		}
	}

	/// Implementation of iLogger
	std::string get_log_level (void) const override
	{
		return lognames[log_level_];
	}

	/// Implementation of iLogger
	void set_log_level (const std::string& log_level) override
	{
		auto it = names2log.find(log_level);
		if (names2log.end() != it)
		{
			log_level_ = names2log.at(log_level);
		}
	}

	/// Implementation of iLogger
	void warn (const std::string& msg) const override
	{
		if (WARN <= log_level_)
		{
			std::cerr << warn_tag << msg << '\n';
		}
	}

	/// Implementation of iLogger
	void error (const std::string& msg) const override
	{
		if (ERROR <= log_level_)
		{
			std::cerr << err_tag << msg << '\n';
		}
	}

	/// Implementation of iLogger
	void fatal (const std::string& msg) const override
	{
		throw std::runtime_error(msg);
	}

	/// Logging levels above this log_level_ are ignored
	LOG_LEVEL log_level_ = INFO;
};

/// Set input logger for ADE global logger
void set_logger (std::shared_ptr<iLogger> logger);

/// Get reference to ADE global logger
iLogger& get_logger (void);

/// Return log level used by global logger
std::string get_log_level (void);

/// Set log level using global logger
void set_log_level (const std::string& log_level);

/// Log at trace level using global logger
void trace (const std::string& msg);

/// Log at debug level using global logger
void debug (const std::string& msg);

/// Log at info level using global logger
void info (const std::string& msg);

/// Warn using global logger
void warn (const std::string& msg);

/// Error using global logger
void error (const std::string& msg);

/// Fatal using global logger
void fatal (const std::string& msg);

/// Log at trace level using global logger with arguments
template <typename... ARGS>
void tracef (std::string format, ARGS... args)
{
	trace(fmts::sprintf(format, args...));
}

/// Log at debug level using global logger with arguments
template <typename... ARGS>
void debugf (std::string format, ARGS... args)
{
	debug(fmts::sprintf(format, args...));
}

/// Log at info level using global logger with arguments
template <typename... ARGS>
void infof (std::string format, ARGS... args)
{
	info(fmts::sprintf(format, args...));
}

/// Warn using global logger with arguments
template <typename... ARGS>
void warnf (std::string format, ARGS... args)
{
	warn(fmts::sprintf(format, args...));
}

/// Error using global logger with arguments
template <typename... ARGS>
void errorf (std::string format, ARGS... args)
{
	error(fmts::sprintf(format, args...));
}

/// Fatal using global logger with arguments
template <typename... ARGS>
void fatalf (std::string format, ARGS... args)
{
	fatal(fmts::sprintf(format, args...));
}

}

#endif // PKG_LOGS_HPP
