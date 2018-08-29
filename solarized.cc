
#include "solarized.h"
#include "imgui.h"

using Color = int;

// solarized light color spec
namespace solarized_light
{
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
}  // namespace solarized_light

struct Solarized
{
  explicit Solarized(bool light);

  Color really_strong_border;
  Color strong_border;
  Color emphasized_content;
  Color primary_content;
  Color not_used;
  Color comments;
  Color background_highlight;
  Color background;

  Color yellow;
  Color orange;
  Color red;
  Color magenta;
  Color violet;
  Color blue;
  Color cyan;
  Color green;
};

Solarized::Solarized(bool light)
    : really_strong_border(
          light ? solarized_light::base03 : solarized_light::base3)
    , strong_border(light ? solarized_light::base02 : solarized_light::base2)
    , emphasized_content(
          light ? solarized_light::base01 : solarized_light::base1)
    , primary_content(light ? solarized_light::base00 : solarized_light::base0)
    , not_used(light ? solarized_light::base0 : solarized_light::base00)
    , comments(light ? solarized_light::base1 : solarized_light::base01)
    , background_highlight(
          light ? solarized_light::base2 : solarized_light::base02)
    , background(light ? solarized_light::base3 : solarized_light::base03)
    , yellow(solarized_light::yellow)
    , orange(solarized_light::orange)
    , red(solarized_light::red)
    , magenta(solarized_light::magenta)
    , violet(solarized_light::violet)
    , blue(solarized_light::blue)
    , cyan(solarized_light::cyan)
    , green(solarized_light::green)
{
}


void
SetupSolarized(bool light, ImGuiStyle* dst)
{
  const Solarized s{light};

  ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();

  auto C = [](Color color, float a = 1.0f) -> ImVec4 {
    const int r = (color >> 24) & 0xff;
    const int g = (color >> 16) & 0xff;
    const int b = (color >> 8) & 0xff;
    return ImVec4{r / 255.0f, g / 255.0f, b / 255.0f, a};
  };

  ImVec4* colors = style->Colors;

  colors[ImGuiCol_Text]                  = C(s.primary_content);
  colors[ImGuiCol_TextDisabled]          = C(s.comments);
  colors[ImGuiCol_WindowBg]              = C(s.background);
  colors[ImGuiCol_ChildWindowBg]         = C(s.background);
  colors[ImGuiCol_PopupBg]               = C(s.background);
  colors[ImGuiCol_Border]                = C(s.primary_content);
  colors[ImGuiCol_BorderShadow]          = C(s.background, 0);
  colors[ImGuiCol_FrameBg]               = C(s.background_highlight);
  colors[ImGuiCol_FrameBgHovered]        = C(s.background);
  colors[ImGuiCol_FrameBgActive]         = C(s.background);
  colors[ImGuiCol_TitleBg]               = C(s.background);
  colors[ImGuiCol_TitleBgActive]         = C(s.background_highlight);
  colors[ImGuiCol_TitleBgCollapsed]      = C(s.background);
  colors[ImGuiCol_MenuBarBg]             = C(s.background);
  colors[ImGuiCol_ScrollbarBg]           = C(s.background);
  colors[ImGuiCol_ScrollbarGrab]         = C(s.comments);
  colors[ImGuiCol_ScrollbarGrabHovered]  = C(s.blue);
  colors[ImGuiCol_ScrollbarGrabActive]   = C(s.primary_content);
  colors[ImGuiCol_CheckMark]             = C(s.primary_content);
  colors[ImGuiCol_SliderGrab]            = C(s.primary_content, 0.5f);
  colors[ImGuiCol_SliderGrabActive]      = C(s.primary_content, 0.3f);
  colors[ImGuiCol_Button]                = C(s.background);
  colors[ImGuiCol_ButtonHovered]         = C(s.background_highlight);
  colors[ImGuiCol_ButtonActive]          = C(s.background_highlight);
  colors[ImGuiCol_Header]                = C(s.background_highlight);
  colors[ImGuiCol_HeaderHovered]         = C(s.background_highlight);
  colors[ImGuiCol_HeaderActive]          = C(s.background_highlight);
  colors[ImGuiCol_Separator]             = C(s.comments);
  colors[ImGuiCol_SeparatorHovered]      = C(s.background_highlight);
  colors[ImGuiCol_SeparatorActive]       = C(s.primary_content);
  colors[ImGuiCol_ResizeGrip]            = C(s.comments);
  colors[ImGuiCol_ResizeGripHovered]     = C(s.blue);
  colors[ImGuiCol_ResizeGripActive]      = C(s.primary_content);
  colors[ImGuiCol_PlotLines]             = C(s.comments);
  colors[ImGuiCol_PlotLinesHovered]      = C(s.primary_content);
  colors[ImGuiCol_PlotHistogram]         = C(s.yellow, 0.9f);
  colors[ImGuiCol_PlotHistogramHovered]  = C(s.yellow);
  colors[ImGuiCol_TextSelectedBg]        = C(s.background_highlight);
  colors[ImGuiCol_DragDropTarget]        = C(s.blue);
  colors[ImGuiCol_NavHighlight]          = C(s.red);
  colors[ImGuiCol_NavWindowingHighlight] = C(s.background);
  colors[ImGuiCol_NavWindowingDimBg]     = C(s.background, 0.6f);
  colors[ImGuiCol_ModalWindowDimBg]      = C(s.background, 0.6f);
}
