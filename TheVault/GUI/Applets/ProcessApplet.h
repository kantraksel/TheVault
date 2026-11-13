#pragma once
#include <future>
#include <string>

class ProcessApplet
{
private:
	std::future<uint64_t> vaultTask;
	std::string title;
	bool isOpen;

public:
	ProcessApplet();
	~ProcessApplet();

	void Open(std::future<uint64_t>&& task, const std::string_view& title);
	void Close();
	void Render();
};
