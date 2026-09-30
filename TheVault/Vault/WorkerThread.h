#pragma once
#include <thread>
#include <condition_variable>
#include <list>
#include <functional>
#include <future>
#include "Utils/SecureArray.h"

class WorkerThread
{
public:
	typedef std::future<uint64_t> Future;

private:
	std::jthread thread;
	std::condition_variable threadCvar;
	std::mutex taskMutex;
	std::list<struct Task> tasks;

	void Run(const std::stop_token& token);

public:
	WorkerThread();
	~WorkerThread();

	void Init();
	void Shutdown();

	Future SendCmd(const std::function<uint64_t()>& f);
	Future SendCmd(const std::function<uint64_t(const SecureArray&)>& f, const SecureArray& arg);
};
