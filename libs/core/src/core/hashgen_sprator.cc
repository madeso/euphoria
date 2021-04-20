#include "core/hashgen_sprator.h"

#include <vector>

#include "core/image.h"
#include "core/rgb.h"
#include "core/assert.h"
#include "core/rng.h"
#include "core/numeric.h"
#include "core/image_draw.h"
#include "core/cint.h"
#include "core/table_bool.h"
#include "core/generator_cell.h"


namespace
{
    using namespace euphoria::core;


    rgbai
    CalculateBorderColor(rgbai base)
    {
        auto h = chsl(crgb(base));
        h.h -= angle::from_degrees(15);
        h.l *= 0.4;
        return {crgbi(crgb(h)), base.a};
    }


    void
    ApplySpratorAlgorithm(BoolTable* half_side, int number_of_steps)
    {
        generator::Rules rules;
        generator::AddComplexRules
        (
            &rules,
            number_of_steps,
            [](bool current, const Wallcounter& wc) -> std::optional<bool>
            {
                const auto c = wc.Count
                (
                    1,
                    false,
                    NeighborhoodAlgorithm::Plus
                );
                if(current)
                {
                    if(c == 2 || c == 3 ) { return true; }
                    else { return false; }
                }
                else
                {
                    if(c <= 1) { return true; }
                    else { return false; }
                }
            }
        );

        auto cell = generator::CellularAutomata
        (
            &rules,
            half_side,
            Fourway<OutsideRule>{OutsideRule::Empty}
        );

        while(cell.HasMoreWork()) { cell.Work(); }
    }


    BoolTable
    Mirror(const BoolTable& half_side)
    {
        const auto height = half_side.GetHeight();
        const auto half_width = half_side.GetWidth();
        const auto width = half_width * 2;

        // offset everything by 1 to get a nice border
        constexpr auto offset = 1;
        constexpr auto extra_size = 2;

        auto result_table = BoolTable::FromWidthHeight
        (
            width + extra_size,
            height + extra_size,
            false
        );

        for(int y=0; y<height; y+=1)
        {
            for(int x=0; x<half_width; x+=1)
            {
                const auto src = half_side(x, y);
                const auto x_mirror = width - (x + 1);
                result_table(x + offset, y + offset) = src;
                result_table(x_mirror + offset, y + offset) = src;
            }
        }

        return result_table;
    }


    int
    CalculateScale(const Image& image, const BoolTable& table)
    {
        auto calculate_scale = [](int image_scale, int table_scale) -> int
        {
            const auto image_scale_float = static_cast<float>(image_scale);
            const auto table_scale_float = static_cast<float>(table_scale);
            const auto scale_factor = image_scale_float / table_scale_float;
            return std::max(1, Floori(scale_factor));
        };

        const auto scale = std::min
        (
            calculate_scale(image.GetWidth(), table.GetWidth()),
            calculate_scale(image.GetHeight(), table.GetHeight())
        );

        return scale;
    }


    void
    DrawImageWithBorder
    (
        Image* image,
        const BoolTable& result_table,
        const rgbai& background_color,
        const rgbai& foreground_color,
        const rgbai& border_color
    )
    {
        Clear(image, background_color);
        auto img = Draw
        (
            result_table,
            foreground_color,
            background_color,
            CalculateScale(*image, result_table),
            BorderSettings{border_color}
        );
        PasteImage(image, vec2i{0, 0}, img);
    }


    template
    <
        typename TGenerator
    >
    void
    RandomizeWorld(BoolTable* half_side, TGenerator* generator)
    {
        SetWhiteNoise
        (
            half_side,
            Fourway<BorderSetupRule>{BorderSetupRule::Random},
            [&]() -> bool { return generator->Next() < 0.5f; }
        );
    }


    template
    <
        typename TGenerator,
        typename I
    >
    void
    RenderSpratorImpl
    (
        Image* image,
        I code,
        const rgbai& foreground_color,
        const std::optional<rgbai> border_color_arg,
        const rgbai& background_color
    )
    {
        constexpr int half_width = 4;
        constexpr int height = 8;
        const int number_of_steps = 3;
        // todo(Gustav): figure out color (randomly?)
        const rgbai border_color = border_color_arg.value_or
        (
            CalculateBorderColor(foreground_color)
        );

        auto half_side = BoolTable::FromWidthHeight(half_width, height);

        auto generator = TGenerator{code};

        // randomize world
        RandomizeWorld<TGenerator>(&half_side, &generator);

        // apply sprator algorithm
        ApplySpratorAlgorithm(&half_side, number_of_steps);

        // flip and copy from small table to big table
        const auto result_table = Mirror(half_side);

        // draw image with border
        DrawImageWithBorder(image, result_table, background_color, foreground_color, border_color);
    }


    template
    <
        typename TGenerator,
        typename I
    >
    void
    RenderSpratorImpl
    (
        std::vector<Image>* images,
        I code,
        const rgbai& foreground_color,
        const std::optional<rgbai> border_color_arg,
        const rgbai& background_color
    )
    {
        constexpr int half_width = 4;
        constexpr int height = 8;
        const int number_of_steps = 3;
        // todo(Gustav): figure out color (randomly?)
        const rgbai border_color = border_color_arg.value_or
        (
            CalculateBorderColor(foreground_color)
        );

        auto half_side = BoolTable::FromWidthHeight(half_width, height);

        auto generator = TGenerator{code};

        // randomize world
        RandomizeWorld<TGenerator>(&half_side, &generator);

        // apply sprator algorithm
        ApplySpratorAlgorithm(&half_side, number_of_steps - 1);

        bool first = true;

        auto orig_half_side = half_side;

        for(auto& image: *images)
        {
            if(first) { first = false; }
            else
            {
                half_side = orig_half_side;

                // animate...
                auto hit = BoolTable::FromWidthHeight(half_side.GetWidth(), half_side.GetHeight(), false);
                for(int i=0; i<2; i+=1)
                {
                    int x = 0;
                    int y = 0;
                    do
                    {
                        x = Floori(generator.Next() * half_side.GetWidth());
                        y = Floori(generator.Next() * half_side.GetHeight());
                    } while(hit(x, y));
                    hit(x, y) = true;
                    half_side(x, y) = !half_side(x, y);
                }
            }


            ApplySpratorAlgorithm(&half_side, 1);

            // flip and copy from small table to big table
            const auto result_table = Mirror(half_side);

            // draw image with border
            DrawImageWithBorder(&image, result_table, background_color, foreground_color, border_color);
        }
    }
}


namespace euphoria::core
{
    void
    RenderSprator
    (
        Image* image,
        int code,
        const rgbai& foreground_color,
        std::optional<rgbai> border_color_arg,
        const rgbai& background_color
    )
    {
        RenderSpratorImpl<xorshift32>
        (
            image,
            Cbit_signed_to_unsigned(code),
            foreground_color,
            border_color_arg,
            background_color
        );
    }


    void
    RenderSprator
    (
        std::vector<Image>* images,
        int code,
        const rgbai& foreground_color,
        std::optional<rgbai> border_color_arg,
        const rgbai& background_color
    )
    {
        RenderSpratorImpl<xorshift32>
        (
            images,
            Cbit_signed_to_unsigned(code),
            foreground_color,
            border_color_arg,
            background_color
        );
    }
}

