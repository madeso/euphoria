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


        uint8 c_u8(int i)
        {
            if(i < 0) { return 0; }
            else if(i > 255) { return 255; }
            else { return static_cast<uint8>(i); }
        }


        uint8 c_u8(float f)
        {
            const auto i = c_float_to_int(f);
            return c_u8(i);
        }


        uint8 channel_blend_normal(uint8 top, uint8)
        {
            return top;
        }


        uint8 channel_blend_lighten(uint8 top, uint8 bottom)
        {
            return (bottom > top) ? bottom : top;
        }


        uint8 channel_blend_darken(uint8 top, uint8 bottom)
        {
            return (bottom > top) ? top : bottom;
        }


        uint8 channel_blend_multiply(uint8 top, uint8 bottom)
        {
            return c_u8(static_cast<float>(static_cast<int>(top) * static_cast<int>(bottom)) / 255.0f);
        }


        uint8 channel_blend_average(uint8 top, uint8 bottom)
        {
            return c_u8(static_cast<float>(static_cast<int>(top) + static_cast<int>(bottom)) / 2.0f);
        }


        uint8 channel_blend_add(uint8 top, uint8 bottom)
        {
            const auto sum = static_cast<int>(top) + static_cast<int>(bottom);
            return c_u8(sum);
        }


        uint8 channel_blend_subtract(uint8 top, uint8 bottom)
        {
            return c_u8(static_cast<int>(top) + static_cast<int>(bottom) - 255);
        }


        uint8 channel_blend_difference(uint8 top, uint8 bottom)
        {
            return c_u8(static_cast<int>(top) - static_cast<int>(bottom));
        }


        uint8 channel_blend_negation(uint8 top, uint8 bottom)
        {
            return 255 - abs(255 - top - bottom);
        }


        uint8 channel_blend_screen(uint8 top, uint8 bottom)
        {
            return 255 - (((255 - top) * (255 - bottom)) >> 8);
        }


        uint8 channel_blend_exclusion(uint8 top, uint8 bottom)
        {
            return top + bottom - 2 * top * bottom / 255;
        }


        uint8 channel_blend_overlay(uint8 top, uint8 bottom)
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


        uint8 channel_blend_soft_light(uint8 top, uint8 bottom)
        {
            const int magic = (top>>1)+64;

            if (bottom < 128)
            {
                const auto r = 2.0f * c_int_to_float(magic) * static_cast<float>(bottom) / 255.0f;
                return c_u8(r);
            }
            else
            {
                const auto r = 2.0f * (255.0f-c_int_to_float(magic)) * static_cast<float>(255-bottom) / 255.0f;
                return c_u8(255.0f - r);
            }
        }


        uint8 channel_blend_hard_light(uint8 top, uint8 bottom)
        {
            return channel_blend_overlay(bottom, top);
        }


        uint8 channel_blend_color_dodge(uint8 top, uint8 bottom)
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


        uint8 channel_blend_color_burn(uint8 top, uint8 bottom)
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


        uint8 channel_blend_linear_dodge(uint8 top, uint8 bottom)
        {
            return channel_blend_add(top, bottom);
        }


        uint8 channel_blend_linear_burn(uint8 top, uint8 bottom)
        {
            return channel_blend_subtract(top, bottom);
        }


        uint8 channel_blend_linear_light(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return channel_blend_linear_burn(top, (2 * bottom));
            }
            else
            {
                return channel_blend_linear_dodge(top, (2 * (bottom - 128)));
            }
        }


        uint8 channel_blend_vivid_light(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return channel_blend_color_burn(top, (2 * bottom));
            }
            else
            {
                return channel_blend_color_dodge(top, (2 * (bottom - 128)));
            }
        }


        uint8 channel_blend_pin_light(uint8 top, uint8 bottom)
        {
            if(bottom < 128)
            {
                return channel_blend_darken(top, (2 * bottom));
            }
            else
            {
                return channel_blend_lighten(top, (2 * (bottom - 128)));
            }
        }


        uint8 channel_blend_hard_mix(uint8 top, uint8 bottom)
        {
            if(channel_blend_vivid_light(top, bottom) < 128)
            {
                return 0;
            }
            else
            {
                return 255;
            }
        }


        uint8 channel_blend_reflect(uint8 top, uint8 bottom)
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


        uint8 channel_blend_glow(uint8 top, uint8 bottom)
        {
            return channel_blend_reflect(bottom, top);
        }


        uint8 channel_blend_phoenix(uint8 top, uint8 bottom)
        {
            return min(top,bottom) - max(top,bottom) + 255;
        }



        // 1 = top
        // 0 = bottom
        uint8
        channel_blend_alpha(uint8 top, uint8 bottom, float factor)
        {
            return c_u8(factor * static_cast<float>(top) + (1.0f - factor) * static_cast<float>(bottom));
        }


        template<typename TF>
        uint8
        channel_blend_alpha_f(uint8 top, uint8 bottom, float factor, TF f)
        {
            return channel_blend_alpha(f(top, bottom), bottom, factor);
        }


        template<typename TBlendFunction>
        Rgbi
        blend(const Rgbi& top, const Rgbi& bottom, TBlendFunction blend_function)
        {
            return
            {
                blend_function(top.r, bottom.r),
                blend_function(top.g, bottom.g),
                blend_function(top.b, bottom.b),
            };
        }


        template<typename TBlendFunction>
        Rgb
        blend(const Rgb& top, const Rgb& bottom, TBlendFunction blend_function)
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
        Rgbai
        blend
        (
            const Rgbai& top,
            const Rgbai& bottom,
            TBlendFunction blend_function
        )
        {
            const auto factor = static_cast<float>(top.a)/255.0f;
            const auto ff = [&](uint8 a_top, uint8 a_bottom)
            {
                return channel_blend_alpha_f
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
        Rgba
        blend(const Rgba& top, const Rgba& bottom, TBlendFunction blend_function)
        {
            return crgba(blend(crgbai(top), crgbai(bottom), blend_function));
        }


        template<typename C>
        C
        color_blend(BlendMode mode, const C& top, const C& bottom)
        {
            switch(mode)
            {
                case BlendMode::normal      : return blend(top, bottom, channel_blend_normal      );
                case BlendMode::lighten     : return blend(top, bottom, channel_blend_lighten     );
                case BlendMode::darken      : return blend(top, bottom, channel_blend_darken      );
                case BlendMode::multiply    : return blend(top, bottom, channel_blend_multiply    );
                case BlendMode::average     : return blend(top, bottom, channel_blend_average     );
                case BlendMode::add         : return blend(top, bottom, channel_blend_add         );
                case BlendMode::subtract    : return blend(top, bottom, channel_blend_subtract    );
                case BlendMode::difference  : return blend(top, bottom, channel_blend_difference  );
                case BlendMode::negation    : return blend(top, bottom, channel_blend_negation    );
                case BlendMode::screen      : return blend(top, bottom, channel_blend_screen      );
                case BlendMode::exclusion   : return blend(top, bottom, channel_blend_exclusion   );
                case BlendMode::overlay     : return blend(top, bottom, channel_blend_overlay     );
                case BlendMode::soft_light  : return blend(top, bottom, channel_blend_soft_light  );
                case BlendMode::hard_light  : return blend(top, bottom, channel_blend_hard_light  );
                case BlendMode::color_dodge : return blend(top, bottom, channel_blend_color_dodge );
                case BlendMode::color_burn  : return blend(top, bottom, channel_blend_color_burn  );
                case BlendMode::linear_dodge: return blend(top, bottom, channel_blend_linear_dodge);
                case BlendMode::linear_burn : return blend(top, bottom, channel_blend_linear_burn );
                case BlendMode::linear_light: return blend(top, bottom, channel_blend_linear_light);
                case BlendMode::vivid_light : return blend(top, bottom, channel_blend_vivid_light );
                case BlendMode::pin_light   : return blend(top, bottom, channel_blend_pin_light   );
                case BlendMode::hard_mix    : return blend(top, bottom, channel_blend_hard_mix    );
                case BlendMode::reflect     : return blend(top, bottom, channel_blend_reflect     );
                case BlendMode::glow        : return blend(top, bottom, channel_blend_glow         );
                case BlendMode::phoenix     : return blend(top, bottom, channel_blend_phoenix     );
                default:
                    DIE("unhandled blend case");
                    return top;
            }
        }
    }


    Rgb   blend(const Rgb&   top, const Rgb&   bottom, BlendMode mode) { return color_blend(mode, top, bottom); }
    Rgba  blend(const Rgba&  top, const Rgba&  bottom, BlendMode mode) { return color_blend(mode, top, bottom); }
    Rgbi  blend(const Rgbi&  top, const Rgbi&  bottom, BlendMode mode) { return color_blend(mode, top, bottom); }
    Rgbai blend(const Rgbai& top, const Rgbai& bottom, BlendMode mode) { return color_blend(mode, top, bottom); }
}
