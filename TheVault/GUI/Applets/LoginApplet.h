#pragma once
#include <string>
#include <future>
#include "Utils/SecureArray.h"
#include "IApplet.h"

class LoginApplet : public IApplet
{
private:
	SecureArray passwordInput;
	std::string hint;
	std::future<uint64_t> vaultTask;
	bool firstFrameDraw;

public:
	LoginApplet();
	~LoginApplet();

	void Initialize();
	void OnEnter() override;
	void OnLeave() override;
	void Render() override;
	void RefreshHintName();
};
