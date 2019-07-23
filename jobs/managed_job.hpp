///
/// managed_job.hpp
/// jobs
///
/// Purpose:
/// Define thread wrapper that can terminate underlying thread
///

#include <functional>
#include <thread>
#include <future>

#ifndef PKG_JOBS_MANAGED_HPP
#define PKG_JOBS_MANAGED_HPP

namespace jobs
{

/// Thread wrapper that offers termination option
struct ManagedJob final
{
	ManagedJob (void) = default;

	template <typename FN, typename ...ARGS>
	ManagedJob (FN&& job, ARGS&&... args)
	{
		std::thread job_thd(job,
			exit_signal_.get_future(),
			std::forward<ARGS>(args)...);
		job_ = std::move(job_thd);
	}

	~ManagedJob (void)
	{
		if (job_.joinable())
		{
			exit_signal_.set_value();
			job_.detach();
		}
	}

	ManagedJob (const ManagedJob& other) = delete;

	ManagedJob (ManagedJob&& other) :
		exit_signal_(std::move(other.exit_signal_)),
		job_(std::move(other.job_)) {}

	ManagedJob& operator = (const ManagedJob& other) = delete;

	ManagedJob& operator = (ManagedJob&& other)
	{
		if (this != &other)
		{
			if (job_.joinable())
			{
				exit_signal_.set_value();
				job_.detach();
			}
			exit_signal_ = std::move(other.exit_signal_);
			job_ = std::move(other.job_);
		}
		return *this;
	}

	/// Return thread id
	std::thread::id get_id (void) const
	{
		return job_.get_id();
	}

	/// Return true if job is running
	/// otherwise false
	bool is_running (void) const
	{
		// since job is never detached,
		// if job is running, then it's joinable
		return job_.joinable();
	}

	/// Join the job if it's joinable
	void join (void)
	{
		if (job_.joinable())
		{
			job_.join();
		}
	}

	/// Stop the job_
	void stop (void)
	{
		if (job_.joinable())
		{
			exit_signal_.set_value();
		}
	}

private:
	std::promise<void> exit_signal_;

	std::thread job_;
};

}

#endif // PKG_JOBS_MANAGED_HPP