#pragma once
#include <future>
#include "IApplet.h"

class ProcessApplet : public IApplet
{
private:
	std::future<uint64_t> vaultTask;
	std::string title;

public:
	ProcessApplet();
	~ProcessApplet();

	void Render() override;
	void ProcessVaultTask(std::future<uint64_t>&& task);
	void SetTitle(const std::string_view& title);
};
