#include "core/identicon.h"

#include <vector>

#include "core/image.h"
#include "core/rgb.h"
#include "core/assert.h"
#include "core/numeric.h"
#include "core/range.h"

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

    struct Context
    {
        // todo(Gustav): implement this
        Rgbi fillStyle = Color::White;

        void
        fillRect(int x, int y, int w, int h)
        {
            // LOG_INFO("fillRect " << x << " " << y << " " << w << " " << h);
        }

        void
        translate(int x, int y)
        {
            // LOG_INFO("translate " << x << " " << y);
        }

        void
        rotate(float r)
        {
            // LOG_INFO("rotate " << r);
        }

        void
        beginPath()
        {
            // LOG_INFO("beginPath");
        }

        void
        closePath()
        {
            // LOG_INFO("closePath");
        }

        void
        moveTo(int x, int y)
        {
            // LOG_INFO("moveTo " << x << " " << y);
        }

        void
        lineTo(int x, int y)
        {
            // LOG_INFO("lineTo " << x << " " << y);
        }

        void
        fill()
        {
            // LOG_INFO("fill");
        }
    };

    void render_identicon_patch
    (
        Image* image,
        int x,
        int y,
        int size,
        int patch,
        int turn,
        int invert,
        const Rgbi& foreColor,
        const Rgbi& backColor
    ) {
	    patch %= patchTypes.size();
	    turn %= 4;
	    if (patch == 15)
        {
		    invert = !invert;
        }

	    auto vertices = patchTypes[patch];
	    auto offset = size / 2;
	    auto scale = size / 4;

        auto ctx = Context{};

	    // paint background
	    ctx.fillStyle = invert ? foreColor : backColor;
	    ctx.fillRect(x, y, size, size);

	    // build patch path
	    ctx.translate(x + offset, y + offset);
	    ctx.rotate(turn * Pi() / 2);
	    ctx.beginPath();
	    ctx.moveTo((vertices[0] % 5 * scale - offset), (Floori(vertices[0] / 5.0f) * scale - offset));
	    for (std::size_t i = 1; i < vertices.size(); i++)
        {
		    ctx.lineTo((vertices[i] % 5 * scale - offset), (Floori(vertices[i] / 5.0f) * scale - offset));
        }
	    ctx.closePath();
	
	    // offset and rotate coordinate space by patch position (x, y) and
	    // 'turn' before rendering patch shape

	    // render rotated patch using fore color (back color if inverted)
	    ctx.fillStyle = invert ? backColor : foreColor;
	    ctx.fill();
    }
}

namespace euphoria::core
{
    void RenderIdenticon(Image* image, int code)
    {
        ASSERT(image);
        ASSERT(image->GetWidth() == image->GetHeight());
	    auto patchSize = image->GetHeight() / 3;
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
	    auto foreColor = Rgbi(C(red << 3), C(green << 3), C(blue << 3));
	    auto backColor = Rgbi(255, 255, 255);
	
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
