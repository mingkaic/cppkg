#include "exam/exam.hpp"

#ifdef PKG_EXAM_HPP

namespace exam
{

std::string TestLogger::latest_msg_;

size_t TestLogger::latest_lvl_ = logs::TRACE + 1;

std::shared_ptr<TestLogger> tlogger = std::make_shared<TestLogger>();

}

#endif
