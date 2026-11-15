#include "ProcessApplet.h"
#include "Utils/ImGuiUtils.h"

ProcessApplet::ProcessApplet() : isOpen{false}
{
}

ProcessApplet::~ProcessApplet()
{
}

void ProcessApplet::Open(const std::string_view& title)
{
	isOpen = true;
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
}
