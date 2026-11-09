#pragma once
#include <future>
#include "Utility/Function.h"

class ReactiveScheduler
{
private:
	std::list<Function<bool()>> tasks;
	std::recursive_mutex taskMutex;

public:
	ReactiveScheduler();
	~ReactiveScheduler();

	void Run();
	void Schedule(const Function<bool()>& callback);

	template <class T>
	void Schedule(std::future<T>&& future, const Function<void(const T&)>& resolveCallback, const Function<void(const std::exception&)>& rejectCallback)
	{
		auto futurePtr = std::make_shared<std::future<T>>(std::move(future));
		std::lock_guard lock(taskMutex);
		tasks.push_back([=]()
			{
				auto& future = *futurePtr;
				auto status = future.wait_for(std::chrono::seconds(0));
				if (status == std::future_status::ready)
				{
					try
					{
						auto response = future.get();
						try
						{
							resolveCallback(response);
						}
						catch (...)
						{
						}
					}
					catch (const std::exception& e)
					{
						if (rejectCallback)
							rejectCallback(e);
					}
					return true;
				}
				return false;
			});
	}
};
