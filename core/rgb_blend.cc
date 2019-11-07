#include "core/rgb_blend.h"

#include <cstdint>

#include "core/assert.h"

// reference: https://stackoverflow.com/questions/5919663/how-does-photoshop-blend-two-images-together

namespace euphoria::core
{
    namespace
    {
        // todo(Gustav): replace uint8 with float
        using uint8 = std::uint8_t;

        uint8 min(uint8 lhs, uint8 rhs)
        {
            return (lhs < rhs) ? lhs : rhs;
        }

        uint8 max(uint8 lhs, uint8 rhs)
        {
            return (lhs > rhs) ? lhs : rhs;
        }

        uint8 abs(uint8 lhs)
        {

            return lhs;
        }

        uint8 ChannelBlend_Normal        (uint8 top, uint8       ) { return ((uint8)(top))                                                                                       ; }
        uint8 ChannelBlend_Lighten       (uint8 top, uint8 bottom) { return ((uint8)((bottom > top) ? bottom:top))                                                                           ; }
        uint8 ChannelBlend_Darken        (uint8 top, uint8 bottom) { return ((uint8)((bottom > top) ? top:bottom))                                                                           ; }
        uint8 ChannelBlend_Multiply      (uint8 top, uint8 bottom) { return ((uint8)((top * bottom) / 255))                                                                           ; }
        uint8 ChannelBlend_Average       (uint8 top, uint8 bottom) { return ((uint8)((top + bottom) / 2))                                                                             ; }
        uint8 ChannelBlend_Add           (uint8 top, uint8 bottom) { return ((uint8)(min(255, (top + bottom))))                                                                       ; }
        uint8 ChannelBlend_Subtract      (uint8 top, uint8 bottom) { return ((uint8)((top + bottom < 255) ? 0:(top + bottom - 255)))                                                         ; }
        uint8 ChannelBlend_Difference    (uint8 top, uint8 bottom) { return ((uint8)(abs(top - bottom)))                                                                              ; }
        uint8 ChannelBlend_Negation      (uint8 top, uint8 bottom) { return ((uint8)(255 - abs(255 - top - bottom)))                                                                  ; }
        uint8 ChannelBlend_Screen        (uint8 top, uint8 bottom) { return ((uint8)(255 - (((255 - top) * (255 - bottom)) >> 8)))                                                    ; }
        uint8 ChannelBlend_Exclusion     (uint8 top, uint8 bottom) { return ((uint8)(top + bottom - 2 * top * bottom / 255))                                                                 ; }
        uint8 ChannelBlend_Overlay       (uint8 top, uint8 bottom) { return ((uint8)((bottom < 128) ? (2 * top * bottom / 255):(255 - 2 * (255 - top) * (255 - bottom) / 255)))                   ; }
        uint8 ChannelBlend_SoftLight     (uint8 top, uint8 bottom) { return ((uint8)((bottom < 128)?(2*((top>>1)+64))*((float)bottom/255):(255-(2*(255-((top>>1)+64))*(float)(255-bottom)/255)))) ; }
        uint8 ChannelBlend_HardLight     (uint8 top, uint8 bottom) { return (ChannelBlend_Overlay(bottom,top))                                                                        ; }
        uint8 ChannelBlend_ColorDodge    (uint8 top, uint8 bottom) { return ((uint8)((bottom == 255) ? bottom:min(255, ((top << 8 ) / (255 - bottom)))))                                        ; }
        uint8 ChannelBlend_ColorBurn     (uint8 top, uint8 bottom) { return ((uint8)((bottom == 0) ? bottom:max(0, (255 - ((255 - top) << 8 ) / bottom))))                                      ; }
        uint8 ChannelBlend_LinearDodge   (uint8 top, uint8 bottom) { return (ChannelBlend_Add(top,bottom))                                                                            ; }
        uint8 ChannelBlend_LinearBurn    (uint8 top, uint8 bottom) { return (ChannelBlend_Subtract(top,bottom))                                                                       ; }
        uint8 ChannelBlend_LinearLight   (uint8 top, uint8 bottom) { return ((uint8)(bottom < 128)?ChannelBlend_LinearBurn(top,(2 * bottom)):ChannelBlend_LinearDodge(top,(2 * (bottom - 128))))  ; }
        uint8 ChannelBlend_VividLight    (uint8 top, uint8 bottom) { return ((uint8)(bottom < 128)?ChannelBlend_ColorBurn(top,(2 * bottom)):ChannelBlend_ColorDodge(top,(2 * (bottom - 128))))    ; }
        uint8 ChannelBlend_PinLight      (uint8 top, uint8 bottom) { return ((uint8)(bottom < 128)?ChannelBlend_Darken(top,(2 * bottom)):ChannelBlend_Lighten(top,(2 * (bottom - 128))))          ; }
        uint8 ChannelBlend_HardMix       (uint8 top, uint8 bottom) { return ((uint8)((ChannelBlend_VividLight(top,bottom) < 128) ? 0:255))                                            ; }
        uint8 ChannelBlend_Reflect       (uint8 top, uint8 bottom) { return ((uint8)((bottom == 255) ? bottom:min(255, (top * top / (255 - bottom)))))                                            ; }
        uint8 ChannelBlend_Glow          (uint8 top, uint8 bottom) { return (ChannelBlend_Reflect(bottom,top))                                                                        ; }
        uint8 ChannelBlend_Phoenix       (uint8 top, uint8 bottom) { return ((uint8)(min(top,bottom) - max(top,bottom) + 255))                                                               ; }


