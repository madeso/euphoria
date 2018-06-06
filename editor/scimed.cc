#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"

#include "render/texture.h"

#include "window/imgui_ext.h"

#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

struct Canvas
{
  CanvasLogic view;
  ImVec2      position = ImVec2{0, 0};

  // user config
  float zoom_speed       = 10;
  float grid_size        = 64.0f;
  ImU32 grid_color       = IM_COL32(200, 200, 200, 40);
  ImU32 background_color = IM_COL32(60, 60, 70, 200);

  void
  ShowGrid()
  {
    const auto  size      = ImGui::GetWindowSize();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float scaled_grid_size = grid_size * view.scale;

    for(float x = fmodf(view.scroll.x, scaled_grid_size); x < size.x;
        x += scaled_grid_size)
    {
      draw_list->AddLine(
          ImVec2(x, 0.0f) + position, ImVec2(x, size.y) + position, grid_color);
    }

    for(float y = fmodf(view.scroll.y, scaled_grid_size); y < size.y;
        y += scaled_grid_size)
    {
      draw_list->AddLine(
          ImVec2(0.0f, y) + position, ImVec2(size.x, y) + position, grid_color);
    }
  }

  void
  VerticalLine(float rx, ImU32 grid_color)
  {
    const auto  x         = view.scroll.x + rx * view.scale;
    const auto  size      = ImGui::GetWindowSize();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(
        ImVec2(x, 0.0f) + position, ImVec2(x, size.y) + position, grid_color);
  }

  void
  HorizontalLine(float ry, ImU32 grid_color)
  {
    const auto  y         = view.scroll.y + ry * view.scale;
    const auto  size      = ImGui::GetWindowSize();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(
        ImVec2(0.0f, y) + position, ImVec2(size.x, y) + position, grid_color);
  }

  void
  ShowRuler(float ruler_interval, ImU32 ruler_color, float length)
  {
    const auto  size      = ImGui::GetWindowSize();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float scaled_grid_size = ruler_interval * view.scale;

    for(float x = fmodf(view.scroll.x, scaled_grid_size); x < size.x;
        x += scaled_grid_size)
    {
      draw_list->AddLine(
          ImVec2(x, 0.0f) + position,
          ImVec2(x, length) + position,
          ruler_color);
    }

    for(float y = fmodf(view.scroll.y, scaled_grid_size); y < size.y;
        y += scaled_grid_size)
    {
      draw_list->AddLine(
          ImVec2(0.0f, y) + position,
          ImVec2(length, y) + position,
          ruler_color);
    }
  }

  void
  ShowRuler()
  {
    ShowRuler(5.0f, grid_color, 10.0f);
  }

  void
  Begin()
  {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, background_color);

    ImGui::BeginChild(
        "scrolling_region",
        ImVec2(0, 0),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    position = ImGui::GetCursorScreenPos();
  }

  void
  DoCanvasScroll()
  {
    if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
       ImGui::IsMouseDragging(2, 0.0f))
    {
      view.Pan(C(ImGui::GetIO().MouseDelta));
    }
  }

  void
  DoCanvasZoom()
  {
    if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
    {
      const auto mouse = ImGui::GetIO().MousePos - position;
      const auto zoom  = ImGui::GetIO().MouseWheel;
      view.Zoom(C(mouse), zoom * zoom_speed);
    }
  }

  void
  End()
  {
    DoCanvasScroll();
    DoCanvasZoom();

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
  }

  ImVec2
  WorldToScreen(const ImVec2& v) const
  {
    return C(view.WorldToScreen(C(v))) + position;
  }

  ImVec2
  ScreenToWorld(const ImVec2& v)
  {
    return C(view.ScreenToWorld(C(v - position)));
  }
};

bool
IsCloseTo(float a, float b, float c = 3)
{
  return fabs(a - b) < c;
}

bool
Scimed::Run()
{
  static Canvas canvas;
  static ImVec2 mouse_popup;

  canvas.Begin();
  canvas.ShowGrid();
  if(texture)
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    auto        tex_id    = reinterpret_cast<ImTextureID>(texture->GetId());

    const auto pos = canvas.WorldToScreen(ImVec2{0, 0});
    const auto size =
        ImVec2{texture->GetWidth(), texture->GetHeight()} * canvas.view.scale;

    draw_list->AddImage(tex_id, pos, pos + size);
  }

  const auto line  = canvas.WorldToScreen(ImVec2{5, 5});
  const auto mouse = ImGui::GetMousePos();

  const bool close = IsCloseTo(mouse.y, line.y);

  canvas.HorizontalLine(
      5, close ? IM_COL32(0, 0, 255, 255) : IM_COL32(255, 0, 0, 255));
  if(close)
  {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
  }
  canvas.ShowRuler();
  canvas.End();

  if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
  {
    ImGui::OpenPopup("asd");
    mouse_popup = canvas.ScreenToWorld(ImGui::GetMousePos());
  }

  if(ImGui::BeginPopup("asd"))
  {
    ImGui::Text("%f %f", mouse_popup.x, mouse_popup.y);
    if(ImGui::Selectable("Ha ha"))
    {
    }
    ImGui::Selectable("dog");
    ImGui::EndPopup();
  }

  if(BeginFixedOverlay(ImguiCorner::TopRight, ""))
  {
    ImGui::Text("%f %f / %f %f", mouse.x, mouse.y, line.x, line.y);
    ImGui::End();
  }

  return false;
}
