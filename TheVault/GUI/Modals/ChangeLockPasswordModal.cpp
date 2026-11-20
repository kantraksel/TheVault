#include "ChangeLockPasswordModal.hpp"
#include "ChangeNameModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "GUI/VaultCommander.hpp"
#include "Vault/VaultKeeper.h"

ChangeLockPasswordModal::ChangeLockPasswordModal(ChangeNameModal& changeNameModal) : BaseIndexModal("Change Password##ChangeLockPassword"), isProcessing{false}, isPasswordShown{false}, changeNameModal(changeNameModal)
{
	pwdInput.resize(256);
}

void ChangeLockPasswordModal::OnClose()
{
	memset(pwdInput.data(), 0, pwdInput.size());
	isProcessing = false;
}

void ChangeLockPasswordModal::OnRender()
{
	Text("Name:");
	ImGui::SameLine();

	auto& keeper = VaultKeeper::Get();
	{
		auto api = keeper.GetDirectApi();
		Text(api.GetHint(selectedIndex));
	}

	if (!isProcessing)
	{
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
				isProcessing = true;
				VaultCommander::Get().SetPassword(selectedIndex, pwd, [this](uint64_t)
						{
							Close();
						});
				memset(pwdInput.data(), 0, pwdInput.size());
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Change Name"))
		{
			Close();
			changeNameModal.Open(selectedIndex, true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			Close();
	}
	else
	{
		ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), "Hashing...");
	}
}