        uint8 ChannelBlend_Alpha(uint8 top,uint8 bottom,float O)     { return ((uint8)(O * top + (1 - O) * bottom)); }

        template<typename TF>
        uint8 ChannelBlend_AlphaF(uint8 top,uint8 bottom,float O, TF F)  { return (ChannelBlend_Alpha(F(top,bottom),bottom,O)); }

        template<typename F>
        Rgbi Blend(const Rgbi& top, const Rgbi& bottom, F f)
        {
            return
            {
                f(top.r, bottom.r),
                f(top.g, bottom.g),
                f(top.b, bottom.b),
            };
        }

        template<typename F>
        Rgb Blend(const Rgb& top, const Rgb& bottom, F f)
        {
            return rgb(Blend(rgbi(top), rgbi(bottom), f));
        }

        template<typename T>
        T AlphaBlend(T lhs, T rhs, float a)
        {
            return static_cast<T>(lhs * (1-a) + rhs * a);
        }

        template<typename F>
        Rgbai Blend(const Rgbai& top, const Rgbai& bottom, F f)
        {
            const auto factor = top.a/255.0f;
            const auto ff = [&](uint8 a_top,uint8 a_bottom){ return ChannelBlend_AlphaF(a_top, a_bottom, factor, f); };
            return
            {
                {
                    ff(top.r, bottom.r),
                    ff(top.g, bottom.g),
                    ff(top.b, bottom.b)
                },
                AlphaBlend(top.a, bottom.a, 1-factor)
            };
        }

        template<typename F>
        Rgba Blend(const Rgba& top, const Rgba& bottom, F f)
        {
            return rgba(Blend(rgbai(top), rgbai(bottom), f));
        }

