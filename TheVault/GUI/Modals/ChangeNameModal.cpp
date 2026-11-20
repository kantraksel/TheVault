#include "ChangeNameModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Vault/VaultKeeper.h"

ChangeNameModal::ChangeNameModal() : BaseDualIndexModal("Change Name")
{
	nameInput.resize(256);
}

void ChangeNameModal::OnOpen()
{
	std::string name;
	if (!isLockIndex)
	{
		auto& store = ContentStore::Get();
		std::lock_guard lock(store.GetAccessLock());
		name = store.GetName(selectedIndex);
	}
	else
		name = VaultKeeper::Get().GetDirectApi().GetHint(selectedIndex);

	memcpy_s(nameInput.data(), nameInput.size(), name.data(), name.size());
	if (nameInput.size() > name.size())
		nameInput[name.size()] = 0;
}

void ChangeNameModal::OnClose()
{
	memset(nameInput.data(), 0, nameInput.size());
}

void ChangeNameModal::OnRender()
{
	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();

	constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;
	bool submit = ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, flags);
	if (ImGui::Button("Change") || submit)
	{
		auto name = std::string_view(nameInput.data());
		if (!name.empty())
		{
			if (!isLockIndex)
				ContentStore::Get().SetName(selectedIndex, name);
			else
				VaultKeeper::Get().ChangeHint(selectedIndex, name);

			Close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
