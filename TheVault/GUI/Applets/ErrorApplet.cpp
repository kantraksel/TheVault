#include "Utils/ImGuiUtils.h"
#include "ErrorApplet.h"
#include "App/Game.h"
#include "Engine/GhostFries.h"

extern Game game;

ErrorApplet::ErrorApplet()
{
}

ErrorApplet::~ErrorApplet()
{
}

void ErrorApplet::Render()
{
	BeginVisibleChildWindow("##ErrorApplet", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);

	Text("Critical error");
	ImGui::Separator();
	TextWrapped(errorText);

	if (ImGui::Button("Exit"))
		GhostFries::Quit();

	ImGui::EndChild();
}

void ErrorApplet::SetError(const std::string_view& text)
{
	errorText = text;
}
