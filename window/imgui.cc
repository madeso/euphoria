#include "window/imgui.h"

#include "core/angle.h"
#include "core/assert.h"

#include <imgui/imgui.h>

#include "render/texture.h"

void
ImguiAngleSlider(
    const char* const name, Angle* angle, float mindeg, float maxdeg)
{
  ASSERT(angle);

  float degrees = angle->InDegrees();
  ImGui::SliderFloat(name, &degrees, mindeg, maxdeg);
  *angle = Angle::FromDegrees(degrees);
}

void
ImGuiColorEdit3(const char* const name, Rgb* rgb)
{
  ImGui::ColorEdit3(name, &rgb->r);
}

void
ImguiImage(Texture2d* texture)
{
  auto        tex_w  = static_cast<float>(texture->GetWidth());
  auto        tex_h  = static_cast<float>(texture->GetHeight());
  ImTextureID tex_id = reinterpret_cast<ImTextureID>(texture->GetId());

  ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
  ImGui::Text("%.0fx%.0f", tex_w, tex_h);
  ImGui::Image(
      tex_id,
      ImVec2(tex_w, tex_h),
      ImVec2(0, 0),
      ImVec2(1, 1),
      ImColor(255, 255, 255, 255),
      ImColor(255, 255, 255, 128));
  if(ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    float focus_sz = 32.0f;
    float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f;
    if(focus_x < 0.0f)
    {
      focus_x = 0.0f;
    }
    else if(focus_x > tex_w - focus_sz)
    {
      focus_x = tex_w - focus_sz;
    }
    float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f;
    if(focus_y < 0.0f)
    {
      focus_y = 0.0f;
    }
    else if(focus_y > tex_h - focus_sz)
    {
      focus_y = tex_h - focus_sz;
    }
    ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
    ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
    ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
    ImVec2 uv1 =
        ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
    ImGui::Image(
        tex_id,
        ImVec2(128, 128),
        uv0,
        uv1,
        ImColor(255, 255, 255, 255),
        ImColor(255, 255, 255, 128));
    ImGui::EndTooltip();
  }
}

// stolen from ShowExampleAppFixedOverlay function in imgui_demo
bool
BeginFixedOverlay(ImguiCorner corner, const std::string& title)
{
  const int   corner_int = static_cast<int>(corner);
  const float distance   = 10.0f;
  const auto  size       = ImGui::GetIO().DisplaySize;
  ImVec2      window_pos =
      corner == ImguiCorner::Center
          ? ImVec2(size.x / 2, size.y / 2)
          : ImVec2(
                (corner_int & 1) ? size.x - distance : distance,
                (corner_int & 2) ? size.y - distance : distance);
  ImVec2 window_pos_pivot =
      corner == ImguiCorner::Center
          ? ImVec2(0.5f, 0.5f)
          : ImVec2(
                (corner_int & 1) ? 1.0f : 0.0f, (corner_int & 2) ? 1.0f : 0.0f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  return ImGui::Begin(
      title.c_str(),
      nullptr,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
          ImGuiWindowFlags_NoSavedSettings);
}
