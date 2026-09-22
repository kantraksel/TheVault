#include "Systems/GraphicsEngine.h"
#include "Systems/YamlDoc.h"
#include "Engine/Logger.h"
#include "Utility/StringUtils.h"

#ifdef _DEBUG
#define D3D_CREATION_DEBUG_FLAG D3D11_CREATE_DEVICE_DEBUG
#else
#define D3D_CREATION_DEBUG_FLAG 0
#endif

#pragma comment(lib, "d3d11")

GraphicsEngine::GraphicsEngine() : mpDevice(nullptr), mpContext(nullptr), mpSwapChain(nullptr), mpTargetView(nullptr),
									mState { 640, 480, false, false }
{
}

GraphicsEngine::~GraphicsEngine()
{
}

bool GraphicsEngine::Initialize(HWND hWnd)
{
	auto featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC desc
	{
		.BufferDesc = {
			.Width = mState.mWidth,
			.Height = mState.mHeight,
			.RefreshRate = {
				.Numerator = 60,
				.Denominator = 1,
			},
			.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
		},
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = hWnd,
		.Windowed = mState.mFullScreen ? FALSE : TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	};
	

	auto hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D_CREATION_DEBUG_FLAG, &featureLevel, 1, D3D11_SDK_VERSION, &desc, &mpSwapChain, &mpDevice, NULL, &mpContext);
	if (FAILED(hResult))
	{
		Logger::LogError("Graphics: Failed to initialize D3D11: {}", StringUtils::Format(hResult));
		return false;
	}

	DXGI_SWAP_CHAIN_DESC desc2;
	mpSwapChain->GetDesc(&desc2);

	hResult = mpSwapChain->ResizeTarget(&desc2.BufferDesc);
	if (FAILED(hResult) && hResult != DXGI_STATUS_MODE_CHANGE_IN_PROGRESS)
	{
		Logger::LogError("Graphics: Failed to resize target window: {}", StringUtils::Format(hResult));
		return false;
	}

	if (!InitializeTargetView())
		return false;
	
	Logger::Log("Initialized graphics engine");
	return true;
}

bool GraphicsEngine::InitializeTargetView()
{
	ID3D11Texture2D* pBackbuffer;
	auto hResult = mpSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackbuffer));
	if (FAILED(hResult))
	{
		Logger::LogError("Graphics: Failed to get backbuffer: {}", StringUtils::Format(hResult));
		return false;
	}

	hResult = mpDevice->CreateRenderTargetView(pBackbuffer, nullptr, &mpTargetView);
	pBackbuffer->Release();
	if (FAILED(hResult))
	{
		Logger::LogError("Graphics: Failed to create render target view: {}", StringUtils::Format(hResult));
		return false;
	}

	return true;
}

void GraphicsEngine::ShutdownTargetView()
{
	if (mpTargetView)
	{
		mpTargetView->Release();
		mpTargetView = nullptr;
	}
}

void GraphicsEngine::Shutdown()
{
	if (mpTargetView)
	{
		mpTargetView->Release();
		mpTargetView = nullptr;
	}

	if (mpSwapChain)
	{
		mpSwapChain->Release();
		mpSwapChain = nullptr;
	}

	if (mpContext)
	{
		mpContext->Release();
		mpContext = nullptr;
	}

	if (mpDevice)
	{
		mpDevice->Release();
		mpDevice = nullptr;
	}

	Logger::Log("Graphics engine shut down");
}

void GraphicsEngine::Render()
{
	static float background[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mpContext->ClearRenderTargetView(mpTargetView, background);
	mpContext->OMSetRenderTargets(1, &mpTargetView, nullptr);

	if (mRenderEvent)
		mRenderEvent();

	auto sync = mState.mVSync ? 1 : 0;
	mpSwapChain->Present(sync, 0);
}

void GraphicsEngine::LoadConfig(YamlDoc& config)
{
	if (mpContext)
	{
		Logger::LogError("Graphics: Cannot load config - context already exists");
		return;
	}

	auto display = config["display"];
	display.SetMap();

	auto width = display["width"].GetUInt();
	auto height = display["height"].GetUInt();
	bool fullscreen = display["fullscreen"].GetBool();
	bool vsync = display["vertical_sync"].GetBool(true);

	//width and height must be valid display mode
	if (width < 640 || height < 480)
	{
		width = 640;
		height = 480;
	}

	mState.mWidth = width;
	mState.mHeight = height;
	mState.mFullScreen = fullscreen;
	mState.mVSync = vsync;
}

static bool ResizeWindow(GraphicsEngine::DisplayState& displayState, IDXGISwapChain* pSwapChain)
{
	DXGI_MODE_DESC desc
	{
		.Width = displayState.mWidth,
		.Height = displayState.mHeight,
		.RefreshRate = { 60, 1 },
		.Format = DXGI_FORMAT_UNKNOWN,
	};
	auto hResult = pSwapChain->ResizeTarget(&desc);
	if (FAILED(hResult) && hResult != DXGI_STATUS_MODE_CHANGE_IN_PROGRESS)
	{
		Logger::LogError("Graphics: Failed to resize target window: {}", StringUtils::Format(hResult));
		return false;
	}

	return true;
}

bool GraphicsEngine::SetDisplayDimensions(unsigned int width, unsigned int height)
{
	Logger::Log("Graphics: Changing display dims to {}x{}", width, height);

	ShutdownTargetView();

	auto hResult = mpSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hResult))
	{
		Logger::LogError("Graphics: Failed to resize swap chain buffers: {}", StringUtils::Format(hResult));
		Logger::Log("Graphics: Recreating context for display dims {}x{}", mState.mWidth, mState.mHeight);
	}
	else
	{
		mState.mWidth = width;
		mState.mHeight = height;
	}

	if (!InitializeTargetView())
	{
		Logger::LogError("Graphics: Failed to recreate render context for new display state");
		return false;
	}

	if (!mState.mFullScreen && !ResizeWindow(mState, mpSwapChain))
		return false;

	return true;
}

void GraphicsEngine::SetVerticalSync(bool vsync)
{
	mState.mVSync = vsync;
}
