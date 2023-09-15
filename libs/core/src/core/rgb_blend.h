#pragma once

#include "core/rgb.h"

namespace eu::core
{
    enum class BlendMode
    {
        normal,
        lighten,
        darken,
        multiply,
        average,
        add,
        subtract,
        difference,
        negation,
        screen,
        exclusion,
        overlay,
        soft_light,
        hard_light,
        color_dodge,
        color_burn,
        linear_dodge,
        linear_burn,
        linear_light,
        vivid_light,
        pin_light,
        hard_mix,
        reflect,
        glow,
        phoenix
    };

    // place top onto bottom
    Rgb   blend(const Rgb&   top, const Rgb&   bottom, BlendMode mode = BlendMode::normal);
    Rgba  blend(const Rgba&  top, const Rgba&  bottom, BlendMode mode = BlendMode::normal);
    Rgbi  blend(const Rgbi&  top, const Rgbi&  bottom, BlendMode mode = BlendMode::normal);
    Rgbai blend(const Rgbai& top, const Rgbai& bottom, BlendMode mode = BlendMode::normal);

    // rgb   Blend(const Rgb&   a, const Rgb&   b, float alpha, const BlendMode mode = BlendMode::Normal);
    // rgba  Blend(const Rgba&  a, const Rgba&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // rgbi  Blend(const Rgbi&  a, const Rgbi&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbdai Blend(const Rgbai& a, const Rgbai& b, float alpha, const BlendMode mode = BlendMode::Normal);
}
