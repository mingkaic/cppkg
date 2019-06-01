#include "gtest/gtest.h"

#include "job/managed_job.hpp"
#include "job/scope_guard.hpp"
#include "job/sequence.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_JOB_TEST


TEST(JOB, ManagedJob)
{
	bool success = false;
	job::ManagedJob managed(
	[](std::future<void> stop_it, bool& success)
	{
		for (size_t attempts = 0;
			stop_it.wait_for(std::chrono::milliseconds(1)) ==
			std::future_status::timeout;
			++attempts)
		{
			if (attempts >= 30)
			{
				// allow at most 30 seconds
				// fail condition
				success = false;
				return;
			}
			std::this_thread::sleep_for(
				std::chrono::milliseconds(1000));
		}
		// absolute win!
		success = true;
	}, std::ref(success));

	std::this_thread::sleep_for(
		std::chrono::milliseconds(100));
	managed.stop();
	managed.join();
	// failure takes at most 30 seconds
	EXPECT_TRUE(success) << "managed job failed to stop" << std::endl;
}


TEST(JOB, ScopeGuards)
{
	bool expect = false;
	{
		job::ScopeGuard defer(
		[&]()
		{
			expect = true;
		});
		EXPECT_FALSE(expect);
	}
	EXPECT_TRUE(expect);
}


TEST(JOB, SequenceOrdering)
{
	job::Sequence seq;
	size_t i = 0;

	// job 1
	seq.attach_job(
	[](std::future<void> dependency, std::future<void> stop_it, size_t& i)
	{
		if (dependency.valid())
		{
			dependency.get(); // wait for dependency completion
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(5000)); // wait longer job 2
		i += 1; // test
	}, std::ref(i));

	// job 2
	seq.attach_job(
	[](std::future<void> dependency, std::future<void> stop_it, size_t& i)
	{
		if (dependency.valid())
		{
			dependency.get(); // wait for dependency completion
		}
		i *= 2; // test
		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000));
	}, std::ref(i));

	seq.join();
	// without sequence, job 2 is more likely to run before job 1
	// if job 2 ran before job 1, i = 0 * 2 + 1 instead of i = (0 + 1) * 2
	EXPECT_EQ(2, i) << "sequence didn't execute properly:" <<
		"{0: 'job 1 did not run', 1: 'job 2 ran before job 1'}";
}


TEST(JOB, SequenceTermination)
{
	job::Sequence seq;
	bool job1_succ = false;
	bool job2_succ = false;

	// job 1
	seq.attach_job(
	[](std::future<void> dependency, std::future<void> stop_it, bool& success)
	{
		if (dependency.valid())
		{
			dependency.get(); // wait for dependency completion
		}
		for (size_t attempts = 0;
			stop_it.wait_for(std::chrono::milliseconds(1)) ==
			std::future_status::timeout;
			++attempts)
		{
			if (attempts >= 20)
			{
				// allow at most 20 seconds
				// fail condition
				success = false;
				return;
			}
			std::this_thread::sleep_for(
				std::chrono::milliseconds(1000));
		}
		// absolute win!
		success = true;
	}, std::ref(job1_succ));

	// job 2
	seq.attach_job(
	[](std::future<void> dependency, std::future<void> stop_it, bool& success)
	{
		if (dependency.valid())
		{
			dependency.get(); // wait for dependency completion
		}
		for (size_t attempts = 0;
			stop_it.wait_for(std::chrono::milliseconds(1)) ==
			std::future_status::timeout;
			++attempts)
		{
			if (attempts >= 20)
			{
				// allow at most 20 seconds
				// fail condition
				success = false;
				return;
			}
			std::this_thread::sleep_for(
				std::chrono::milliseconds(1000));
		}
		// absolute win!
		success = true;
	}, std::ref(job2_succ));

	std::this_thread::sleep_for(
		std::chrono::milliseconds(100));
	seq.stop();
	seq.join();
	// failure takes at most 40 seconds
	EXPECT_TRUE(job1_succ) << "job 1 failed to stop before 20 seconds";
	EXPECT_TRUE(job2_succ) << "job 2 failed to stop before 20 seconds";
}


#endif // DISABLE_JOB_TEST
