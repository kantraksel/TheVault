#include "WelcomeApplet.h"
#include "GUI/VaultCommander.hpp"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"

WelcomeApplet::WelcomeApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow)
{
}

WelcomeApplet::~WelcomeApplet()
{
}

void WelcomeApplet::Close()
{
	BlockingApplet::Close();
	name.clear();
}

void WelcomeApplet::OnRender()
{
	BeginVisibleChildWindow("##Welcome", ImVec2(285, 0));

	Text("Welcome to TheVault");
	ImGui::Separator();

	if (ImGui::Button("Open existing vault") && WinApi::OpenFileDialog(L"Open existing vault", L"vault.bin", name))
	{
		VaultCommander::Get().OpenVault(name);
		Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Create new vault") && WinApi::SaveFileDialog(L"Create new vault", L"vault.bin", name))
	{
		VaultCommander::Get().CreateVault(name);
		Close();
	}

	ImGui::EndChild();
}
