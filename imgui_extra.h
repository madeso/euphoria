#ifndef MUSIC_IMGUI_EXTRA_H
#define MUSIC_IMGUI_EXTRA_H

namespace imgui
{
  // based on https://github.com/ocornut/imgui/issues/942

  enum KnobStyle
  {
    // interaction style
    KS_UI_DRAG_X = 1 << 1,
    KS_UI_DRAG_Y = 1 << 2,
    KS_UI_AIM    = 1 << 3,

    // purely visible stuffw
    KS_VIS_MARKERS_VISIBLE   = 1 << 5,
    KS_VIS_MAXMIN_VISIBLE    = 1 << 6,
    KS_VIS_OFF_MARKER_HIDDEN = 1 << 7,
    KS_VIS_DRAW_BACKGROUND   = 1 << 8,

    KS_DEFAULT = KS_UI_AIM | KS_VIS_MAXMIN_VISIBLE | KS_VIS_MARKERS_VISIBLE |
                 KS_VIS_OFF_MARKER_HIDDEN | KS_VIS_DRAW_BACKGROUND
  };

  bool
  Knob(
      const char* label,
      float*      p_value,
      float       v_min,
      float       v_max,
      KnobStyle   style = KS_DEFAULT);
}

#endif  // MUSIC_IMGUI_EXTRA_H
