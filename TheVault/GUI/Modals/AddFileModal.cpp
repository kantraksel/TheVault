#include <filesystem>
#include "AddFileModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"
#include "../Modals.hpp"

extern Game game;

AddFileModal::AddFileModal() : BaseModal("Add File")
{
	nameInput.resize(256);
}

void AddFileModal::OnRender()
{
	TextWidget("Name:");
	ImGui::SameLine();

	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();
	ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, ImGuiInputTextFlags_NoUndoRedo);
	if (ImGui::Button("Select File"))
	{
		if (WinApi::OpenFileDialog(L"Place new file in the vault", L"", filePath))
		{
			fileName = StringUtils::WideStringToUtf8(std::filesystem::path(filePath).filename().native());

			auto name = std::string_view(nameInput.data());
			if (name.empty())
				nameInput = fileName;
		}
	}
	ImGui::SameLine();
	if (filePath.empty())
		Text("(not selected)");
	else
		Text(fileName);

	if (ImGui::Button("Add"))
	{
		if (!filePath.empty())
		{
			auto name = std::string_view(nameInput.data());
			if (!game.GetContentStore().AddFile(name, filePath))
			{
				Modals::GetErrorModal().Open("Failed to read file - check permissions or logs for more details");
			}
			memset(nameInput.data(), 0, nameInput.size());
			filePath.clear();
			fileName.clear();
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		memset(nameInput.data(), 0, nameInput.size());
		filePath.clear();
		fileName.clear();
		ImGui::CloseCurrentPopup();
	}
}
