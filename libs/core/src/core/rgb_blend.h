#pragma once

#include "core/rgb.h"

namespace euphoria::core
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
    rgb   blend(const rgb&   top, const rgb&   bottom, BlendMode mode = BlendMode::normal);
    rgba  blend(const rgba&  top, const rgba&  bottom, BlendMode mode = BlendMode::normal);
    rgbi  blend(const rgbi&  top, const rgbi&  bottom, BlendMode mode = BlendMode::normal);
    rgbai blend(const rgbai& top, const rgbai& bottom, BlendMode mode = BlendMode::normal);

    // rgb   Blend(const rgb&   a, const rgb&   b, float alpha, const BlendMode mode = BlendMode::Normal);
    // rgba  Blend(const rgba&  a, const rgba&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // rgbi  Blend(const rgbi&  a, const rgbi&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbdai Blend(const rgbai& a, const rgbai& b, float alpha, const BlendMode mode = BlendMode::Normal);
}
