#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>

class AutoResetEvent
{
public:
	AutoResetEvent(const AutoResetEvent&) = delete;
	AutoResetEvent& operator=(const AutoResetEvent&) = delete;

	AutoResetEvent(bool initial = false) : set(initial)
	{
		
	}

	void Set()
	{
		std::lock_guard<std::mutex> l { mtx };
		set = true;
		signal.notify_one();
	}

	void Reset()
	{
		std::lock_guard<std::mutex> l { mtx };
		set = false;
	}

	bool WaitOne()
	{
		std::unique_lock<std::mutex> l { mtx };
		while(!set)
		{
			signal.wait(l);
		}
		set = false;
		return true;
	}

private:
	bool set;
	std::mutex mtx;
	std::condition_variable signal;
};
