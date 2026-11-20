#include "ShowPasswordModal.hpp"
#include "ChangePasswordModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"

ShowPasswordModal::ShowPasswordModal(ChangePasswordModal& changePasswordModal) : BaseIndexModal("Password Details"), changePasswordModal(changePasswordModal)
{
}

void ShowPasswordModal::OnRender()
{
	auto& store = ContentStore::Get();
	std::lock_guard lock(store.GetAccessLock());
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
		Close();
		changePasswordModal.Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Close"))
		Close();
}
