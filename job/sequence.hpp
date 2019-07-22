#include <functional>
#include <thread>
#include <future>

#ifndef PKG_JOB_SEQUENCE_HPP
#define PKG_JOB_SEQUENCE_HPP

namespace job
{

struct Sequence final
{
	~Sequence (void)
	{
		if (dependency_.valid())
		{
			for (auto& exit_signal : exit_signals_)
			{
				try
				{
					exit_signal.set_value();
				}
				catch (...) {}
			}
			dependency_.wait();
		}
	}

	template <typename FN, typename ...ARGS>
	void attach_job (FN&& call, ARGS&&... args)
	{
		// create a job that retries sending creation request
		std::packaged_task<void(
			std::future<void>,std::future<void>,ARGS...)> tsk(
			[call](
				std::future<void> dependency,
				std::future<void> stop_it, ARGS... args)
			{
				call(std::move(dependency), std::move(stop_it),
					std::forward<ARGS>(args)...);
			});

		std::promise<void> exit_sig;
		auto killer = exit_sig.get_future();
		auto dep = tsk.get_future();
		std::thread job(std::move(tsk), std::move(dependency_),
			std::move(killer), std::forward<ARGS>(args)...);
		dependency_ = std::move(dep);
		exit_signals_.push_back(std::move(exit_sig));
		job.detach();
	}

	/// return whether any job is running
	bool is_running (void) const
	{
		return dependency_.valid();
	}

	/// join if joinable
	void join (void)
	{
		if (dependency_.valid())
		{
			dependency_.wait();
		}
	}

	/// stop the job_
	void stop (void)
	{
		if (dependency_.valid())
		{
			for (auto& exit_signal : exit_signals_)
			{
				exit_signal.set_value();
			}
		}
	}

private:
	std::future<void> dependency_;

	std::vector<std::promise<void>> exit_signals_; // force all threads to exit
};

}

#endif // PKG_JOB_SEQUENCE_HPP
