#include "WelcomeApplet.h"
#include "GUI/VaultCommander.hpp"
#include "Utils/ImGuiUtils.h"

WelcomeApplet::WelcomeApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow), openNextFrame{false}
{
}

WelcomeApplet::~WelcomeApplet()
{
}

void WelcomeApplet::Open()
{
	BlockingApplet::Open();
	openNextFrame = true;
}

void WelcomeApplet::Close()
{
	BlockingApplet::Close();
	openNextFrame = false;
}

void WelcomeApplet::OnRender()
{
	if (openNextFrame)
	{
		openNextFrame = false;
		ImGui::OpenPopup("##Welcome");
	}

	CenterNextWindow();
	if (!ImGui::BeginPopup("##Welcome"))
	{
		if (isOpen)
			Close();
		return;
	}

	Text("Welcome to TheVault");
	ImGui::SameLine();
	ImGui::BeginDisabled();
	Text("(?)");
	ImGui::EndDisabled();
	if (ImGui::BeginItemTooltip())
	{
		Text("TheVault is a password manager with multi-layer encryption.\n"
		"Each layer is universal - it may encrypt final document or another layer.\n"
		"Key generation is slow on purpose, intended to fight brute-force attacks.");
		ImGui::EndTooltip();
	}
	ImGui::Separator();

	if (ImGui::Button("Open existing vault") && VaultCommander::Get().OpenVault())
		Close();
	ImGui::SameLine();
	if (ImGui::Button("Create new vault") && VaultCommander::Get().CreateVault())
		Close();

	ImGui::EndPopup();
}
