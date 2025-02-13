#pragma once
#include <string_view>
#include <imgui.h>
#include <imgui_internal.h>

static void SetNextLabelAlign(const char* label, float maxSize = 320.0f)
{
	auto& style = ImGui::GetStyle();

	auto width = ImGui::CalcTextSize(label, nullptr, true).x;
	width = ImGui::GetContentRegionAvail().x - width - style.ItemInnerSpacing.x;
	width = ImMin(width, maxSize);
	ImGui::SetNextItemWidth(width);
}

static void SetNextRightButtonAlign(const char* label)
{
	auto& style = ImGui::GetStyle();

	auto width = ImGui::CalcTextSize(label, nullptr, true).x;
	width += style.FramePadding.x * 2.0f + style.ItemSpacing.x;
	ImGui::SetNextItemWidth(-width);
}

static bool ButtonRightAligned(const char* label)
{
	auto& style = ImGui::GetStyle();

	auto textSize = ImGui::CalcTextSize(label, nullptr, true);
	float width = textSize.x + style.FramePadding.x * 2.0f + style.ItemSpacing.x;
	width = ImGui::GetContentRegionAvail().x - width;
	ImGui::Dummy(ImVec2(width, 0));

	ImGui::SameLine();
	return ImGui::Button(label);
}

static void Text(const std::string_view& label)
{
	ImGui::TextUnformatted(label.data(), label.data() + label.size());
}

static void InvisibleButton(const char* label)
{
	auto& style = ImGui::GetStyle();
	auto label_size = ImGui::CalcTextSize(label, nullptr, true);
	ImVec2 size = ImGui::CalcItemSize({ 0.0f, 0.0f }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
	ImGui::InvisibleButton(label, size);
}

static void CenterNextWindow()
{
	auto* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetCenter(), 0, ImVec2(0.5f, 0.5f));
}

static void BeginVisibleChildWindow(const char* id, const ImVec2& size, ImGuiChildFlags flags = ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags wndFlags = 0)
{
	CenterNextWindow();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
	ImGui::BeginChild(id, size, flags, wndFlags);
	ImGui::PopStyleColor();
}
