#include "LockApplet.h"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "Utils/StringUtils.h"
#include "../Modals.hpp"

extern Game game;

LockApplet::LockApplet() : isOpen{}
{
}

LockApplet::~LockApplet()
{
}

void LockApplet::Open()
{
	isOpen = true;
}

void LockApplet::Close()
{
	isOpen = false;
}

void LockApplet::Render()
{
	if (!isOpen)
		return;
	BeginVisibleChildWindow("##VaultEncryptor", ImVec2(617, 200), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		Text("Vault Locks");

		PadMenuItem("Close");
		if (ImGui::MenuItem("Close"))
			Close();

		ImGui::EndMenuBar();
	}

	constexpr auto flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY;
	if (ImGui::BeginTable("HintEditor", 3, flags))
	{
		ImGui::TableSetupColumn("No.");
		ImGui::TableSetupColumn("Lock", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Actions");
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
			auto title = "Change";
			float width = 0;

			if (!keeper.IsKeyAssigned(i))
			{
				auto& style = ImGui::GetStyle();
				auto textSize = ImGui::CalcTextSize(title, nullptr, true);
				width = textSize.x + style.FramePadding.x * 2.0f;
				title = "Set";
			}

			if (ImGui::Button(title, ImVec2(width, 0)))
				Modals::GetChangeLockPasswordModal().Open(i);
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
				Modals::GetDeleteEntryModal().Open(i, true);

			ImGui::PopID();
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		if (ImGui::Button("New Vault Lock"))
			Modals::GetAddLockModal().Open();
		ImGui::TableNextColumn();
		ImGui::BeginDisabled();
		ImGui::Button("Change");
		ImGui::SameLine();
		ImGui::Button("Delete");
		ImGui::EndDisabled();

		ImGui::EndTable();
	}

	ImGui::EndChild();
}
