#include "logs/logs.hpp"

#ifdef PKG_LOGS_HPP

namespace logs
{

static std::shared_ptr<iLogger> glogger = std::make_shared<DefLogger>();

void set_logger (std::shared_ptr<iLogger> logger)
{
	glogger = logger;
}

const iLogger& get_logger (void)
{
	return *glogger;
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
