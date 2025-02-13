#pragma once
#include <string>
#include <future>
#include "../../SecureArray.h"
#include "IApplet.h"

class LoginApplet : public IApplet
{
private:
	SecureArray passwordInput;
	std::string hint;
	std::future<uint64_t> vaultTask;

public:
	LoginApplet();
	~LoginApplet();

	void Initialize();
	void OnLeave() override;
	void Render() override;
	void RefreshHintName();
};
