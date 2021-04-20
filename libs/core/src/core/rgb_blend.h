#ifndef EUPHORIA_CORE_RGB_BLEND_H
#define EUPHORIA_CORE_RGB_BLEND_H

#include "core/rgb.h"

namespace euphoria::core
{
    enum class BlendMode
    {
        Normal,
        Lighten,
        Darken,
        Multiply,
        Average,
        Add,
        Subtract,
        Difference,
        Negation,
        Screen,
        Exclusion,
        Overlay,
        SoftLight,
        HardLight,
        ColorDodge,
        ColorBurn,
        LinearDodge,
        LinearBurn,
        LinearLight,
        VividLight,
        PinLight,
        HardMix,
        Reflect,
        Glow,
        Phoenix
    };

    // place top onto bottom
    rgb   Blend(const rgb&   top, const rgb&   bottom, BlendMode mode = BlendMode::Normal);
    rgba  Blend(const rgba&  top, const rgba&  bottom, BlendMode mode = BlendMode::Normal);
    rgbi  Blend(const rgbi&  top, const rgbi&  bottom, BlendMode mode = BlendMode::Normal);
    rgbai Blend(const rgbai& top, const rgbai& bottom, BlendMode mode = BlendMode::Normal);

    // Rgb   Blend(const Rgb&   a, const Rgb&   b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgba  Blend(const Rgba&  a, const Rgba&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbi  Blend(const Rgbi&  a, const Rgbi&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbai Blend(const Rgbai& a, const Rgbai& b, float alpha, const BlendMode mode = BlendMode::Normal);
}

#endif  // EUPHORIA_CORE_RGB_BLEND_H
