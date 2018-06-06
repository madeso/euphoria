#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"
#include "core/str.h"
#include "core/numeric.h"

#include "render/texture.h"

#include "window/imgui_ext.h"

#include "editor/canvas.h"

bool
IsCloseTo(float a, float b, float c = 3)
{
  return Abs(a - b) < c;
}

bool
Scimed::Run()
{
  static Canvas canvas;
  static vec2f  mouse_popup = vec2f{0, 0};

  canvas.Begin();
  canvas.ShowGrid();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  if(texture)
  {
    auto tex_id = reinterpret_cast<ImTextureID>(texture->GetId());

    const auto pos = canvas.WorldToScreen(ImVec2{0, 0});
    const auto size =
        canvas.WorldToScreen(ImVec2{texture->GetWidth(), texture->GetHeight()});

    draw_list->AddImage(tex_id, pos, size);
  }

  draw_list->AddText(
      canvas.WorldToScreen(ImVec2{100, 100}),
      IM_COL32(255, 255, 255, 255),
      "Hello world");

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
    mouse_popup = C(canvas.ScreenToWorld(ImGui::GetMousePos()));
  }

  if(ImGui::BeginPopup("asd"))
  {
    ImguiLabel(Str() << "Mouse: " << mouse_popup);
    if(ImGui::Selectable("Ha ha"))
    {
    }
    ImGui::Selectable("dog");
    ImGui::EndPopup();
  }


  // debug
  if(BeginFixedOverlay(ImguiCorner::TopRight, ""))
  {
    ImguiLabel(Str() << "Mouse: " << C(mouse) - C(canvas.position));
    ImGui::End();
  }

  return false;
}
