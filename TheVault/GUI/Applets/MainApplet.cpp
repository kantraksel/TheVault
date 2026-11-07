#include <filesystem>
#include "Utils/ImGuiUtils.h"
#include "Utility/StringUtils.h"
#include "MainApplet.h"
#include "App/Game.h"
#include "Utils/WinApi.h"
#include "../Modals.hpp"

extern Game game;

void MainApplet::Render()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Settings"))
			game.GetMainWindow().SwitchToLockSetup();
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

	constexpr auto flags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
	if (ImGui::BeginTable("MainContent", 2, flags, ImVec2(0, 0), ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();

		auto& store = game.GetContentStore();
		int hintCount = store.GetCount();
		for (int i = 0; i < hintCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			TextWidget(store.GetName(i));

			ImGui::TableNextColumn();
			if (store.IsText(i))
			{
				if (ImGui::Button("Show"))
					Modals::GetShowPasswordWarningModal().Open(i);
				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					WinApi::SetClipboardText(store.GetText(i));
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					Modals::GetChangePasswordModal().Open(i);
			}
			else if (store.IsFile(i))
			{
				auto& style = ImGui::GetStyle();

				auto textSize = ImGui::CalcTextSize("Show", nullptr, true);
				float width = textSize.x + style.FramePadding.x * 2.0f;
				textSize = ImGui::CalcTextSize("Copy", nullptr, true);
				width += textSize.x + style.FramePadding.x * 2.0f;
				width += style.ItemSpacing.x;

				if (ImGui::Button("Extract", ImVec2(width, 0)))
				{
					auto fileName = StringUtils::Utf8ToWideString(store.GetName(i));
					if (WinApi::SaveFileDialog(L"Extract file", fileName, fileName))
					{
						if (!store.ExtractFile(i, fileName))
						{
							Modals::GetErrorModal().Open("Failed to extract file - check permissions or logs for more details");
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Change"))
					Modals::GetChangeFileModal().Open(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				Modals::GetDeleteEntryModal().Open(i);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (ImGui::Button("Add Password"))
			Modals::GetAddPasswordModal().Open();
		ImGui::SameLine();
		if (ImGui::Button("Add File"))
			Modals::GetAddFileModal().Open();

		ImGui::TableNextColumn();
		ImGui::BeginDisabled();
		ImGui::Button("Show");
		ImGui::SameLine();
		ImGui::Button("Copy");
		ImGui::SameLine();
		ImGui::Button("Change");
		ImGui::SameLine();
		ImGui::Button("Delete");
		ImGui::EndDisabled();

		ImGui::EndTable();
	}
}
