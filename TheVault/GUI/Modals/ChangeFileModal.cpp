#include <filesystem>
#include "ChangeFileModal.hpp"
#include "ChangeNameModal.hpp"
#include "ErrorModal.hpp"
#include "App/ContentStore.h"
#include "Utils/ImGuiUtils.h"
#include "Utils/WinApi.h"
#include "Utility/StringUtils.h"

ChangeFileModal::ChangeFileModal(ChangeNameModal& changeNameModal) : BaseIndexModal("Change File"), changeNameModal(changeNameModal)
{
}

void ChangeFileModal::OnClose()
{
	filePath.clear();
	fileName.clear();
}

void ChangeFileModal::OnRender()
{
	auto& store = ContentStore::Get();
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
				ErrorModal::Get().Open("Failed to read file - check permissions or logs for more details");
			}
			Close();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Change Name"))
	{
		Close();
		changeNameModal.Open(selectedIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		Close();
}
