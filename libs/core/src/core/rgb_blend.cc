#include "core/rgb_blend.h"

#include <cstdint>

#include "core/assert.h"
#include "core/cint.h"

// reference: https://stackoverflow.com/questions/5919663/how-does-photoshop-blend-two-images-together

namespace euphoria::core
{
    namespace
    {
        // todo(Gustav): replace uint8 with float
        using uint8 = std::uint8_t;


        uint8
        min(uint8 lhs, uint8 rhs)
        {
            return (lhs < rhs) ? lhs : rhs;
        }


        uint8
        max(uint8 lhs, uint8 rhs)
        {
            return (lhs > rhs) ? lhs : rhs;
        }


        uint8
        abs(uint8 lhs)
        {
            return lhs;
        }


        uint8 C(int i)
        {
            if(i < 0) { return 0; }
            else if(i > 255) { return 255; }
            else { return static_cast<uint8>(i); }
        }


        uint8 C(float f)
        {
            const auto i = Cfloat_to_int(f);
            return C(i);
        }


        uint8 ChannelBlend_Normal(uint8 top, uint8)
        {
            return top;
        }


        uint8 ChannelBlend_Lighten(uint8 top, uint8 bottom)
        {
            return (bottom > top) ? bottom : top;
        }


        uint8 ChannelBlend_Darken(uint8 top, uint8 bottom)
        {
            return (bottom > top) ? top : bottom;
        }


        uint8 ChannelBlend_Multiply(uint8 top, uint8 bottom)
        {
            return C(static_cast<float>(static_cast<int>(top) * static_cast<int>(bottom)) / 255.0f);
        }


        uint8 ChannelBlend_Average(uint8 top, uint8 bottom)
        {
            return C(static_cast<float>(static_cast<int>(top) + static_cast<int>(bottom)) / 2.0f);
        }


        uint8 ChannelBlend_Add(uint8 top, uint8 bottom)
        {
            const auto sum = static_cast<int>(top) + static_cast<int>(bottom);
            return C(sum);
        }


        uint8 ChannelBlend_Subtract(uint8 top, uint8 bottom)
        {
            return C(static_cast<int>(top) + static_cast<int>(bottom) - 255);
        }


        uint8 ChannelBlend_Difference(uint8 top, uint8 bottom)
        {
            return C(static_cast<int>(top) - static_cast<int>(bottom));
        }


        uint8 ChannelBlend_Negation(uint8 top, uint8 bottom)
        {
            return 255 - abs(255 - top - bottom);
        }


        uint8 ChannelBlend_Screen(uint8 top, uint8 bottom)
        {
            return 255 - (((255 - top) * (255 - bottom)) >> 8);
        }


        uint8 ChannelBlend_Exclusion(uint8 top, uint8 bottom)
        {
            return top + bottom - 2 * top * bottom / 255;
        }


        uint8 ChannelBlend_Overlay(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return 2 * top * bottom / 255;
            }
            else
            {
                return 255 - 2 * (255 - top) * (255 - bottom) / 255;
            }
        }


        uint8 ChannelBlend_SoftLight(uint8 top, uint8 bottom)
        {
            const auto magic = (top>>1)+64;

            if (bottom < 128)
            {
                const auto r = 2.0f * magic * static_cast<float>(bottom) / 255.0f;
                return C(r);
            }
            else
            {
                const auto r = 2.0f * (255.0f-magic) * static_cast<float>(255-bottom) / 255.0f;
                return C(255.0f - r);
            }
        }


        uint8 ChannelBlend_HardLight(uint8 top, uint8 bottom)
        {
            return ChannelBlend_Overlay(bottom,top);
        }


        uint8 ChannelBlend_ColorDodge(uint8 top, uint8 bottom)
        {
            if(bottom == 255)
            {
                return bottom;
            }
            else
            {
                return min(255, ((top << 8 ) / (255 - bottom)));
            }
        }


        uint8 ChannelBlend_ColorBurn(uint8 top, uint8 bottom)
        {
            if(bottom == 0)
            {
                return bottom;
            }
            else
            {
                return max(0, (255 - ((255 - top) << 8 ) / bottom));
            }
        }


        uint8 ChannelBlend_LinearDodge(uint8 top, uint8 bottom)
        {
            return ChannelBlend_Add(top,bottom);
        }


        uint8 ChannelBlend_LinearBurn(uint8 top, uint8 bottom)
        {
            return ChannelBlend_Subtract(top,bottom);
        }


