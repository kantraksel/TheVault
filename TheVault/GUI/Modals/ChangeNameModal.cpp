#include "ChangeNameModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

ChangeNameModal::ChangeNameModal() : BaseDualIndexModal("Change Name")
{
	nameInput.resize(256);
}

void ChangeNameModal::OnRender()
{
	if (firstFrameDraw)
		ImGui::SetKeyboardFocusHere();

	constexpr auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;
	bool submit = ImGui::InputText("##Name", nameInput.data(), nameInput.size() - 1, flags);
	if (ImGui::Button("Change") || submit)
	{
		auto name = std::string_view(nameInput.data());
		if (!name.empty())
		{
			if (!isLockIndex)
				game.GetContentStore().SetName(selectedIndex, name);
			else
				game.GetKeeper().ChangeHint(selectedIndex, name);

			memset(nameInput.data(), 0, nameInput.size());
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		memset(nameInput.data(), 0, nameInput.size());
		ImGui::CloseCurrentPopup();
	}
}

void ChangeNameModal::OnOpen()
{
	std::string_view name;
	if (!isLockIndex)
		name = game.GetContentStore().GetName(selectedIndex);
	else
		name = game.GetKeeper().GetDirectApi().GetHint(selectedIndex);

	memcpy_s(nameInput.data(), nameInput.size(), name.data(), name.size());
	if (nameInput.size() > name.size())
		nameInput[name.size()] = 0;
}
