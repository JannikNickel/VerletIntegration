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
}

void ThreadPool::WaitForCompletion()
{
	while(unfinishedJobs > 0)
	{
		std::this_thread::yield();
	}
}

uint32_t ThreadPool::ThreadCount()
{
	return workerThreads.size();
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
		unfinishedJobs--;
	}
}
