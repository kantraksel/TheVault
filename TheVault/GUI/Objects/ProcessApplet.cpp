#include "ImGuiUtils.h"
#include "ProcessApplet.h"
#include "../../Game.h"
#include "../../WinApi.h"

extern Game game;

ProcessApplet::ProcessApplet()
{
	title = "Loading...";
}

ProcessApplet::~ProcessApplet()
{
}

void ProcessApplet::Render()
{
	CenterNextWindow();
	ImGui::BeginChild("##GlobalProcess", ImVec2(300, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoBackground);
	ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(-FLT_MIN, 0), title.c_str());
	ImGui::EndChild();

	game.GetMainWindow().ProcessVaultResponse(vaultTask);
}

void ProcessApplet::ProcessVaultTask(std::future<uint64_t>&& task)
{
	vaultTask = std::move(task);
}

void ProcessApplet::SetTitle(const std::string_view& title)
{
	if (title.empty())
		this->title = "Loading...";
	else
		this->title = title;
}
