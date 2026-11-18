#include "ProcessApplet.h"
#include "Utils/ImGuiUtils.h"

ProcessApplet::ProcessApplet(MainWindow& mainWindow) : BlockingApplet(mainWindow)
{
}

ProcessApplet::~ProcessApplet()
{
}

void ProcessApplet::Open(const std::string_view& title)
{
	BlockingApplet::Open();
	this->title = title.empty() ? "Loading..." : title;
}

void ProcessApplet::Close()
{
	BlockingApplet::Close();
	title = "Loading...";
}

void ProcessApplet::OnRender()
{
	CenterNextWindow();
	ImGui::BeginChild("##GlobalProcess", ImVec2(300, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoBackground);
	ImGui::ProgressBar(-1.0f * static_cast<float>(ImGui::GetTime()), ImVec2(-FLT_MIN, 0), title.c_str());
	ImGui::EndChild();
}
