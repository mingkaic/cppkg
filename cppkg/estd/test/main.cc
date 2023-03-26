#include <memory>

#include "gtest/gtest.h"

#include "cisab/logs/logger.h"

#include "verum/exam/macros.h"

using namespace verum::exam;

using namespace cisab::logs;

int main (int argc, char** argv)
{
	auto tlogger = std::make_shared<MockLogger>();
	set_logger(std::static_pointer_cast<iLogger>(tlogger));

	::testing::InitGoogleTest(&argc, argv);
	auto ret = RUN_ALL_TESTS();
	set_logger(nullptr);
	return ret;
}
