#include "DeleteEntryModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

DeleteEntryModal::DeleteEntryModal() : BaseIndexModal("Delete Password")
{
}

void DeleteEntryModal::OnRender()
{
	Text("Are you sure you want to delete the password?");
	Text("Name:");
	ImGui::SameLine();

	auto& store = game.GetContentStore();
	Text(store.GetName(selectedIndex));
	if (ImGui::Button("Yes"))
	{
		store.Remove(selectedIndex);
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
		ImGui::CloseCurrentPopup();
}
