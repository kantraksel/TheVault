#pragma once
#include <vector>
#include "IRender.h"

class GUIManager
{
private:
	std::vector<IRender*> objects;

	bool InitImguiBackends();
	bool SetupWindow();

public:
	GUIManager();
	~GUIManager();

	bool Initialize();
	void Shutdown();
	
	void RegisterObject(IRender* obj);
	void Render();
};
