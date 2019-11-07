#include "logs/logs.hpp"

#ifdef PKG_LOGS_HPP

namespace logs
{

static std::shared_ptr<iLogger> glogger = std::make_shared<DefLogger>();

void set_logger (std::shared_ptr<iLogger> logger)
{
	glogger = logger;
}

iLogger& get_logger (void)
{
	return *glogger;
}

size_t get_log_level (void)
{
	return get_logger().get_logger();
}

void set_log_level (size_t log_level)
{
	get_logger().set_log_level(log_level);
}

void trace (std::string msg)
{
	get_logger().log(TRACE, msg);
}

void debug (std::string msg)
{
	get_logger().log(DEBUG, msg);
}

void info (std::string msg)
{
	get_logger().log(INFO, msg);
}

void warn (std::string msg)
{
	get_logger().warn(msg);
}

void error (std::string msg)
{
	get_logger().error(msg);
}

void fatal (std::string msg)
{
	get_logger().fatal(msg);
}

}

#endif
