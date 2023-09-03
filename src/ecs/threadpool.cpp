#include "threadpool.h"
#include <iostream>

ThreadPool::ThreadPool()
{
	uint32_t threads = std::thread::hardware_concurrency();
	jobSlots = new JobSlot[threads];
	condVars = std::vector<AutoResetEvent>(threads);
	for(uint32_t i = 0; i < threads; i++)
	{
		jobSlots[i].free = true;
		std::thread t = std::thread(&ThreadPool::Worker, this, i);
		workerThreads.push_back(std::move(t));
	}
}

ThreadPool::~ThreadPool()
{
	stop = true;

	for(size_t i = 0; i < workerThreads.size(); i++)
	{
		condVars[i].Set();
	}
	for(std::thread& t : workerThreads)
	{
		t.join();
	}
}

void ThreadPool::EnqueueJob(std::function<void()>&& job)
{
	while(!jobSlots[slotIndex].free)
	{
		slotIndex = (slotIndex + 1) % workerThreads.size();
	}
	jobSlots[slotIndex].job = std::move(job);
	jobSlots[slotIndex].free = false;
	unfinishedJobs++;
	condVars[slotIndex].Set();
	waitEvent.Reset();
}

void ThreadPool::WaitForCompletion()
{
	if(unfinishedJobs.load() > 0)
	{
		waitEvent.WaitOne();
	}
}

uint32_t ThreadPool::ThreadCount()
{
	return workerThreads.size();
}

std::vector<std::pair<size_t, size_t>> ThreadPool::SplitWork(size_t workAmount, size_t workerAmount)
{
	size_t sectionSize = workAmount / workerAmount;
	size_t remainder = workAmount % workerAmount;
	std::vector<std::pair<size_t, size_t>> ranges = {};
	ranges.reserve(workerAmount);

	size_t start = 0;
	for(size_t i = 0; i < workerAmount; i++)
	{
		size_t end = start + sectionSize + static_cast<size_t>(i < remainder);
		ranges.emplace_back(start, end - start);
		start = end;
	}

	return ranges;
}

void ThreadPool::Worker(size_t index)
{
	while(true)
	{
		condVars[index].WaitOne();

		if(stop)
		{
			break;
		}

		if(jobSlots[index].free)
		{
			continue;
		}

		JobSlot& slot = jobSlots[index];
		try
		{
			slot.job();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what();
		}
		slot.free = true;

		if((unfinishedJobs.fetch_sub(1) - 1) == 0)
		{
			waitEvent.Set();
		}
	}
}
