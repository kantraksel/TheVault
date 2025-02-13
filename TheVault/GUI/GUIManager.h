#pragma once
#include <vector>
#include "IRender.h"
#include "Engine/Components/ICanvasComponent.h"

class GUIManager : public ICanvasComponent
{
private:
	std::vector<IRender*> objects;

	bool InitImguiBackends();
	bool SetupWindow();

public:
	GUIManager(class Actor* pParent);
	~GUIManager() override;

	bool Initialize();
	void Shutdown();
	
	void RegisterObject(IRender* obj);
	void Render(class D2DEngine& engine) override;
};
