///
/// logs.hpp
/// logs
///
/// Purpose:
/// Define log handling and interface
///

#include <iostream>
#include <memory>

#include "fmts/fmts.hpp"

#ifndef PKG_LOGS_HPP
#define PKG_LOGS_HPP

namespace logs
{

/// Interface of logger
struct iLogger
{
	virtual ~iLogger (void) = default;

	/// Log message at any specified level of verbosity
	virtual void log (size_t msg_level, std::string msg) const = 0;

	virtual size_t get_log_level (void) const = 0;

	virtual void set_log_level (size_t log_level) = 0;

	/// Warn user of message regarding poor decisions
	virtual void warn (std::string msg) const = 0;

	/// Notify user of message regarding recoverable error
	virtual void error (std::string msg) const = 0;

	/// Notify user of message regarding fatal error, then finish him
	virtual void fatal (std::string msg) const = 0;
};

/// String tagged prepending a warning message in default logger
const std::string warn_tag = "[WARNING]:";

/// String tagged prepending an error message in default logger
const std::string err_tag = "[ERROR]:";

enum LOG_LEVEL
{
	FATAL = 0,
	ERROR,
	WARN,
	INFO,
	DEBUG,
	TRACE,
};

/// Default implementation of iLogger used in ADE
struct DefLogger final : public iLogger
{
	void log (size_t msg_level, std::string msg) const override
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

	size_t get_log_level (void) const override
	{
		return log_level_;
	}

	void set_log_level (size_t log_level) override
	{
		log_level_ = (LOG_LEVEL) log_level;
	}

	/// Implementation of iLogger
	void warn (std::string msg) const override
	{
		if (WARN <= log_level_)
		{
			std::cerr << warn_tag << msg << '\n';
		}
	}

	/// Implementation of iLogger
	void error (std::string msg) const override
	{
		if (ERROR <= log_level_)
		{
			std::cerr << err_tag << msg << '\n';
		}
	}

	/// Implementation of iLogger
	void fatal (std::string msg) const override
	{
		if (FATAL <= log_level_)
		{
			throw std::runtime_error(msg);
		}
	}

	LOG_LEVEL log_level_ = INFO;
};

/// Set input logger for ADE global logger
void set_logger (std::shared_ptr<iLogger> logger);

/// Get reference to ADE global logger
const iLogger& get_logger (void);

/// Log at trace level using global logger
void trace (std::string msg);

/// Log at debug level using global logger
void debug (std::string msg);

/// Log at info level using global logger
void info (std::string msg);

/// Warn using global logger
void warn (std::string msg);

/// Error using global logger
void error (std::string msg);

/// Fatal using global logger
void fatal (std::string msg);

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
