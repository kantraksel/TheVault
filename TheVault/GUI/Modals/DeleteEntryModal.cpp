#include "DeleteEntryModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

DeleteEntryModal::DeleteEntryModal() : BaseDualIndexModal("Delete Password")
{
}

void DeleteEntryModal::OnRender()
{
	Text("Are you sure you want to delete the password?");
	Text("Name:");
	ImGui::SameLine();

	if (!isLockIndex)
	{
		auto& store = game.GetContentStore();
		Text(store.GetName(selectedIndex));
	}
	else
	{
		auto api = game.GetKeeper().GetDirectApi();
		Text(api.GetHint(selectedIndex));
	}

	if (ImGui::Button("Yes"))
	{
		if (!isLockIndex)
			game.GetContentStore().Remove(selectedIndex);
		else
			game.GetKeeper().RemoveHint(selectedIndex);
		Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
		Close();
}
