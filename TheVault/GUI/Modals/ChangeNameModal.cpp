#include "ChangeNameModal.hpp"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

ChangeNameModal::ChangeNameModal() : BaseIndexModal("Change Name")
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
			game.GetContentStore().SetName(selectedIndex, name);
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
	// TODO: fix - fill buffer, but dont change size
	nameInput = game.GetContentStore().GetName(selectedIndex);
}
