#include "ChangePasswordModal.hpp"
#include "ChangeNameModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"

ChangePasswordModal::ChangePasswordModal(ChangeNameModal& changeNameModal) : BaseIndexModal("Change Password"), isPasswordShown{false}, changeNameModal(changeNameModal)
{
	pwdInput.resize(256);
}

void ChangePasswordModal::OnClose()
{
	memset(pwdInput.data(), 0, pwdInput.size());
}

void ChangePasswordModal::OnRender()
{
	auto& store = ContentStore::Get();
	std::lock_guard lock(store.GetAccessLock());
	Text("Name:");
	ImGui::SameLine();
	Text(store.GetName(selectedIndex));

	SetNextRightButtonAlign("*", true);
	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();

	auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;
	if (!isPasswordShown)
		flags = flags | ImGuiInputTextFlags_Password;
	bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.size() - 1, flags);
	SameInnerLine();
	ImGui::Button("*");
	isPasswordShown = ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left);
	if (ImGui::Button("Change") || submit)
	{
		auto pwd = std::string_view(pwdInput.data());
		if (!pwd.empty())
		{
			store.SetText(selectedIndex, pwd);
			Close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Change Name"))
	{
		Close();
		changeNameModal.Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
