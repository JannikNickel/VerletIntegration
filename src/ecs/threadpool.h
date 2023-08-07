#pragma once
#include <cstdint>
#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <future>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	std::future<void> QueueJob(std::function<void()> job);
	uint32_t ThreadCount();

private:
	std::vector<std::thread> workerThreads = {};
	std::queue<std::function<void()>> jobQueue = {};
	bool stop = false;
	std::mutex lock = {};
	std::condition_variable condVar = {};

	void Worker();
};
