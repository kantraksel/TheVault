#include "WelcomeApplet.h"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "Utils/WinApi.h"

extern Game game;

WelcomeApplet::WelcomeApplet() : isOpen{false}
{
}

WelcomeApplet::~WelcomeApplet()
{
}

void WelcomeApplet::Open()
{
	isOpen = true;
}

void WelcomeApplet::Close()
{
	isOpen = false;
	name.clear();
}

void WelcomeApplet::Render()
{
	if (!isOpen)
		return;
	BeginVisibleChildWindow("##Welcome", ImVec2(285, 0));

	Text("Welcome to TheVault");
	ImGui::Separator();

	if (ImGui::Button("Open existing vault") && WinApi::OpenFileDialog(L"Open existing vault", L"vault.bin", name))
	{
		game.GetMainWindow().OpenVault(name);
		Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Create new vault") && WinApi::SaveFileDialog(L"Create new vault", L"vault.bin", name))
	{
		game.GetMainWindow().CreateVault(name);
		Close();
	}

	ImGui::EndChild();
}
