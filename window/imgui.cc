#include "window/imgui.h"

#include "core/angle.h"
#include "core/assert.h"

#include <imgui/imgui.h>

void
ImguiAngleSlider(const char* const name, Angle* angle, float mindeg,
                 float maxdeg)
{
  ASSERT(angle);

  float degrees = angle->InDegrees();
  ImGui::SliderFloat(name, &degrees, mindeg, maxdeg);
  *angle = Angle::FromDegrees(degrees);
}
