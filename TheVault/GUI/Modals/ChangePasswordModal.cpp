#include "ChangePasswordModal.hpp"
#include "ChangeNameModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"

ChangePasswordModal::ChangePasswordModal(ChangeNameModal& changeNameModal) : BaseIndexModal("Change Password"), changeNameModal(changeNameModal)
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
	Text("Name:");
	ImGui::SameLine();
	Text(store.GetName(selectedIndex));

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();

	constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
	bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.size() - 1, flags);
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
