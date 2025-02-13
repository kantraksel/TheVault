#include <string>
#include "ImGuiUtils.h"
#include "LockApplet.h"
#include "../../Game.h"
#include "../../WinApi.h"
#include "../../Crypto.h"
#include "../../StringUtils.h"

extern Game game;

LockApplet::LockApplet()
{
	openSetHintValueModal = false;
	openChangeHintModal = false;
	openDeleteModal = false;
	openResetSalts = false;
	modalIdx = 0;

	nameInput.reserve(256);
}

LockApplet::~LockApplet()
{
}

void LockApplet::Initialize()
{
	passwordInput = Crypto::AllocMemory(256);
	if (!passwordInput)
	{
		game.GetKeeper().RaiseError("Failed to allocate memory", true);
		return;
	}
	Crypto::ZeroMemory(passwordInput);
}

void LockApplet::OnLeave()
{
	nameInput.clear();
}

void LockApplet::OpenSetHintValueModal(int idx)
{
	modalIdx = idx;
	openSetHintValueModal = true;
}

void LockApplet::OpenChangeHintModal(int idx)
{
	modalIdx = idx;
	openChangeHintModal = true;
}

void LockApplet::OpenDeleteModal(int idx)
{
	modalIdx = idx;
	openDeleteModal = true;
}

void LockApplet::OpenResetSaltsModal()
{
	openResetSalts = true;
}

void LockApplet::Render()
{
	RenderMain();
	RenderSetHintValueModal();
	RenderChangeHintModal();
	RenderDeleteModal();
	RenderResetSaltsModal();
}

void LockApplet::RenderMain()
{
	BeginVisibleChildWindow("##VaultEncryptor", ImVec2(617, 200), ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Passwords"))
			game.GetMainWindow().SwitchToMainView();
		if (ImGui::MenuItem("Save"))
			game.GetMainWindow().SaveVault();
		if (ImGui::MenuItem("Close"))
		{
			if (game.GetUnsavedState().HasChanged())
				game.GetMainWindow().OpenConfirmExitModal();
			else
				game.GetMainWindow().CloseVault();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::Button("Reset public tokens"))
		OpenResetSaltsModal();
	ImGui::Separator();

	Text("Vault Locks");
	ImGui::Separator();

	bool submit2 = ImGui::Button("Add");
	ImGui::SameLine();
	Text("Name:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	bool submit = ImGui::InputText("##HintNameInput", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_EnterReturnsTrue);
	if (submit || submit2)
	{
		game.GetKeeper().AddHint(nameInput.data());
		nameInput.clear();
	}

	constexpr auto flags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
	if (ImGui::BeginTable("HintEditor", 5, flags, ImVec2(600, 100), ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("No.");
		ImGui::TableSetupColumn("Hint", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Value");
		ImGui::TableSetupColumn("");
		ImGui::TableHeadersRow();

		auto& keeper = game.GetKeeper();
		keeper.LockDirectApi();

		int hintCount = keeper.GetHintCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			Text(StringUtils::ToString(i + 1));

			ImGui::TableNextColumn();
			Text(keeper.GetHint(i));

			ImGui::TableNextColumn();
			if (ImGui::Button("Change##ChangeName"))
				OpenChangeHintModal(i);

			ImGui::TableNextColumn();
			auto* title = "Set";
			if (keeper.IsKeyAssigned(i))
				title = "Change";
			if (ImGui::Button(title))
				OpenSetHintValueModal(i);

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete"))
				OpenDeleteModal(i);

			ImGui::PopID();
		}
		keeper.UnlockDirectApi();

		if (hintCount == 0)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TableNextColumn();
			Text("There are no hints. Consider adding one :)");
			ImGui::TableNextColumn();
			InvisibleButton("Change");
			ImGui::TableNextColumn();
			InvisibleButton("Delete");
		}

		ImGui::EndTable();
	}

	ImGui::EndChild();
}

void LockApplet::RenderSetHintValueModal()
{
	if (openSetHintValueModal)
	{
		openSetHintValueModal = false;
		ImGui::OpenPopup("Hint Value");
	}

	if (ImGui::BeginPopupModal("Hint Value", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto& keeper = game.GetKeeper();

		keeper.LockDirectApi();
		Text(keeper.GetHint(modalIdx));
		keeper.UnlockDirectApi();

		if (!vaultTask.valid())
		{
			constexpr auto flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo;
			bool submit = ImGui::InputText("##Password", passwordInput.str(), passwordInput.size(), flags);
			bool submit2 = ImGui::Button("Submit");

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				Crypto::ZeroMemory(passwordInput);
			}

			if (submit || submit2)
			{
				vaultTask = keeper.SetHintKey(modalIdx, passwordInput);
				Crypto::ZeroMemory(passwordInput);
			}
		}
		else
		{
			ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(-FLT_MIN, 0), "Hashing...");
			if (game.GetMainWindow().ProcessVaultResponse(vaultTask) != MainWindow::_TR_NoAction)
				ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void LockApplet::RenderChangeHintModal()
{
	if (openChangeHintModal)
	{
		openChangeHintModal = false;
		ImGui::OpenPopup("Change hint");

		nameInput = game.GetKeeper().GetHint(modalIdx);
	}

	if (ImGui::BeginPopupModal("Change hint", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("##Name", nameInput.data(), nameInput.capacity() - 1, ImGuiInputTextFlags_NoUndoRedo);

		if (ImGui::Button("Set"))
		{
			auto name = std::string_view(nameInput.data());
			game.GetKeeper().ChangeHint(modalIdx, name);
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			nameInput.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void LockApplet::RenderDeleteModal()
{
	if (openDeleteModal)
	{
		openDeleteModal = false;
		ImGui::OpenPopup("Delete Hint");
	}

	if (ImGui::BeginPopupModal("Delete Hint", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Are you sure you want to delete the hint?");
		Text("Name: ");
		ImGui::SameLine();

		auto& keeper = game.GetKeeper();
		keeper.LockDirectApi();

		Text(keeper.GetHint(modalIdx));
		if (ImGui::Button("Yes"))
		{
			keeper.RemoveHint(modalIdx);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
			ImGui::CloseCurrentPopup();

		keeper.UnlockDirectApi();
		ImGui::EndPopup();
	}
}

void LockApplet::RenderResetSaltsModal()
{
	if (openResetSalts)
	{
		openResetSalts = false;
		ImGui::OpenPopup("Reset Public Tokens");
	}

	if (ImGui::BeginPopupModal("Reset Public Tokens", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Text("Resetting public token requires setting hint values again.");

		if (ImGui::Button("Reset"))
		{
			game.GetKeeper().ResetSalts();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}
