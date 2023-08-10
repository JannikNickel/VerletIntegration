#pragma once
#include "autoresetevent.h"
#include <cstdint>
#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

class ThreadPool
{
	struct JobSlot
	{
		std::atomic<bool> free = false;
		std::function<void()> job;
	};

public:
	ThreadPool();
	~ThreadPool();

	void EnqueueJob(std::function<void()>&& job);
	void WaitForCompletion();
	uint32_t ThreadCount();

private:
	std::vector<std::thread> workerThreads = {};
	JobSlot* jobSlots;
	std::vector<AutoResetEvent> condVars;
	std::atomic<size_t> slotIndex = 0;
	std::atomic<size_t> unfinishedJobs = 0;
	std::atomic<bool> stop = false;
	AutoResetEvent waitEvent;

	void Worker(size_t index);
};
