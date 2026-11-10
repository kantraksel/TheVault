#include "AddLockModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

AddLockModal::AddLockModal() : BaseModal("New Vault Lock")
{
	nameInput.resize(256);
	pwdInput.resize(256);
}

void AddLockModal::OnRender()
{
	TextWidget("Name:");
	ImGui::SameLine();

	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	bool submit = ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo);

	if (!vaultTask.valid())
	{
		TextWidget("Value:");
		ImGui::SameLine();

		constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
		submit = ImGui::InputText("##Password", pwdInput.data(), pwdInput.size() - 1, flags) || submit;
		if (ImGui::Button("Add") || submit)
		{
			auto name = std::string_view(nameInput.data());
			auto pwd = std::string_view(pwdInput.data());
			if (!name.empty())
			{
				auto& keeper = game.GetKeeper();
				int index = keeper.AddHint(name);
				memset(nameInput.data(), 0, nameInput.size());

				if (!pwd.empty())
				{
					vaultTask = keeper.SetHintKey(index, pwd);
					memset(pwdInput.data(), 0, pwdInput.size());
				}
				else
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
	else
	{
		ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), "Hashing...");
		if (game.GetMainWindow().ProcessVaultResponse(vaultTask) != MainWindow::_TR_NoAction)
			ImGui::CloseCurrentPopup();
	}
}
