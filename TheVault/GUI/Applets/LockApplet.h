#pragma once
#include "Utils/SecureArray.h"
#include "IApplet.h"

class LockApplet : public IApplet
{
private:
	std::string nameInput;

public:
	LockApplet();
	~LockApplet();

	void OnLeave() override;
	void Render() override;
};
