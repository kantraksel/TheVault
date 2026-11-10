#include <string>
#include "Utils/ImGuiUtils.h"
#include "LockApplet.h"
#include "App/Game.h"
#include "Utils/StringUtils.h"
#include "../Modals.hpp"

extern Game game;

LockApplet::LockApplet()
{
	nameInput.reserve(256);
}

LockApplet::~LockApplet()
{
}

void LockApplet::OnLeave()
{
	nameInput.clear();
}

void LockApplet::Render()
{
	BeginVisibleChildWindow("##VaultEncryptor", ImVec2(617, 200), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Passwords"))
			game.GetMainWindow().SwitchToMainView();
		if (ImGui::MenuItem("Save"))
			game.GetMainWindow().SaveVault();
		if (ImGui::MenuItem("Close"))
		{
			if (game.GetUnsavedState().HasChanged())
				Modals::GetConfirmExitModal().Open();
			else
				game.GetMainWindow().CloseVault();
		}
		ImGui::EndMenuBar();
	}

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

		auto keeper = game.GetKeeper().GetDirectApi();

		int hintCount = keeper.GetHintCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			Text(StringUtils::ToStringNoAlloc(i + 1));

			ImGui::TableNextColumn();
			Text(keeper.GetHint(i));

			ImGui::TableNextColumn();
			if (ImGui::Button("Change##ChangeName"))
				Modals::GetChangeNameModal().Open(i, true);

			ImGui::TableNextColumn();
			auto* title = "Set";
			if (keeper.IsKeyAssigned(i))
				title = "Change";
			if (ImGui::Button(title))
				Modals::GetChangeLockPasswordModal().Open(i);

			ImGui::TableNextColumn();
			if (ImGui::Button("Delete"))
				Modals::GetDeleteEntryModal().Open(i, true);

			ImGui::PopID();
		}

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
