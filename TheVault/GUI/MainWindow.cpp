#include "MainWindow.h"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

MainWindow::MainWindow()
{
	main.DisableInteractions();
	main.GetWelcomeApplet().Open();
}

MainWindow::~MainWindow()
{
}

void MainWindow::Render()
{
	auto* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	if (ImGui::Begin("TheVault##MainWindow", nullptr, flags))
	{
		main.Render();
		modals.Render();
		RenderAppletBar();
	}
	
	ImGui::End();

	game.GetVaultCommander().Update();
}

void MainWindow::RenderAppletBar()
{
#if _DEBUG
	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("DemoWindow", nullptr, showDemoWindow))
				showDemoWindow = !showDemoWindow;

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
#endif
}