        uint8 ChannelBlend_LinearLight(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return ChannelBlend_LinearBurn(top,(2 * bottom));
            }
            else
            {
                return ChannelBlend_LinearDodge(top,(2 * (bottom - 128)));
            }
        }


        uint8 ChannelBlend_VividLight(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return ChannelBlend_ColorBurn(top,(2 * bottom));
            }
            else
            {
                return ChannelBlend_ColorDodge(top,(2 * (bottom - 128)));
            }
        }


        uint8 ChannelBlend_PinLight(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return ChannelBlend_Darken(top,(2 * bottom));
            }
            else
            {
                return ChannelBlend_Lighten(top,(2 * (bottom - 128)));
            }
        }


        uint8 ChannelBlend_HardMix(uint8 top, uint8 bottom)
        {
            if(ChannelBlend_VividLight(top, bottom) < 128)
            {
                return 0;
            }
            else
            {
                return 255;
            }
        }


        uint8 ChannelBlend_Reflect(uint8 top, uint8 bottom)
        {
            if(bottom == 255)
            {
                return bottom;
            }
            else
            {
                return min(255, (top * top / (255 - bottom)));
            }
        }


        uint8 ChannelBlend_Glow(uint8 top, uint8 bottom)
        {
            return ChannelBlend_Reflect(bottom,top);
        }


        uint8 ChannelBlend_Phoenix(uint8 top, uint8 bottom)
        {
            return min(top,bottom) - max(top,bottom) + 255;
        }



        // 1 = top
        // 0 = bottom
        uint8
        ChannelBlend_Alpha(uint8 top, uint8 bottom, float factor)
        {
            return C(factor * static_cast<float>(top) + (1.0f - factor) * static_cast<float>(bottom));
        }


        template<typename TF>
        uint8
        ChannelBlend_AlphaF(uint8 top, uint8 bottom, float factor, TF F)
        {
            return ChannelBlend_Alpha(F(top,bottom), bottom, factor);
        }


        template<typename TBlendFunction>
        rgbi
        blend(const rgbi& top, const rgbi& bottom, TBlendFunction blend_function)
        {
            return
            {
                blend_function(top.r, bottom.r),
                blend_function(top.g, bottom.g),
                blend_function(top.b, bottom.b),
            };
        }


        template<typename TBlendFunction>
        rgb
        blend(const rgb& top, const rgb& bottom, TBlendFunction blend_function)
        {
            return crgb(blend(crgbi(top), crgbi(bottom), blend_function));
        }


        template<typename T>
        T
        alpha_blend(T lhs, T rhs, float a)
        {
            return static_cast<T>(lhs * (1-a) + rhs * a);
        }


        template<typename TBlendFunction>
        rgbai
        blend
        (
            const rgbai& top,
            const rgbai& bottom,
            TBlendFunction blend_function
        )
        {
            const auto factor = static_cast<float>(top.a)/255.0f;
            const auto ff = [&](uint8 a_top, uint8 a_bottom)
            {
                return ChannelBlend_AlphaF
                (
                    a_top,
                    a_bottom,
                    factor,
                    blend_function
                );
            };
            return
            {
                {
                    ff(top.r, bottom.r),
                    ff(top.g, bottom.g),
                    ff(top.b, bottom.b)
                },
                alpha_blend(top.a, bottom.a, 1-factor)
            };
        }


        template<typename TBlendFunction>
        rgba
        blend(const rgba& top, const rgba& bottom, TBlendFunction blend_function)
        {
            return crgba(blend(crgbai(top), crgbai(bottom), blend_function));
        }


        template<typename C>
        C
        color_blend(blend_mode mode, const C& top, const C& bottom)
        {
            switch(mode)
            {
                case blend_mode::normal      : return blend(top, bottom, ChannelBlend_Normal     );
                case blend_mode::lighten     : return blend(top, bottom, ChannelBlend_Lighten    );
                case blend_mode::darken      : return blend(top, bottom, ChannelBlend_Darken     );
                case blend_mode::multiply    : return blend(top, bottom, ChannelBlend_Multiply   );
                case blend_mode::average     : return blend(top, bottom, ChannelBlend_Average    );
                case blend_mode::add         : return blend(top, bottom, ChannelBlend_Add        );
                case blend_mode::subtract    : return blend(top, bottom, ChannelBlend_Subtract   );
                case blend_mode::difference  : return blend(top, bottom, ChannelBlend_Difference );
                case blend_mode::negation    : return blend(top, bottom, ChannelBlend_Negation   );
                case blend_mode::screen      : return blend(top, bottom, ChannelBlend_Screen     );
                case blend_mode::exclusion   : return blend(top, bottom, ChannelBlend_Exclusion  );
                case blend_mode::overlay     : return blend(top, bottom, ChannelBlend_Overlay    );
                case blend_mode::soft_light  : return blend(top, bottom, ChannelBlend_SoftLight  );
                case blend_mode::hard_light  : return blend(top, bottom, ChannelBlend_HardLight  );
                case blend_mode::color_dodge : return blend(top, bottom, ChannelBlend_ColorDodge );
                case blend_mode::color_burn  : return blend(top, bottom, ChannelBlend_ColorBurn  );
                case blend_mode::linear_dodge: return blend(top, bottom, ChannelBlend_LinearDodge);
                case blend_mode::linear_burn : return blend(top, bottom, ChannelBlend_LinearBurn );
                case blend_mode::linear_light: return blend(top, bottom, ChannelBlend_LinearLight);
                case blend_mode::vivid_light : return blend(top, bottom, ChannelBlend_VividLight );
                case blend_mode::pin_light   : return blend(top, bottom, ChannelBlend_PinLight   );
                case blend_mode::hard_mix    : return blend(top, bottom, ChannelBlend_HardMix    );
                case blend_mode::reflect     : return blend(top, bottom, ChannelBlend_Reflect    );
                case blend_mode::glow        : return blend(top, bottom, ChannelBlend_Glow       );
                case blend_mode::phoenix     : return blend(top, bottom, ChannelBlend_Phoenix    );
                default:
                    DIE("unhandled blend case");
                    return top;
            }
        }
    }


    rgb   blend(const rgb&   top, const rgb&   bottom, blend_mode mode) { return color_blend(mode, top, bottom); }
    rgba  blend(const rgba&  top, const rgba&  bottom, blend_mode mode) { return color_blend(mode, top, bottom); }
    rgbi  blend(const rgbi&  top, const rgbi&  bottom, blend_mode mode) { return color_blend(mode, top, bottom); }
    rgbai blend(const rgbai& top, const rgbai& bottom, blend_mode mode) { return color_blend(mode, top, bottom); }
}
