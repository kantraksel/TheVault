#include "ImGuiUtils.h"
#include "ErrorApplet.h"
#include "../../Game.h"
#include "../../WinApi.h"

extern Game game;

ErrorApplet::ErrorApplet()
{
	openErrorModal = false;
}

ErrorApplet::~ErrorApplet()
{
}

void ErrorApplet::Render()
{
	std::lock_guard lock(errorMutex);
	BeginVisibleChildWindow("##ErrorApplet", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);

	Text("App encountered critical error");
	ImGui::Separator();
	Text(errorText);

	if (ImGui::Button("Exit"))
		std::exit(1);

	ImGui::EndChild();
}

void ErrorApplet::RenderModal()
{
	if (openErrorModal)
	{
		openErrorModal = false;
		ImGui::OpenPopup("Error##ErrorModal");
	}

	if (ImGui::BeginPopupModal("Error##ErrorModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::lock_guard lock(errorMutex);
		Text(errorText);
		if (ImGui::Button("Close"))
		{
			errorText.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ErrorApplet::SetError(const std::string_view& text, bool openModal)
{
	std::lock_guard lock(errorMutex);
	errorText = text;
	if (openModal)
		openErrorModal = true;
}
