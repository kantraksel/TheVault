#pragma once
#include <string_view>
#define IMGUI_DEFINE_MATH_OPERATORS
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

static void TextWidget(const std::string_view& label)
{
	using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = (wrap_pos_x >= 0.0f);
    if (label.size() <= 2000 || wrap_enabled)
    {
    	const char* text_begin;
    	const char* text_end;
    	if (label.empty())
    		text_begin = text_end = "";
    	else
    	{
    		text_begin = label.data();
    		text_end = label.data() + label.size();
    	}

        // Common case
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

    	// Reason the function exists: align text as it was in a widget
    	auto paddingY = GetStyle().FramePadding.y;
    	text_size.y += paddingY * 2.0f;

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size, 0.0f);
        if (!ItemAdd(bb, 0))
            return;

    	// Reason the function exists: align text as it was in a widget
    	bb.Min.y += paddingY;

        // Render (we don't hide text after ## in this end-user function)
        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
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

static void BeginVisibleChildWindow(const char* id, const ImVec2& size, ImGuiChildFlags flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags wndFlags = 0)
{
	CenterNextWindow();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
	ImGui::BeginChild(id, size, flags, wndFlags);
	ImGui::PopStyleColor();
}
