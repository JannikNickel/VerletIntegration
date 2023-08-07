#include "threadpool.h"

ThreadPool::ThreadPool()
{
    uint32_t threads = std::thread::hardware_concurrency();
	for(uint32_t i = 0; i < threads; i++)
	{
		std::thread t = std::thread(&ThreadPool::Worker, this);
		workerThreads.push_back(std::move(t));
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> l { lock };
		stop = true;
		condVar.notify_all();
	}

	for(std::thread& t : workerThreads)
	{
		t.join();
	}
}

std::future<void> ThreadPool::QueueJob(std::function<void()> job)
{
	std::shared_ptr<std::packaged_task<void()>> task = std::make_shared<std::packaged_task<void()>>(job);
	std::function<void()> wrapper = [task]()
	{
		(*task)();
	};

	std::unique_lock<std::mutex> l { lock };
	jobQueue.push(wrapper);
	condVar.notify_one();

	return task->get_future();
}

uint32_t ThreadPool::ThreadCount()
{
	return workerThreads.size();
}

void ThreadPool::Worker()
{
	std::function<void()> job;

	while(true)
	{
		{
			std::unique_lock<std::mutex> l { lock };

			condVar.wait(l, [this] { return stop || !jobQueue.empty(); });

			if(stop)
			{
				break;
			}

			if(jobQueue.empty())
			{
				continue;
			}

			job = std::move(jobQueue.front());
			jobQueue.pop();
		}

		job();
	}
}
