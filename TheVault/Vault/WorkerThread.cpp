#include "WorkerThread.h"
#include "App/Game.h"

extern Game game;
using Future = WorkerThread::Future;

struct Task
{
	std::function<uint64_t()> func;
	std::promise<uint64_t> promise;
};

WorkerThread::WorkerThread()
{
}

WorkerThread::~WorkerThread()
{
	Shutdown();
}

void WorkerThread::Init()
{
	if (thread.joinable())
		return;
	using namespace std::placeholders;
	thread = std::jthread([this](const std::stop_token& token)
		{
			Run(token);
		});
}

void WorkerThread::Shutdown()
{
	thread.request_stop();
	threadCvar.notify_one();

	if (thread.joinable())
		thread.join();
	thread = {};

	tasks.clear();
}

void WorkerThread::Run(const std::stop_token& token)
{
	std::unique_lock lock(taskMutex);

	while (true)
	{
		if (token.stop_requested())
			return;

		if (tasks.empty())
		{
			threadCvar.wait(lock);
			if (tasks.empty())
				continue;

			if (token.stop_requested())
				return;
		}
		auto& task = tasks.front();

		lock.unlock();
		uint64_t value = task.func();
		lock.lock();

		task.promise.set_value(value);
		tasks.pop_front();
	}
}

Future WorkerThread::SendCmd(const std::function<uint64_t()>& func)
{
	Task task
	{
		.func = [=]() -> uint64_t
		{
			if (func)
				return func();
			return 0;
		},
	};
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}

Future WorkerThread::SendCmd(const std::function<uint64_t(const SecureArray&)>& func, const SecureArray& arg)
{
	auto argCopy = SecureArray::Copy(arg);

	Task task
	{
		.func = [=]() -> uint64_t
		{
			if (func)
				return func(argCopy);
			return 0;
		},
	};
	auto future = task.promise.get_future();

	{
		std::lock_guard lock(taskMutex);
		tasks.push_back(std::move(task));
	}
	threadCvar.notify_one();
	return future;
}
