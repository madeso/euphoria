#include "imgui_extra.h"

#include "imgui.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <imgui_internal.h>

namespace imgui
{
  bool
  Knob(const char* label, float* p_value, float v_min, float v_max)
  {
    constexpr auto  pi              = static_cast<float>(M_PI);
    constexpr auto  rad2deg         = 180 / pi;
    constexpr auto  pi2             = pi * 2;
    constexpr float angle_min       = pi * 0.75f;
    constexpr float angle_max       = pi * 2.25f;
    constexpr float angle_step      = 20 * (pi / 180);
    constexpr float size_outer      = 20;
    constexpr float peg_max_end     = size_outer;
    constexpr float peg_end         = 19;
    constexpr float peg_start       = 15;
    constexpr float knob_size       = 15;
    constexpr float knob_mark_start = 15;
    constexpr float knob_mark_end   = 8;
    constexpr int   seg             = 16;

    ImGuiIO&    io        = ImGui::GetIO();
    ImGuiStyle& style     = ImGui::GetStyle();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const ImVec2 start  = ImGui::GetCursorScreenPos();
    const ImVec2 center = ImVec2(start.x + size_outer, start.y + size_outer);
    const float  line_height = ImGui::GetTextLineHeight();

    ImGui::InvisibleButton(
        label,
        ImVec2(
            size_outer * 2,
            size_outer * 2 + line_height + style.ItemInnerSpacing.y));
    const bool is_active  = ImGui::IsItemActive();
    const bool is_hovered = ImGui::IsItemHovered();

    const float t     = (*p_value - v_min) / (v_max - v_min);
    const float angle = angle_min + (angle_max - angle_min) * t;

    // changing value
    bool value_changed = false;

    if(true)
    {
      ImVec2      direct(io.MousePos.x - center.x, io.MousePos.y - center.y);
      const float directl = sqrtf(direct.x * direct.x + direct.y * direct.y);
      direct.x            = direct.x / directl;
      direct.y            = direct.y / directl;

      // todo: flip y based on x
      const auto acos        = acosf(direct.x);
      const auto ang         = direct.y > 0 ? pi2 - acos : acos;
      float      input_angle = -ang;
      input_angle += pi2;
      if(ang < pi2+pi/2) {
      }
      else
      {
        input_angle += pi2;
      }

      const float input_angle_t =
          (input_angle - angle_min) / (angle_max - angle_min);

      if(is_active && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
      {
        *p_value = v_min + (v_max - v_min) * input_angle_t;

        if(*p_value < v_min)
          *p_value = v_min;
        if(*p_value > v_max)
          *p_value = v_max;
      }

      std::ostringstream ss;
      ss << ang * rad2deg << "/" << input_angle * rad2deg;
      // ss << " / " << direct.y << " / " << input_angle;
      draw_list->AddText(
          ImVec2(start.x, start.y + size_outer * 2 + style.ItemInnerSpacing.y),
          ImGui::GetColorU32(ImGuiCol_Text),
          ss.str().c_str());
    }
    else
    {
      if(is_active && io.MouseDelta.x != 0.0f)
      {
        float step = (v_max - v_min) / 200.0f;
        *p_value += io.MouseDelta.x * step;
        if(*p_value < v_min)
          *p_value = v_min;
        if(*p_value > v_max)
          *p_value = v_max;

        value_changed = true;
      }
    }

    // colors
    const auto label_color     = ImGui::GetColorU32(ImGuiCol_Text);
    const auto fill_color      = ImGui::GetColorU32(ImGuiCol_FrameBg);
    const auto knob_color      = ImGui::GetColorU32(ImGuiCol_SliderGrab);
    const auto indicator_color = ImGui::GetColorU32(ImGuiCol_Text);
    const auto peg_color_off   = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    const auto peg_color_max   = ImGui::GetColorU32(ImGuiCol_Text);

    // util function
    const auto Pos = [=](float angle, float rad) -> ImVec2 {
      return ImVec2(center.x + cosf(angle) * rad, center.y + sinf(angle) * rad);
    };

    // ----------------- visualization
    // background
    draw_list->AddCircleFilled(center, size_outer, fill_color, seg);

    // peg indicators
    for(float a = angle_min; a <= angle; a += angle_step)
    {
      draw_list->AddLine(
          Pos(a, peg_start), Pos(a, peg_end), peg_color_off, 1.0f);
    }
    draw_list->AddLine(
        Pos(angle_max, peg_start),
        Pos(angle_max, peg_max_end),
        peg_color_max,
        1.0f);
    draw_list->AddLine(
        Pos(angle_min, peg_start),
        Pos(angle_min, peg_max_end),
        peg_color_max,
        1.0f);

    // the knob
    draw_list->AddCircleFilled(center, knob_size, knob_color, seg);
    draw_list->AddLine(
        Pos(angle, knob_mark_start),
        Pos(angle, knob_mark_end),
        indicator_color,
        2.0f);

    // knob control name
    if(false)
      draw_list->AddText(
          ImVec2(start.x, start.y + size_outer * 2 + style.ItemInnerSpacing.y),
          label_color,
          label);

    // tooltip
    if(is_active || is_hovered)
    {
      ImGui::SetNextWindowPos(ImVec2(
          start.x - style.WindowPadding.x,
          start.y - line_height - style.ItemInnerSpacing.y -
              style.WindowPadding.y));
      ImGui::BeginTooltip();
      ImGui::Text("%.3f", *p_value);
      ImGui::EndTooltip();
    }

    return value_changed;
  }
}