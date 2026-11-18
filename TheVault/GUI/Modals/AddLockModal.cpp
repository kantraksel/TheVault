#include "AddLockModal.hpp"
#include "GUI/VaultCommander.hpp"
#include "Utils/ImGuiUtils.h"
#include "Vault/VaultKeeper.h"

AddLockModal::AddLockModal() : BaseModal("New Vault Lock"), isProcessing{false}
{
	nameInput.resize(256);
	pwdInput.resize(256);
}

void AddLockModal::OnClose()
{
	memset(nameInput.data(), 0, nameInput.size());
	memset(pwdInput.data(), 0, pwdInput.size());
	isProcessing = false;
}

void AddLockModal::OnRender()
{
	TextWidget("Name:");
	ImGui::SameLine();

	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	bool submit = ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo);

	if (!isProcessing)
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
				int index = VaultKeeper::Get().AddHint(name);
				memset(nameInput.data(), 0, nameInput.size());

				if (!pwd.empty())
				{
					isProcessing = true;
					VaultCommander::Get().SetPassword(index, pwd, [this](uint64_t)
						{
							Close();
						});
					memset(pwdInput.data(), 0, pwdInput.size());
				}
				else
					Close();
			}
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
