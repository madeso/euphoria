#include "core/identicon.h"

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

LOG_SPECIFY_DEFAULT_LOGGER("core.identicon")


namespace
{
    using namespace euphoria::core;

    const auto patch0 = std::vector<int>{0, 4, 24, 20 };
    const auto patch1 = std::vector<int>{0, 4, 20 };
    const auto patch2 = std::vector<int>{2, 24, 20 };
    const auto patch3 = std::vector<int>{0, 2,  20, 22 };
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
    const auto patchTypes = std::vector<std::vector<int>>
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
    const auto centerPatchTypes = std::vector<int>{0, 4, 8, 15};

    void render_identicon_patch
    (
        image* image,
        int x,
        int y,
        int size,
        int patch,
        int turn,
        bool invert,
        const rgbi& foreColor,
        const rgbi& backColor
    ) {
        patch %= patchTypes.size();
        turn %= 4;
        if (patch == 15)
        {
            invert = !invert;
        }

        auto vertices = patchTypes[patch];
        auto offset = static_cast<float>(size) / 2.0f;
        auto scale = static_cast<float>(size) / 4.0f;

        auto ctx = canvas{image};

        // paint background
        ctx.fill_style = invert ? foreColor : backColor;
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
        ctx.fill_style = invert ? backColor : foreColor;
        ctx.fill();
    }
}

namespace euphoria::core
{
    void render_identicon(image* image, int code)
    {
        ASSERT(image);
        ASSERT(image->width == image->height);
        auto patchSize = image->height / 3;
        auto middleType = centerPatchTypes[code & 3];
        auto middleInvert = ((code >> 2) & 1) != 0;
        auto cornerType = (code >> 3) & 15;
        auto cornerInvert = ((code >> 7) & 1) != 0;
        auto cornerTurn = (code >> 8) & 3;
        auto sideType = (code >> 10) & 15;
        auto sideInvert = ((code >> 14) & 1) != 0;
        auto sideTurn = (code >> 15) & 3;
        auto blue = (code >> 16) & 31;
        auto green = (code >> 21) & 31;
        auto red = (code >> 27) & 31;

        const auto C = [](int i) { return static_cast<uint8_t>(KeepWithin(Range<int>{0, 255}, i)); };
        auto foreColor = rgbi(C(red << 3), C(green << 3), C(blue << 3));
        auto backColor = rgbi(255, 255, 255);

        // middle patch
        render_identicon_patch(image, patchSize, patchSize, patchSize, middleType, 0, middleInvert, foreColor, backColor);
        // side patchs, starting from top and moving clock-wise
        render_identicon_patch(image, patchSize, 0, patchSize, sideType, sideTurn++, sideInvert, foreColor, backColor);
        render_identicon_patch(image, patchSize * 2, patchSize, patchSize, sideType, sideTurn++, sideInvert, foreColor, backColor);
        render_identicon_patch(image, patchSize, patchSize * 2, patchSize, sideType, sideTurn++, sideInvert, foreColor, backColor);
        render_identicon_patch(image, 0, patchSize, patchSize, sideType, sideTurn++, sideInvert, foreColor, backColor);
        // corner patchs, starting from top left and moving clock-wise
        render_identicon_patch(image, 0, 0, patchSize, cornerType, cornerTurn++, cornerInvert, foreColor, backColor);
        render_identicon_patch(image, patchSize * 2, 0, patchSize, cornerType, cornerTurn++, cornerInvert, foreColor, backColor);
        render_identicon_patch(image, patchSize * 2, patchSize * 2, patchSize, cornerType, cornerTurn++, cornerInvert, foreColor, backColor);
        render_identicon_patch(image, 0, patchSize * 2, patchSize, cornerType, cornerTurn++, cornerInvert, foreColor, backColor);
    }
}
