#include "WelcomeApplet.h"
#include "GUI/VaultCommander.hpp"
#include "Utils/ImGuiUtils.h"

WelcomeApplet::WelcomeApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow)
{
}

WelcomeApplet::~WelcomeApplet()
{
}

void WelcomeApplet::OnRender()
{
	BeginVisibleChildWindow("##Welcome", ImVec2(285, 0));

	Text("Welcome to TheVault");
	ImGui::Separator();

	if (ImGui::Button("Open existing vault") && VaultCommander::Get().OpenVault())
		Close();
	ImGui::SameLine();
	if (ImGui::Button("Create new vault") && VaultCommander::Get().CreateVault())
		Close();

	ImGui::EndChild();
}
