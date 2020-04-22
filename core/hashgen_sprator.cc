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

        auto half_side = BoolTable::FromWidthHeight(half_width, height);

        {
            auto generator = TGenerator{code};
            SetWhiteNoise
            (
                &half_side,
                Fourway<BorderSetupRule>{BorderSetupRule::Random},
                [&]() -> bool { return generator.Next() < 0.5f; }
            );
        }

        {
            generator::Rules rules;
            generator::AddComplexRules
            (
                &rules,
                3,
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

            auto cell = generator::CellularAutomata(&rules, &half_side, Fourway<OutsideRule>{OutsideRule::Empty});
            while(cell.HasMoreWork())
            {
                cell.Work();
            }
        }

        // flip
        const auto width = half_width * 2;
        auto res = BoolTable::FromWidthHeight(width, height);

        // copy from small table to big table
        for(int y=0; y<height; y+=1)
        for(int x=0; x<half_width; x+=1)
        {
            res(x,y) = half_side(x, y);
            res(width-(x+1),y) = half_side(x,y);
        }

        Clear(image, background_color);
        auto sf = [](int is, int ts) -> int
        {
            const auto fis = static_cast<float>(is);
            const auto fts = static_cast<float>(ts);
            const auto sf = fis / fts;
            const auto si = std::max(1, static_cast<int>(sf));
            return si;
        };

        auto scale = std::min
        (
            sf(image->GetWidth(), res.GetWidth()),
            sf(image->GetHeight(), res.GetHeight())
        );
        auto img = Draw(res, foreground_color, background_color, scale);
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

