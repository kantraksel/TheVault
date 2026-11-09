#include <chrono>
#include "Utils/ImGuiUtils.h"
#include "MainWindow.h"
#include "Engine/Logger.h"
#include "App/Game.h"
#include "Engine/GhostFries.h"

extern Game game;

MainWindow::MainWindow()
{
	content = RenderContent::Welcome;
	applet = nullptr;
	SwitchToWelcome();
}

MainWindow::~MainWindow()
{
}

void MainWindow::Initialize()
{
	login.Initialize();
	lock.Initialize();
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

void MainWindow::SwitchToWelcome()
{
	SwitchApplet(RenderContent::Welcome, &welcome);
}

void MainWindow::SwitchToGlobalProcess()
{
	SwitchApplet(RenderContent::GlobalProcess, &process);
}

void MainWindow::SwitchToCriticalError()
{
	SwitchApplet(RenderContent::CriticalError, &error);
}

void MainWindow::SwitchToLoginChallenge()
{
	SwitchApplet(RenderContent::LoginChallenge, &login);
}

void MainWindow::SwitchToMainView()
{
	SwitchApplet(RenderContent::MainView, &main);
}

void MainWindow::SwitchToLockSetup()
{
	SwitchApplet(RenderContent::LockSetup, &lock);
}

void MainWindow::ShowError(const std::string_view& text, bool critical)
{
	std::string message(text);
	scheduler.Schedule([=, this]()
	{
		if (critical)
		{
			error.SetError(message);
			SwitchToCriticalError();
		}
		else
			Modals::GetErrorModal().Open(message);
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
	process.ProcessVaultTask(std::move(task));
	process.SetTitle(title);
	SwitchToGlobalProcess();
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
			SwitchToWelcome();
			break;
		}
		case TR_SwitchToLogin:
		{
			SwitchToLoginChallenge();
			login.RefreshHintName();
			break;
		}
		case TR_SwitchToMainView:
		{
			SwitchToMainView();
			break;
		}
		case TR_SwitchToLockSetup:
		{
			SwitchToLockSetup();
			break;
		}
		case TR_FetchNextHint:
		{
			login.RefreshHintName();
			break;
		}
		case TR_CriticalError:
		{
			SwitchToCriticalError();
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
		if (ImGui::MenuItem("Welcome", nullptr, content == RenderContent::Welcome))
			SwitchToWelcome();

		if (ImGui::MenuItem("GlobalProcess", nullptr, content == RenderContent::GlobalProcess))
			SwitchToGlobalProcess();

		if (ImGui::MenuItem("LoginChallenge", nullptr, content == RenderContent::LoginChallenge))
			SwitchToLoginChallenge();

		if (ImGui::MenuItem("MainView", nullptr, content == RenderContent::MainView))
			SwitchToMainView();

		if (ImGui::MenuItem("LockSetup", nullptr, content == RenderContent::LockSetup))
			SwitchToLockSetup();

		if (ImGui::MenuItem("DemoWindow", nullptr, showDemoWindow))
			showDemoWindow = !showDemoWindow;

		ImGui::EndMenuBar();
	}
#endif
}
