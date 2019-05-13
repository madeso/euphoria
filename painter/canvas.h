#ifndef EUPHORIA_CANVAS_H
#define EUPHORIA_CANVAS_H

#include "core/canvaslogic.h"

#include <imgui/imgui.h>

struct CanvasConfig
{
  float zoom_speed       = 10;
  float grid_size        = 64.0f;
  ImU32 grid_color       = IM_COL32(200, 200, 200, 40);
  ImU32 background_color = IM_COL32(60, 60, 70, 200);
};

struct Canvas
{
  CanvasLogic view;
  ImVec2      position = ImVec2{0, 0};

  void
  ShowGrid(const CanvasConfig& cc);

  void
  VerticalLine(float rx, ImU32 grid_color);

  void
  HorizontalLine(float ry, ImU32 grid_color);

  void
  ShowRuler(float ruler_interval, ImU32 ruler_color, float length);

  void
  ShowRuler(const CanvasConfig& cc);

  void
  Begin(const CanvasConfig& cc);

  void
  End(const CanvasConfig& cc);

  ImVec2
  WorldToScreen(const ImVec2& v) const;

  ImVec2
  WorldToScreenSize(const ImVec2& v) const;

  ImVec2
  ScreenToWorld(const ImVec2& v) const;

  ImVec2
  GetMouse() const;
};


#endif  // EUPHORIA_CANVAS_H
