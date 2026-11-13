#include <chrono>
#include "Utils/ImGuiUtils.h"
#include "MainWindow.h"
#include "Engine/Logger.h"
#include "App/Game.h"
#include "Engine/GhostFries.h"

extern Game game;

MainWindow::MainWindow()
{
	content = RenderContent::MainView;
	applet = nullptr;

	SwitchToMainView();
	main.DisableInteractions();
	main.GetWelcomeApplet().Open();
}

MainWindow::~MainWindow()
{
}

void MainWindow::Initialize()
{
}

void IApplet::OnEnter()
{
}

void IApplet::OnLeave()
{
}

void MainWindow::SwitchApplet(RenderContent content, IApplet* applet)
{
	if (this->applet)
		this->applet->OnLeave();
	this->content = content;
	this->applet = applet;
	applet->OnEnter();
}

void MainWindow::SwitchToMainView()
{
	SwitchApplet(RenderContent::MainView, &main);
	main.EnableInteractions();
}

void MainWindow::ShowError(const std::string_view& text, bool critical)
{
	std::string message(text);
	scheduler.Schedule([=]()
	{
		Modals::GetErrorModal().Open(message, critical);
		return true;
	});
}

void MainWindow::Render()
{
	auto* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("TheVault##MainWindow", nullptr, flags))
	{
		applet->Render();
		modals.Render();
		RenderAppletBar();
	}
	
	ImGui::End();

	scheduler.Run();
}

void MainWindow::CreateVault(const std::wstring& file)
{
	ProcessVaultTask(game.GetKeeper().CreateVault(file));
}

void MainWindow::OpenVault(const std::wstring& file)
{
	ProcessVaultTask(game.GetKeeper().OpenVault(file));
}

void MainWindow::SaveVault(bool closeVault, bool exitOnConfirm)
{
	ProcessVaultTask(game.GetKeeper().SaveVault(closeVault, false, exitOnConfirm), "Saving vault...");
}

void MainWindow::CloseVault(bool exitOnConfirm)
{
	ProcessVaultTask(game.GetKeeper().CloseVault(exitOnConfirm), "Closing vault...");
}

void MainWindow::ProcessVaultTask(std::future<uint64_t>&& task, const std::string_view& title)
{
	main.GetProcessApplet().Open(std::move(task), title);
	main.DisableInteractions();
}

uint64_t MainWindow::ProcessVaultResponse(std::future<uint64_t>& task)
{
	using namespace std::chrono_literals;
	if (!task.valid() || task.wait_for(0s) != std::future_status::ready)
		return _TR_NoAction;

	auto value = task.get();
	switch (value)
	{
		case TR_SwitchToWelcome:
		{
			SwitchToMainView();
			main.DisableInteractions();
			main.GetWelcomeApplet().Open();
			break;
		}
		case TR_SwitchToLogin:
		{
			SwitchToMainView();
			main.DisableInteractions();
			main.GetLoginApplet().Open();
			break;
		}
		case TR_SwitchToMainView:
		{
			SwitchToMainView();
			break;
		}
		case TR_SwitchToLockSetup:
		{
			SwitchToMainView();
			main.GetLockApplet().Open();
			break;
		}
		case TR_FetchNextHint:
		{
			main.GetLoginApplet().RefreshHintName();
			break;
		}
		case TR_CloseVault:
		{
			ProcessVaultTask(game.GetKeeper().CloseVault(), "Closing vault...");
			break;
		}
		case TR_CloseApp:
		{
			GhostFries::Quit();
			break;
		}
	}
	return value;
}

void MainWindow::RenderAppletBar()
{
#if _DEBUG
	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("MainView", nullptr, content == RenderContent::MainView))
			SwitchToMainView();

		if (ImGui::MenuItem("DemoWindow", nullptr, showDemoWindow))
			showDemoWindow = !showDemoWindow;

		ImGui::EndMenuBar();
	}
#endif
}
