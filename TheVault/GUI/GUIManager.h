#pragma once
#include "Utility/Function.h"

class GUIManager
{
private:
	Function<void()> renderCallback;

	bool InitImguiBackends();
	bool SetupWindow();

public:
	GUIManager();
	~GUIManager();

	bool Initialize(const Function<void()>& callback);
	void Shutdown();
	void Render();
};
