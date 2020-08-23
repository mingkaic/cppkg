#include <limits>

#include "exam/exam.hpp"

#ifdef PKG_EXAM_HPP

namespace exam
{

std::string TestLogger::latest_msg_;

size_t TestLogger::latest_lvl_ = std::numeric_limits<size_t>::max();

std::string TestLogger::set_llevel;

std::string TestLogger::get_llevel = logs::trace_level;

std::shared_ptr<TestLogger> tlogger = std::make_shared<TestLogger>();

}

#endif
