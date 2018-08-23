#ifndef MUSIC_IMGUI_EXTRA_H
#define MUSIC_IMGUI_EXTRA_H

namespace imgui
{
  // source: https://github.com/ocornut/imgui/issues/942
  bool
  Knob(const char* label, float* p_value, float v_min, float v_max);
}

#endif  // MUSIC_IMGUI_EXTRA_H
