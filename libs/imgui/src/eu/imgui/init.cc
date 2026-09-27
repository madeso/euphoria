#include "eu/imgui/init.h"

#include <SDL_video.h>
#include "imgui.h"

namespace eu::imgui
{

float calculate_app_scale()
{
    return SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
}

std::pair<int, int> rescale_window(int width, int height, float app_scale)
{
    const auto new_width = static_cast<int>(static_cast<float>(width) * app_scale);
    const auto new_height = static_cast<int>(static_cast<float>(height) * app_scale);
    return { new_width, new_height };
}

void setup_scale(float app_scale)
{
    // scale dear imgui: https://wiki.libsdl.org/SDL3/README-highdpi
    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(app_scale);
    style.FontScaleDpi *= app_scale;
}


}
