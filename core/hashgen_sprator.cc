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

    Rgbi
    BorderColor(Rgbi base)
    {
        auto h = hsl(rgb(base));
        h.h -= Angle::FromDegrees(15);
        h.l *= 0.4;
        return rgbi(rgb(h));
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
        int half_width = 4,
        int height = 8
    )
    {
        // todo(Gustav): figure out color (randomly?)
        const auto foreground_color = Color::White;
        const auto background_color = Color::Black;
        const auto border_color = BorderColor(foreground_color);
        const int number_of_steps = 1;

        auto half_side = BoolTable::FromWidthHeight(half_width, height);

        // randomize world
        {
            auto generator = TGenerator{code};
            SetWhiteNoise
            (
                &half_side,
                Fourway<BorderSetupRule>{BorderSetupRule::Random},
                [&]() -> bool { return generator.Next() < 0.5f; }
            );
        }

        // apply sprator algorithm
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
                &half_side,
                Fourway<OutsideRule>{OutsideRule::Empty}
            );

            while(cell.HasMoreWork()) { cell.Work(); }
        }

        // flip and copy from small table to big table
        const auto width = half_width * 2;
        auto result_table = BoolTable::FromWidthHeight(width+2, height+2, false);

        for(int y=0; y<height; y+=1)
        for(int x=0; x<half_width; x+=1)
        {
            result_table(x+1,y+1) = half_side(x, y);
            result_table(width-(x+1)+1,y+1) = half_side(x,y);
        }

        // draw image with border
        Clear(image, background_color);
        auto calculate_scale = [](int image_scale, int table_scale) -> int
        {
            const auto image_scale_float = static_cast<float>(image_scale);
            const auto table_scale_float = static_cast<float>(table_scale);
            const auto scale_factor = image_scale_float / table_scale_float;
            return std::max(1, Floori(scale_factor));
        };

        auto scale = std::min
        (
            calculate_scale(image->GetWidth(), result_table.GetWidth()),
            calculate_scale(image->GetHeight(), result_table.GetHeight())
        );
        const auto border = BorderSettings{border_color};
        auto img = Draw
        (
            result_table,
            foreground_color,
            background_color,
            scale,
            border
        );
        PasteImage(image, vec2i{0, 0}, img);
    }
}


namespace euphoria::core
{
    void
    RenderSprator(Image* image, int code)
    {
        RenderSpratorImpl<xorshift32>(image, Cbit_signed_to_unsigned(code));
    }
}

