#pragma once

#include "open-color.h"
#include "imgui/imgui.h"

namespace euphoria::window
{
    using Colors = std::array<ImU32, 10>;

    constexpr ImU32 oc_to_dear_imgui_color(open_color::Hex h)
    {
        constexpr auto b_shift = 0;
        constexpr auto g_shift = 8;
        constexpr auto r_shift = 16;
        constexpr auto mask = 0xFF;

        return IM_COL32
        (
            (h >> r_shift) & mask,
            (h >> g_shift) & mask,
            (h >> b_shift) & mask,
            255
        );
    }

    constexpr Colors oc_to_dear_imgui_color(open_color::Hexs h)
    {
        return
        {
            oc_to_dear_imgui_color(h[0]), oc_to_dear_imgui_color(h[1]),
            oc_to_dear_imgui_color(h[2]), oc_to_dear_imgui_color(h[3]),
            oc_to_dear_imgui_color(h[4]), oc_to_dear_imgui_color(h[5]),
            oc_to_dear_imgui_color(h[6]), oc_to_dear_imgui_color(h[7]),
            oc_to_dear_imgui_color(h[8]), oc_to_dear_imgui_color(h[9])
        };
    }

    constexpr ImU32  oc_white  = oc_to_dear_imgui_color(open_color::white   );
    constexpr ImU32  oc_black  = oc_to_dear_imgui_color(open_color::black   );
    constexpr Colors oc_gray   = oc_to_dear_imgui_color(open_color::gray   );
    constexpr Colors oc_red    = oc_to_dear_imgui_color(open_color::red    );
    constexpr Colors oc_pink   = oc_to_dear_imgui_color(open_color::pink   );
    constexpr Colors oc_grape  = oc_to_dear_imgui_color(open_color::grape  );
    constexpr Colors oc_violet = oc_to_dear_imgui_color(open_color::violet );
    constexpr Colors oc_indigo = oc_to_dear_imgui_color(open_color::indigo );
    constexpr Colors oc_blue   = oc_to_dear_imgui_color(open_color::blue   );
    constexpr Colors oc_cyan   = oc_to_dear_imgui_color(open_color::cyan   );
    constexpr Colors oc_teal   = oc_to_dear_imgui_color(open_color::teal   );
    constexpr Colors oc_green  = oc_to_dear_imgui_color(open_color::green  );
    constexpr Colors oc_lime   = oc_to_dear_imgui_color(open_color::lime   );
    constexpr Colors oc_yellow = oc_to_dear_imgui_color(open_color::yellow );
    constexpr Colors oc_orange = oc_to_dear_imgui_color(open_color::orange );
}
