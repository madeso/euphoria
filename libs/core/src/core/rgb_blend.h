#ifndef EUPHORIA_CORE_RGB_BLEND_H
#define EUPHORIA_CORE_RGB_BLEND_H

#include "core/rgb.h"

namespace euphoria::core
{
    enum class blend_mode
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
    rgb   blend(const rgb&   top, const rgb&   bottom, blend_mode mode = blend_mode::normal);
    rgba  blend(const rgba&  top, const rgba&  bottom, blend_mode mode = blend_mode::normal);
    rgbi  blend(const rgbi&  top, const rgbi&  bottom, blend_mode mode = blend_mode::normal);
    rgbai blend(const rgbai& top, const rgbai& bottom, blend_mode mode = blend_mode::normal);

    // Rgb   Blend(const Rgb&   a, const Rgb&   b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgba  Blend(const Rgba&  a, const Rgba&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbi  Blend(const Rgbi&  a, const Rgbi&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbdai Blend(const Rgbai& a, const Rgbai& b, float alpha, const BlendMode mode = BlendMode::Normal);
}

#endif // EUPHORIA_CORE_RGB_BLEND_H
