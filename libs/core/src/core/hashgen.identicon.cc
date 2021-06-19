#include "core/hashgen.h"

#include <vector>

#include "core/image.h"
#include "core/rgb.h"
#include "core/assert.h"
#include "core/numeric.h"
#include "core/range.h"
#include "core/vec2.h"
#include "core/mat2.h"
#include "core/mat3.h"
#include "core/image_draw.h"
#include "core/image_canvas.h"

#include "core/log.h"


namespace
{
    using namespace euphoria::core;

    const auto patch0 = std::vector<int>{0, 4, 24, 20 };
    const auto patch1 = std::vector<int>{0, 4, 20 };
    const auto patch2 = std::vector<int>{2, 24, 20 };
    const auto patch3 = std::vector<int>{0, 2, 20, 22 };
    const auto patch4 = std::vector<int>{2, 14, 22, 10 };
    const auto patch5 = std::vector<int>{0, 14, 24, 22 };
    const auto patch6 = std::vector<int>{2, 24, 22, 13, 11, 22, 20 };
    const auto patch7 = std::vector<int>{0, 14, 22 };
    const auto patch8 = std::vector<int>{6, 8, 18, 16 };
    const auto patch9 = std::vector<int>{4, 20, 10, 12, 2 };
    const auto patch10 = std::vector<int>{0, 2, 12, 10 };
    const auto patch11 = std::vector<int>{10, 14, 22 };
    const auto patch12 = std::vector<int>{20, 12, 24 };
    const auto patch13 = std::vector<int>{10, 2, 12 };
    const auto patch14 = std::vector<int>{0, 2, 10 };
    const auto patch_types = std::vector<std::vector<int>>
    {
        patch0,
        patch1,
        patch2,
        patch3,
        patch4,
        patch5,
        patch6,
        patch7,
        patch8,
        patch9,
        patch10,
        patch11,
        patch12,
        patch13,
        patch14,
        patch0
    };
    const auto center_patch_types = std::vector<int>{0, 4, 8, 15};

    void render_identicon_patch
    (
        image* image,
        int x,
        int y,
        int size,
        int patch,
        int turn,
        bool invert,
        const rgbi& foreground_color,
        const rgbi& background_color
    ) {
        patch %= patch_types.size();
        turn %= 4;
        if (patch == 15)
        {
            invert = !invert;
        }

        auto vertices = patch_types[patch];
        auto offset = static_cast<float>(size) / 2.0f;
        auto scale = static_cast<float>(size) / 4.0f;

        auto ctx = canvas{image};

        // paint background
        ctx.fill_style = invert ? foreground_color : background_color;
        ctx.fill_rect(x, y, size, size);

        // build patch path
        ctx.translate(static_cast<float>(x) + offset, static_cast<float>(y) + offset);
        ctx.rotate(static_cast<float>(turn) * pi / 2);
        ctx.begin_path();
        ctx.move_to
        (
            static_cast<float>(vertices[0] % 5) * scale - offset,
            std::floor(static_cast<float>(vertices[0]) / 5.0f) * scale - offset
        );
        for (std::size_t i = 1; i < vertices.size(); i++)
        {
            ctx.line_to
            (
                static_cast<float>(vertices[i] % 5) * scale - offset,
                std::floor(static_cast<float>(vertices[i]) / 5.0f) * scale - offset
            );
        }
        ctx.close_path();

        // offset and rotate coordinate space by patch position (x, y) and
        // 'turn' before rendering patch shape

        // render rotated patch using fore color (back color if inverted)
        ctx.fill_style = invert ? background_color : foreground_color;
        ctx.fill();
    }
}

namespace euphoria::core
{
    void render_identicon(image* image, int code)
    {
        ASSERT(image);
        ASSERT(image->width == image->height);
        auto patch_size = image->height / 3;
        auto middle_type = center_patch_types[code & 3];
        auto middle_invert = ((code >> 2) & 1) != 0;
        auto corner_type = (code >> 3) & 15;
        auto corner_invert = ((code >> 7) & 1) != 0;
        auto corner_turn = (code >> 8) & 3;
        auto side_type = (code >> 10) & 15;
        auto side_invert = ((code >> 14) & 1) != 0;
        auto side_turn = (code >> 15) & 3;
        auto blue = (code >> 16) & 31;
        auto green = (code >> 21) & 31;
        auto red = (code >> 27) & 31;

        const auto con = [](int i) { return static_cast<uint8_t>(keep_within(range<int>{0, 255}, i)); };
        auto foreground_color = rgbi(con(red << 3), con(green << 3), con(blue << 3));
        auto background_color = rgbi(255, 255, 255);

        // middle patch
        render_identicon_patch(image, patch_size, patch_size, patch_size, middle_type, 0, middle_invert, foreground_color, background_color);
        // side patchs, starting from top and moving clock-wise
        render_identicon_patch(image, patch_size, 0, patch_size, side_type, side_turn++, side_invert, foreground_color, background_color);
        render_identicon_patch(image, patch_size * 2, patch_size, patch_size, side_type, side_turn++, side_invert, foreground_color, background_color);
        render_identicon_patch(image, patch_size, patch_size * 2, patch_size, side_type, side_turn++, side_invert, foreground_color, background_color);
        render_identicon_patch(image, 0, patch_size, patch_size, side_type, side_turn++, side_invert, foreground_color, background_color);
        // corner patchs, starting from top left and moving clock-wise
        render_identicon_patch(image, 0, 0, patch_size, corner_type, corner_turn++, corner_invert, foreground_color, background_color);
        render_identicon_patch(image, patch_size * 2, 0, patch_size, corner_type, corner_turn++, corner_invert, foreground_color, background_color);
        render_identicon_patch(image, patch_size * 2, patch_size * 2, patch_size, corner_type, corner_turn++, corner_invert, foreground_color, background_color);
        render_identicon_patch(image, 0, patch_size * 2, patch_size, corner_type, corner_turn++, corner_invert, foreground_color, background_color);
    }
}
