#pragma once
#include <future>
#include "Engine/Logger.h"
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
		Schedule([=]()
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
						catch (const std::exception& e)
						{
							Logger::LogError("Exception in resolve callback: {}", e.what());
						}
					}
					catch (const std::exception& e)
					{
						try
						{
							if (rejectCallback)
								rejectCallback(e);
							else
								Logger::LogError("Unhandled rejection: {}", e.what());
						}
						catch (const std::exception& e)
						{
							Logger::LogError("Exception in reject callback: {}", e.what());
						}
					}
					return true;
				}
				return false;
			});
	}
};
