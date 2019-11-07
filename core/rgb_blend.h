#ifndef EUPHORIA_CORE_RGB_BLEND_H
#define EUPHORIA_CORE_RGB_BLEND_H

#include "core/rgb.h"

namespace euphoria::core
{
    enum class BlendMode
    {
        Normal     ,
        Lighten    ,
        Darken     ,
        Multiply   ,
        Average    ,
        Add        ,
        Subtract   ,
        Difference ,
        Negation   ,
        Screen     ,
        Exclusion  ,
        Overlay    ,
        SoftLight  ,
        HardLight  ,
        ColorDodge ,
        ColorBurn  ,
        LinearDodge,
        LinearBurn ,
        LinearLight,
        VividLight ,
        PinLight   ,
        HardMix    ,
        Reflect    ,
        Glow       ,
        Phoenix    
    };

    // place top onto bottom
    Rgb   Blend(const Rgb&   top, const Rgb&   bottom, const BlendMode mode = BlendMode::Normal);
    Rgba  Blend(const Rgba&  top, const Rgba&  bottom, const BlendMode mode = BlendMode::Normal);
    Rgbi  Blend(const Rgbi&  top, const Rgbi&  bottom, const BlendMode mode = BlendMode::Normal);
    Rgbai Blend(const Rgbai& top, const Rgbai& bottom, const BlendMode mode = BlendMode::Normal);

    // Rgb   Blend(const Rgb&   a, const Rgb&   b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgba  Blend(const Rgba&  a, const Rgba&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbi  Blend(const Rgbi&  a, const Rgbi&  b, float alpha, const BlendMode mode = BlendMode::Normal);
    // Rgbai Blend(const Rgbai& a, const Rgbai& b, float alpha, const BlendMode mode = BlendMode::Normal);
}

#endif EUPHORIA_CORE_RGB_BLEND_H
