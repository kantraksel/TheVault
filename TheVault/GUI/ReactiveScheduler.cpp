#include "ReactiveScheduler.hpp"

ReactiveScheduler::ReactiveScheduler()
{
}

ReactiveScheduler::~ReactiveScheduler()
{
}

void ReactiveScheduler::Run()
{
	std::unique_lock lock(taskMutex);
	std::erase_if(tasks, [&](const std::function<bool()>& task)
		{
			lock.unlock();
			bool result = task();
			lock.lock();
			return result;
		});
}

void ReactiveScheduler::Schedule(const Function<bool()>& callback)
{
	std::lock_guard lock(taskMutex);
	tasks.push_back(callback);
}
