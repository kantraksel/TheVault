#include "AddPasswordModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

AddPasswordModal::AddPasswordModal() : BaseModal("Add Password")
{
	nameInput.resize(256);
	pwdInput.resize(256);
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
			game.GetContentStore().AddText(name, pwd);

			memset(nameInput.data(), 0, nameInput.size());
			memset(pwdInput.data(), 0, pwdInput.size());
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		memset(nameInput.data(), 0, nameInput.size());
		memset(pwdInput.data(), 0, pwdInput.size());
		ImGui::CloseCurrentPopup();
	}
}
