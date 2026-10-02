#include <thread>
#include "Engine/GhostFries.h"
#include "Engine/version.h"
#include "Systems/WindowManager.h"
#include "Systems/Configurations.h"
#include "Systems/GraphicsEngine.h"
#include "Systems/AppData.h"
#include "Engine/Logger.h"
#include "Systems/YamlDoc.h"
#include "Utility/StringUtils.h"

struct GhostFriesImpl
{
	IGame* mpGame;
	bool mEngineInitialized;
	bool mShutdown;

	GhostFriesImpl(IGame* pGame);

	bool PreInit(const wchar_t* config);
	bool Initialize();
	void UpdateLoop();
	bool Shutdown();
	void LoadConfigs();

	WindowManager mWindowManager;
	GraphicsEngine mRenderEngine;
	Configurations mConfigurations;
	YamlDoc mConfig;
};

static GhostFriesImpl* pInstance = nullptr;

bool GhostFries::Main(IGame* pGame, const wchar_t* config)
{
	if (pInstance)
		return false;
	pInstance = new GhostFriesImpl(pGame);

	if (!pInstance->PreInit(config))
	{
		WindowManager::ShowError(L"Invalid engine configuration. Contact application developers so they can fix it :)");
		return false;
	}

	if (!pInstance->Initialize())
		WindowManager::ShowError(L"There was an error while launching the game. Check logs for more details!");
	else if (!pInstance->mShutdown)
	{
		Logger::Log("Game initialized");
		pInstance->UpdateLoop();
		Logger::Log("Game shutting down");
	}

	pInstance->Shutdown();
	delete pInstance;
	return true;
}

void GhostFries::Quit()
{
	pInstance->mShutdown = true;
}

bool GhostFries::IsRunning()
{
	return pInstance != nullptr;
}

GhostFriesImpl::GhostFriesImpl(IGame* pGame) : mpGame(pGame), mShutdown(false)
{
	mEngineInitialized = false;
}

bool GhostFriesImpl::PreInit(const wchar_t* config)
{
	if (config)
	{
		auto configStr = StringUtils::WideStringToUtf8(config);
		auto ref = FixedArrayChar::Copy(configStr.data(), static_cast<uint32_t>(configStr.size()));

		if (!mConfig.Load(ref, L"engine.yaml") && !mConfig.Load(L"engine.yaml"))
			return false;
	}
	else if (!mConfig.Load(L"engine.yaml"))
		return false;

	auto root = mConfig.GetRootNode();
	auto name = root["name"].GetString();
	auto folder = root["data_folder"].GetString();
	auto portableData = root["portable_data"].GetBool(true);

	AppData::SetAppDataName(folder, portableData);
	Logger::OpenLogFile(AppData::GetAppDataFolder() / "log.txt");
	
	if (name.empty())
		name = "GhostFries Engine";
	Logger::Log("GhostFries {}; Game {}", EngineVersion::Version(), name);
	Logger::SetTitle(name);

	auto nameW = StringUtils::Utf8ToWideString(name);
	mWindowManager.SetWindowTitle(nameW.c_str());

	return true;
}

bool GhostFriesImpl::Initialize()
{
	LoadConfigs();

	if (!mWindowManager.Initialize())
		return false;
	if (!mRenderEngine.Initialize(mWindowManager.GetWindow()))
		return false;

	mWindowManager.SetDestroyCallback(GhostFries::Quit);
	
	mEngineInitialized = true;
	if (!mpGame->OnInitialize())
	{
		Logger::LogWarn("Game failed to intialize, shutting down");
		return false;
	}
	mWindowManager.SetVisibility(true);
	return true;
}

void GhostFriesImpl::UpdateLoop()
{
	while (!mShutdown)
	{
		mWindowManager.Update();

		if (!mWindowManager.IsMinimized())
		{
			mRenderEngine.Render();
		}
		else
		{
			// sleep() sleeps for multiplies of 20ms
			// the sleep forces 25fps
			static std::chrono::milliseconds time(40);
			std::this_thread::sleep_for(time);
		}
	}
}

bool GhostFriesImpl::Shutdown()
{
	if (mEngineInitialized)
	{
		mpGame->OnShutdown();
		mEngineInitialized = false;
	}

	mRenderEngine.Shutdown();
	mWindowManager.Shutdown();
	return true;
}

WindowManager& GhostFries::GetWindowManager()
{
	return pInstance->mWindowManager;
}

GraphicsEngine& GhostFries::GetRenderEngine()
{
	return pInstance->mRenderEngine;
}

void GhostFriesImpl::LoadConfigs()
{
	mConfigurations.LoadAll();
	auto* config = mConfigurations.Get(Configurations::Type::Graphics);
	if (config)
		mRenderEngine.LoadConfig(*config);
}
