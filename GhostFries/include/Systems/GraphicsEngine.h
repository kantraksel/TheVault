#pragma once
#include "D3DS.h"
#include "Utility/Function.h"

class GraphicsEngine
{
private:
	ID3D11Device* mpDevice;
	ID3D11DeviceContext* mpContext;
	IDXGISwapChain* mpSwapChain;
	ID3D11RenderTargetView* mpTargetView;
	Function<void()> mRenderEvent;

public:
	struct DisplayState
	{
		unsigned int mWidth;
		unsigned int mHeight;
		bool mFullScreen;
		bool mVSync;
	};

	bool InitializeTargetView();
	void ShutdownTargetView();

private:
	DisplayState mState;

public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Initialize(HWND hWnd);
	void Shutdown();
	void LoadConfig(struct YamlDoc& config);

	void Render();
	auto* GetContext() { return mpContext; }
	auto& GetRenderEvent() { return mRenderEvent; }

	bool SetDisplayDimensions(unsigned int width, unsigned int height);
	void SetVerticalSync(bool vsync);
};
