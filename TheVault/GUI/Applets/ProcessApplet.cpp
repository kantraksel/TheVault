#include "ProcessApplet.h"
#include "Utils/ImGuiUtils.h"
#include "App/Game.h"

extern Game game;

ProcessApplet::ProcessApplet() : isOpen{false}
{
}

ProcessApplet::~ProcessApplet()
{
}

void ProcessApplet::Open(std::future<uint64_t>&& task, const std::string_view& title)
{
	isOpen = true;
	vaultTask = std::move(task);
	this->title = title.empty() ? "Loading..." : title;
}

void ProcessApplet::Close()
{
	isOpen = false;
}

void ProcessApplet::Render()
{
	if (!isOpen)
		return;
	CenterNextWindow();
	ImGui::BeginChild("##GlobalProcess", ImVec2(300, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoBackground);
	ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), title.c_str());
	ImGui::EndChild();

	auto result = game.GetMainWindow().ProcessVaultResponse(vaultTask);
	if (result != MainWindow::_TR_NoAction)
		Close();
}
