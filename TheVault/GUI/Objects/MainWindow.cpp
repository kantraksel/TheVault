#include "ImGuiUtils.h"
#include "MainWindow.h"
#include "Engine/Logger.h"
#include "../../Game.h"

extern Game game;

MainWindow::MainWindow()
{
	openConfirmExitModal = false;

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

void IApplet::OnLeave()
{
}

void MainWindow::SwitchApplet(RenderContent content, IApplet* applet)
{
	if (this->applet)
		this->applet->OnLeave();
	this->content = content;
	this->applet = applet;
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
	if (critical)
	{
		error.SetError(text, false);
		SwitchToCriticalError();
	}
	else
		error.SetError(text, true);
}

class DemoWindowApplet : public IApplet
{
public:
	void Render() override
	{
		ImGui::ShowDemoWindow();
	}
};
static DemoWindowApplet demoApplet;

void MainWindow::Render()
{
	auto* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

#if _DEBUG
	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
#else
	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
#endif
	if (ImGui::Begin("TheVault##MainWindow", nullptr, flags))
	{
#if _DEBUG
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

			if (ImGui::MenuItem("DemoWindow", nullptr, content == RenderContent::DemoWindow))
				SwitchApplet(RenderContent::DemoWindow, &demoApplet);

			ImGui::EndMenuBar();
		}
#endif

		applet->Render();
		error.RenderModal();
		RenderConfirmExitModal();
	}
	
	ImGui::End();
}

void MainWindow::SaveVault()
{
	ProcessVaultTask(game.GetKeeper().SaveVault(), "Saving vault...");
}

void MainWindow::CloseVault()
{
	ProcessVaultTask(game.GetKeeper().CloseVault(), "Closing vault...");
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
	}
	return value;
}

void MainWindow::OpenConfirmExitModal()
{
	openConfirmExitModal = true;
}

void MainWindow::RenderConfirmExitModal()
{
	if (openConfirmExitModal)
	{
		openConfirmExitModal = false;
		ImGui::OpenPopup("Confirm exit");
	}

	if (ImGui::BeginPopupModal("Confirm exit", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Do you want to save changes?");

		if (ImGui::Button("Yes"))
		{
			ImGui::CloseCurrentPopup();
			ProcessVaultTask(game.GetKeeper().SaveCloseVault(), "Saving vault...");
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
			ProcessVaultTask(game.GetKeeper().CloseVault(), "Closing vault...");
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}
