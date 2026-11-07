#include "ShowPasswordModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "Utils/WinApi.h"
#include "../Modals.hpp"

extern Game game;

ShowPasswordModal::ShowPasswordModal() : BaseIndexModal("Password Details")
{
}

void ShowPasswordModal::OnRender()
{
	auto& store = game.GetContentStore();
	Text("Name:");
	ImGui::SameLine();
	Text(store.GetName(selectedIndex));
	Text("Value:");
	ImGui::SameLine();
	Text(store.GetText(selectedIndex));

	if (ImGui::Button("Copy"))
	{
		WinApi::SetClipboardText(store.GetText(selectedIndex));
	}
	ImGui::SameLine();
	if (ImGui::Button("Change"))
	{
		ImGui::CloseCurrentPopup();
		Modals::GetChangePasswordModal().Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Close"))
		ImGui::CloseCurrentPopup();
}
