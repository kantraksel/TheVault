#include "ChangePasswordModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "../Modals.hpp"

extern Game game;

ChangePasswordModal::ChangePasswordModal() : BaseIndexModal("Change Password")
{
	pwdInput.resize(256);
}

void ChangePasswordModal::OnClose()
{
	memset(pwdInput.data(), 0, pwdInput.size());
}

void ChangePasswordModal::OnRender()
{
	auto& store = game.GetContentStore();
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
		Modals::GetChangeNameModal().Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
