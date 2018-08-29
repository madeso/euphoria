
#include "solarized.h"
#include "imgui.h"

void
SetupSolarized(bool light, ImGuiStyle* dst)
{
  using Color = int;

  constexpr Color base03  = 0x002B36FF;
  constexpr Color base02  = 0x073642FF;
  constexpr Color base01  = 0x586E75FF;
  constexpr Color base00  = 0x657B83FF;
  constexpr Color base0   = 0x839496FF;
  constexpr Color base1   = 0x93A1A1FF;
  constexpr Color base2   = 0xEEE8D5FF;
  constexpr Color base3   = 0xFDF6E3FF;
  constexpr Color yellow  = 0xB58900FF;
  constexpr Color orange  = 0xCB4B16FF;
  constexpr Color red     = 0xDC322FFF;
  constexpr Color magenta = 0xD33682FF;
  constexpr Color violet  = 0x6C71C4FF;
  constexpr Color blue    = 0x268BD2FF;
  constexpr Color cyan    = 0x2AA198FF;
  constexpr Color green   = 0x859900FF;

  const auto really_strong_border = light ? base03 : base3;
  const auto strong_border        = light ? base02 : base2;
  const auto emphasized_content   = light ? base01 : base1;
  const auto primary_content      = light ? base00 : base0;
  const auto not_used             = light ? base0 : base00;
  const auto comments             = light ? base1 : base01;
  const auto background_highlight = light ? base2 : base02;
  const auto background           = light ? base3 : base03;

  ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();

  auto C = [](Color color, float a = 1.0f) -> ImVec4 {
    const int r = (color >> 24) & 0xff;
    const int g = (color >> 16) & 0xff;
    const int b = (color >> 8) & 0xff;
    return ImVec4{r / 255.0f, g / 255.0f, b / 255.0f, a};
  };

  ImVec4* colors = style->Colors;

  colors[ImGuiCol_Text]                  = C(primary_content);
  colors[ImGuiCol_TextDisabled]          = C(comments);
  colors[ImGuiCol_WindowBg]              = C(background);
  colors[ImGuiCol_ChildWindowBg]         = C(background);
  colors[ImGuiCol_PopupBg]               = C(background);
  colors[ImGuiCol_Border]                = C(primary_content);
  colors[ImGuiCol_BorderShadow]          = C(background, 0);
  colors[ImGuiCol_FrameBg]               = C(background_highlight);
  colors[ImGuiCol_FrameBgHovered]        = C(background);
  colors[ImGuiCol_FrameBgActive]         = C(background);
  colors[ImGuiCol_TitleBg]               = C(background);
  colors[ImGuiCol_TitleBgActive]         = C(background_highlight);
  colors[ImGuiCol_TitleBgCollapsed]      = C(background);
  colors[ImGuiCol_MenuBarBg]             = C(background);
  colors[ImGuiCol_ScrollbarBg]           = C(background);
  colors[ImGuiCol_ScrollbarGrab]         = C(comments);
  colors[ImGuiCol_ScrollbarGrabHovered]  = C(blue);
  colors[ImGuiCol_ScrollbarGrabActive]   = C(primary_content);
  colors[ImGuiCol_CheckMark]             = C(primary_content);
  colors[ImGuiCol_SliderGrab]            = C(primary_content, 0.5f);
  colors[ImGuiCol_SliderGrabActive]      = C(primary_content, 0.3f);
  colors[ImGuiCol_Button]                = C(background);
  colors[ImGuiCol_ButtonHovered]         = C(background_highlight);
  colors[ImGuiCol_ButtonActive]          = C(background_highlight);
  colors[ImGuiCol_Header]                = C(background_highlight);
  colors[ImGuiCol_HeaderHovered]         = C(background_highlight);
  colors[ImGuiCol_HeaderActive]          = C(background_highlight);
  colors[ImGuiCol_Separator]             = C(comments);
  colors[ImGuiCol_SeparatorHovered]      = C(background_highlight);
  colors[ImGuiCol_SeparatorActive]       = C(primary_content);
  colors[ImGuiCol_ResizeGrip]            = C(comments);
  colors[ImGuiCol_ResizeGripHovered]     = C(blue);
  colors[ImGuiCol_ResizeGripActive]      = C(primary_content);
  colors[ImGuiCol_PlotLines]             = C(comments);
  colors[ImGuiCol_PlotLinesHovered]      = C(primary_content);
  colors[ImGuiCol_PlotHistogram]         = C(yellow, 0.9f);
  colors[ImGuiCol_PlotHistogramHovered]  = C(yellow);
  colors[ImGuiCol_TextSelectedBg]        = C(background_highlight);
  colors[ImGuiCol_DragDropTarget]        = C(blue);
  colors[ImGuiCol_NavHighlight]          = C(red);
  colors[ImGuiCol_NavWindowingHighlight] = C(background);
  colors[ImGuiCol_NavWindowingDimBg]     = C(background, 0.6f);
  colors[ImGuiCol_ModalWindowDimBg]      = C(background, 0.6f);
}
