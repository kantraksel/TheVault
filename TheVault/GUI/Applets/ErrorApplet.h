#pragma once
#include <string>
#include "IApplet.h"

class ErrorApplet : public IApplet
{
private:
	std::string errorText;

public:
	ErrorApplet();
	~ErrorApplet();

	void Render() override;

	void SetError(const std::string_view& text);
};
