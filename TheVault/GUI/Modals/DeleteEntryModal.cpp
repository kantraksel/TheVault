#include "DeleteEntryModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Vault/VaultKeeper.h"

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
		auto& store = ContentStore::Get();
		Text(store.GetName(selectedIndex));
	}
	else
	{
		auto api = VaultKeeper::Get().GetDirectApi();
		Text(api.GetHint(selectedIndex));
	}

	if (ImGui::Button("Yes"))
	{
		if (!isLockIndex)
			ContentStore::Get().Remove(selectedIndex);
		else
			VaultKeeper::Get().RemoveHint(selectedIndex);
		Close();
	}
	ImGui::SameLine();
	if (ImGui::Button("No"))
		Close();
}
