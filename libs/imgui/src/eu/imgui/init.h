#pragma once

namespace eu::imgui
{
float calculate_app_scale();
std::pair<int, int> rescale_window(int width, int height, float app_scale);
void setup_scale(float app_scale);
}
