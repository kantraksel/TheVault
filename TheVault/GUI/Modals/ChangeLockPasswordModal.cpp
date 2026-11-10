#include "ChangeLockPasswordModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "../Modals.hpp"

extern Game game;

ChangeLockPasswordModal::ChangeLockPasswordModal() : BaseIndexModal("Change Password##ChangeLockPassword")
{
	pwdInput.resize(256);
}

void ChangeLockPasswordModal::OnRender()
{
	Text("Name:");
	ImGui::SameLine();

	auto& keeper = game.GetKeeper();
	{
		auto api = keeper.GetDirectApi();
		Text(api.GetHint(selectedIndex));
	}

	if (!vaultTask.valid())
	{
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
				vaultTask = keeper.SetHintKey(selectedIndex, pwd);
				memset(pwdInput.data(), 0, pwdInput.size());
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Change Name"))
		{
			memset(pwdInput.data(), 0, pwdInput.size());
			ImGui::CloseCurrentPopup();
			Modals::GetChangeNameModal().Open(selectedIndex, true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
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
