#include "AddPasswordModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"

AddPasswordModal::AddPasswordModal() : BaseModal("Add Password")
{
	nameInput.resize(256);
	pwdInput.resize(256);
}

void AddPasswordModal::OnClose()
{
	memset(nameInput.data(), 0, nameInput.size());
	memset(pwdInput.data(), 0, pwdInput.size());
}

void AddPasswordModal::OnRender()
{
	TextWidget("Name:");
	ImGui::SameLine();

	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, ImGuiInputTextFlags_NoUndoRedo);

	TextWidget("Value:");
	ImGui::SameLine();

	constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
	bool submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.size() - 1, flags);
	if (ImGui::Button("Add") || submit)
	{
		auto name = std::string_view(nameInput.data());
		auto pwd = std::string_view(pwdInput.data());
		if (!name.empty() && !pwd.empty())
		{
			ContentStore::Get().AddText(name, pwd);
			Close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
