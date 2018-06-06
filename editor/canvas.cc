#include "editor/canvas.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "window/imgui_ext.h"

void
Canvas::ShowGrid()
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
Canvas::VerticalLine(float rx, ImU32 grid_color)
{
  const auto  x         = view.scroll.x + rx * view.scale;
  const auto  size      = ImGui::GetWindowSize();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddLine(
      ImVec2(x, 0.0f) + position, ImVec2(x, size.y) + position, grid_color);
}

void
Canvas::HorizontalLine(float ry, ImU32 grid_color)
{
  const auto  y         = view.scroll.y + ry * view.scale;
  const auto  size      = ImGui::GetWindowSize();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddLine(
      ImVec2(0.0f, y) + position, ImVec2(size.x, y) + position, grid_color);
}

void
Canvas::ShowRuler(float ruler_interval, ImU32 ruler_color, float length)
{
  const auto  size      = ImGui::GetWindowSize();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const float scaled_grid_size = ruler_interval * view.scale;

  for(float x = fmodf(view.scroll.x, scaled_grid_size); x < size.x;
      x += scaled_grid_size)
  {
    draw_list->AddLine(
        ImVec2(x, 0.0f) + position, ImVec2(x, length) + position, ruler_color);
  }

  for(float y = fmodf(view.scroll.y, scaled_grid_size); y < size.y;
      y += scaled_grid_size)
  {
    draw_list->AddLine(
        ImVec2(0.0f, y) + position, ImVec2(length, y) + position, ruler_color);
  }
}

void
Canvas::ShowRuler()
{
  ShowRuler(5.0f, grid_color, 10.0f);
}

void
Canvas::Begin()
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
Canvas::DoCanvasScroll()
{
  if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
     ImGui::IsMouseDragging(2, 0.0f))
  {
    view.Pan(C(ImGui::GetIO().MouseDelta));
  }
}

void
Canvas::DoCanvasZoom()
{
  if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
  {
    const auto mouse = ImGui::GetIO().MousePos - position;
    const auto zoom  = ImGui::GetIO().MouseWheel;
    view.Zoom(C(mouse), zoom * zoom_speed);
  }
}

void
Canvas::End()
{
  DoCanvasScroll();
  DoCanvasZoom();

  ImGui::EndChild();

  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);
}

ImVec2
Canvas::WorldToScreen(const ImVec2& v) const
{
  return C(view.WorldToScreen(C(v))) + position;
}

ImVec2
Canvas::WorldToScreenSize(const ImVec2& v) const
{
  return v * view.scale;
}

ImVec2
Canvas::ScreenToWorld(const ImVec2& v) const
{
  return C(view.ScreenToWorld(C(v - position)));
}

ImVec2
Canvas::GetMouse() const
{
  return ImGui::GetMousePos() - position;
}
