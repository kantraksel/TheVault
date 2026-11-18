#include <filesystem>
#include "AddFileModal.hpp"
#include "ErrorModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"

AddFileModal::AddFileModal() : BaseModal("Add File")
{
	nameInput.resize(256);
}

void AddFileModal::OnClose()
{
	memset(nameInput.data(), 0, nameInput.size());
	filePath.clear();
	fileName.clear();
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
			{
				memcpy_s(nameInput.data(), nameInput.size(), name.data(), name.size());
				if (nameInput.size() > name.size())
					nameInput[name.size()] = 0;
			}
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
			if (!ContentStore::Get().AddFile(name, filePath))
			{
				ErrorModal::Get().Open("Failed to read file - check permissions or logs for more details");
			}
			Close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
