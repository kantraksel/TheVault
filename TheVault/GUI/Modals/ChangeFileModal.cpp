#include <filesystem>
#include "ChangeFileModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"
#include "../Modals.hpp"

extern Game game;

ChangeFileModal::ChangeFileModal() : BaseIndexModal("Change File")
{
}

void ChangeFileModal::OnRender()
{
	auto& store = game.GetContentStore();
	Text("Name:");
	ImGui::SameLine();
	Text(store.GetName(selectedIndex));

	if (ImGui::Button("Select File"))
	{
		filePath = StringUtils::Utf8ToWideString(store.GetName(selectedIndex));
		if (WinApi::OpenFileDialog(L"Replace file in vault", filePath, filePath))
		{
			fileName = StringUtils::WideStringToUtf8(std::filesystem::path(filePath).filename().native());
		}
	}
	ImGui::SameLine();
	if (filePath.empty())
		Text("(not selected)");
	else
		Text(fileName);

	if (ImGui::Button("Change"))
	{
		if (!filePath.empty())
		{
			if (!store.SetFile(selectedIndex, filePath))
			{
				Modals::GetErrorModal().Open("Failed to read file - check permissions or logs for more details");
			}
			filePath.clear();
			fileName.clear();
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Change Name"))
	{
		filePath.clear();
		fileName.clear();
		ImGui::CloseCurrentPopup();
		Modals::GetChangeNameModal().Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		filePath.clear();
		fileName.clear();
		ImGui::CloseCurrentPopup();
	}
}
