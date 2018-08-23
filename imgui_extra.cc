#include "imgui_extra.h"

#include "imgui.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace imgui
{
  bool
  Knob(const char* label, float* p_value, float v_min, float v_max)
  {
    constexpr auto  pi           = static_cast<float>(M_PI);
    constexpr float angle_min    = pi * 0.75f;
    constexpr float angle_max    = pi * 2.25f;
    constexpr float radius_outer = 20.0f;
    constexpr float radius_inner = radius_outer * 0.40f;

    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec2      pos    = ImGui::GetCursorScreenPos();
    ImVec2      center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
    float       line_height = ImGui::GetTextLineHeight();
    ImDrawList* draw_list   = ImGui::GetWindowDrawList();

    ImGui::InvisibleButton(
        label,
        ImVec2(
            radius_outer * 2,
            radius_outer * 2 + line_height + style.ItemInnerSpacing.y));
    const bool is_active  = ImGui::IsItemActive();
    const bool is_hovered = ImGui::IsItemHovered();

    // changing value
    const bool value_changed = is_active && io.MouseDelta.x != 0.0f;
    if(value_changed)
    {
      float step = (v_max - v_min) / 200.0f;
      *p_value += io.MouseDelta.x * step;
      if(*p_value < v_min)
        *p_value = v_min;
      if(*p_value > v_max)
        *p_value = v_max;
    }

    const float t         = (*p_value - v_min) / (v_max - v_min);
    const float angle     = angle_min + (angle_max - angle_min) * t;
    const float angle_cos = cosf(angle);
    const float angle_sin = sinf(angle);

    // colors
    const auto label_color        = ImGui::GetColorU32(ImGuiCol_Text);
    const auto outer_circle_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
    const auto value_indicator_color =
        ImGui::GetColorU32(ImGuiCol_SliderGrabActive);
    const auto inner_circle_color = ImGui::GetColorU32(
        is_active ? ImGuiCol_FrameBgActive
                  : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

    // visualization
    draw_list->AddCircleFilled(center, radius_outer, outer_circle_color, 16);
    draw_list->AddLine(
        ImVec2(
            center.x + angle_cos * radius_inner,
            center.y + angle_sin * radius_inner),
        ImVec2(
            center.x + angle_cos * (radius_outer - 2),
            center.y + angle_sin * (radius_outer - 2)),
        value_indicator_color,
        2.0f);
    draw_list->AddCircleFilled(center, radius_inner, inner_circle_color, 16);
    draw_list->AddText(
        ImVec2(pos.x, pos.y + radius_outer * 2 + style.ItemInnerSpacing.y),
        label_color,
        label);


    // tooltip
    if(is_active || is_hovered)
    {
      ImGui::SetNextWindowPos(ImVec2(
          pos.x - style.WindowPadding.x,
          pos.y - line_height - style.ItemInnerSpacing.y -
              style.WindowPadding.y));
      ImGui::BeginTooltip();
      ImGui::Text("%.3f", *p_value);
      ImGui::EndTooltip();
    }

    return value_changed;
  }
}