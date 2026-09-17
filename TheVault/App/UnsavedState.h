#pragma once
#include <atomic>

class UnsavedState
{
private:
	std::atomic_bool state;

public:
	UnsavedState()
	{
		state = false;
	}

	void NotifyChange()
	{
		state = true;
	}

	void ClearChange()
	{
		state = false;
	}

	bool HasChanged()
	{
		return state;
	}
};
