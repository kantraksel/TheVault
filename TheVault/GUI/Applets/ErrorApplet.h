#pragma once
#include <mutex>
#include "IApplet.h"

class ErrorApplet : public IApplet
{
private:
	std::mutex errorMutex;
	std::string errorText;
	bool openErrorModal;

public:
	ErrorApplet();
	~ErrorApplet();

	void Render() override;
	void RenderModal();

	void SetError(const std::string_view& text, bool openModal);
};
