#pragma once

#include "open-color.h"
#include "imgui/imgui.h"

namespace eu::window
{
    using Colors = std::array<ImU32, 10>;

    constexpr ImU32 from_oc_to_dear_imgui_color(open_color::Hex h)
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

    constexpr Colors from_oc_to_dear_imgui_color(open_color::Hexs h)
    {
        return
        {
            from_oc_to_dear_imgui_color(h[0]), from_oc_to_dear_imgui_color(h[1]),
            from_oc_to_dear_imgui_color(h[2]), from_oc_to_dear_imgui_color(h[3]),
            from_oc_to_dear_imgui_color(h[4]), from_oc_to_dear_imgui_color(h[5]),
            from_oc_to_dear_imgui_color(h[6]), from_oc_to_dear_imgui_color(h[7]),
            from_oc_to_dear_imgui_color(h[8]), from_oc_to_dear_imgui_color(h[9])
        };
    }

    constexpr ImU32  oc_white  = from_oc_to_dear_imgui_color(open_color::white   );
    constexpr ImU32  oc_black  = from_oc_to_dear_imgui_color(open_color::black   );
    constexpr Colors oc_gray   = from_oc_to_dear_imgui_color(open_color::gray   );
    constexpr Colors oc_red    = from_oc_to_dear_imgui_color(open_color::red    );
    constexpr Colors oc_pink   = from_oc_to_dear_imgui_color(open_color::pink   );
    constexpr Colors oc_grape  = from_oc_to_dear_imgui_color(open_color::grape  );
    constexpr Colors oc_violet = from_oc_to_dear_imgui_color(open_color::violet );
    constexpr Colors oc_indigo = from_oc_to_dear_imgui_color(open_color::indigo );
    constexpr Colors oc_blue   = from_oc_to_dear_imgui_color(open_color::blue   );
    constexpr Colors oc_cyan   = from_oc_to_dear_imgui_color(open_color::cyan   );
    constexpr Colors oc_teal   = from_oc_to_dear_imgui_color(open_color::teal   );
    constexpr Colors oc_green  = from_oc_to_dear_imgui_color(open_color::green  );
    constexpr Colors oc_lime   = from_oc_to_dear_imgui_color(open_color::lime   );
    constexpr Colors oc_yellow = from_oc_to_dear_imgui_color(open_color::yellow );
    constexpr Colors oc_orange = from_oc_to_dear_imgui_color(open_color::orange );
}
