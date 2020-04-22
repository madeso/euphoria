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
        const auto width = half_width * 2;
        // todo(Gustav): figure out color (randomly?)
        const auto foreground_color = Color::White;
        const auto background_color = Color::Black;

        auto table = BoolTable::FromWidthHeight(width, height);

        {
            auto generator = TGenerator{code};
            SetWhiteNoise
            (
                &table,
                Fourway<BorderSetupRule>{BorderSetupRule::Random},
                [&]() -> bool { return generator.Next() < 0.5f; }
            );
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
            sf(image->GetWidth(), table.GetWidth()),
            sf(image->GetHeight(), table.GetHeight())
        );
        auto img = Draw(table, foreground_color, background_color, scale);
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