        template<typename C>
        C
        ColorBlend(BlendMode mode, const C& top, const C& bottom)
        {
            switch(mode)
            {
                case BlendMode::Normal     : return    Blend(top, bottom, ChannelBlend_Normal     );
                case BlendMode::Lighten    : return    Blend(top, bottom, ChannelBlend_Lighten    );
                case BlendMode::Darken     : return    Blend(top, bottom, ChannelBlend_Darken     );
                case BlendMode::Multiply   : return    Blend(top, bottom, ChannelBlend_Multiply   );
                case BlendMode::Average    : return    Blend(top, bottom, ChannelBlend_Average    );
                case BlendMode::Add        : return    Blend(top, bottom, ChannelBlend_Add        );
                case BlendMode::Subtract   : return    Blend(top, bottom, ChannelBlend_Subtract   );
                case BlendMode::Difference : return    Blend(top, bottom, ChannelBlend_Difference );
                case BlendMode::Negation   : return    Blend(top, bottom, ChannelBlend_Negation   );
                case BlendMode::Screen     : return    Blend(top, bottom, ChannelBlend_Screen     );
                case BlendMode::Exclusion  : return    Blend(top, bottom, ChannelBlend_Exclusion  );
                case BlendMode::Overlay    : return    Blend(top, bottom, ChannelBlend_Overlay    );
                case BlendMode::SoftLight  : return    Blend(top, bottom, ChannelBlend_SoftLight  );
                case BlendMode::HardLight  : return    Blend(top, bottom, ChannelBlend_HardLight  );
                case BlendMode::ColorDodge : return    Blend(top, bottom, ChannelBlend_ColorDodge );
                case BlendMode::ColorBurn  : return    Blend(top, bottom, ChannelBlend_ColorBurn  );
                case BlendMode::LinearDodge: return    Blend(top, bottom, ChannelBlend_LinearDodge);
                case BlendMode::LinearBurn : return    Blend(top, bottom, ChannelBlend_LinearBurn );
                case BlendMode::LinearLight: return    Blend(top, bottom, ChannelBlend_LinearLight);
                case BlendMode::VividLight : return    Blend(top, bottom, ChannelBlend_VividLight );
                case BlendMode::PinLight   : return    Blend(top, bottom, ChannelBlend_PinLight   );
                case BlendMode::HardMix    : return    Blend(top, bottom, ChannelBlend_HardMix    );
                case BlendMode::Reflect    : return    Blend(top, bottom, ChannelBlend_Reflect    );
                case BlendMode::Glow       : return    Blend(top, bottom, ChannelBlend_Glow       );
                case BlendMode::Phoenix    : return    Blend(top, bottom, ChannelBlend_Phoenix    );
                default:
                    DIE("unhandled blend case");
                    return top;
            }
        }

        template<typename C>
        C
        ColorBlend(BlendMode mode, const C& top, const C& bottom, float alpha)
        {
            switch(mode)
            {
                case BlendMode::Normal     : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Normal     );});
                case BlendMode::Lighten    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Lighten    );});
                case BlendMode::Darken     : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Darken     );});
                case BlendMode::Multiply   : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Multiply   );});
                case BlendMode::Average    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Average    );});
                case BlendMode::Add        : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Add        );});
                case BlendMode::Subtract   : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Subtract   );});
                case BlendMode::Difference : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Difference );});
                case BlendMode::Negation   : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Negation   );});
                case BlendMode::Screen     : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Screen     );});
                case BlendMode::Exclusion  : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Exclusion  );});
                case BlendMode::Overlay    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Overlay    );});
                case BlendMode::SoftLight  : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_SoftLight  );});
                case BlendMode::HardLight  : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_HardLight  );});
                case BlendMode::ColorDodge : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_ColorDodge );});
                case BlendMode::ColorBurn  : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_ColorBurn  );});
                case BlendMode::LinearDodge: return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_LinearDodge);});
                case BlendMode::LinearBurn : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_LinearBurn );});
                case BlendMode::LinearLight: return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_LinearLight);});
                case BlendMode::VividLight : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_VividLight );});
                case BlendMode::PinLight   : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_PinLight   );});
                case BlendMode::HardMix    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_HardMix    );});
                case BlendMode::Reflect    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Reflect    );});
                case BlendMode::Glow       : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Glow       );});
                case BlendMode::Phoenix    : return    Blend(top, bottom, [alpha](uint8 top, uint8 bottom){return ChannelBlend_AlphaF(top, bottom, alpha, ChannelBlend_Phoenix    );});
                default:
                    DIE("unhandled alpha blend case");
                    return top;
            }
        }
    }

    Rgb   Blend(const Rgb&   top, const Rgb&   bottom, const BlendMode mode) { return ColorBlend(mode, top, bottom); }
    Rgba  Blend(const Rgba&  top, const Rgba&  bottom, const BlendMode mode) { return ColorBlend(mode, top, bottom); }
    Rgbi  Blend(const Rgbi&  top, const Rgbi&  bottom, const BlendMode mode) { return ColorBlend(mode, top, bottom); }
    Rgbai Blend(const Rgbai& top, const Rgbai& bottom, const BlendMode mode) { return ColorBlend(mode, top, bottom); }
}
