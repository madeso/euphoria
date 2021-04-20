#include "core/image_canvas.h"

#include "core/image.h"
#include "core/assert.h"
#include "core/numeric.h"
#include "core/mat2.h"
#include "core/image_draw.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("core.identicon")

namespace euphoria::core
{
    vec2f
    Canvas::C(const vec2f& v) const
    {
        const auto vv = transform * vec3f {v, 1};
        return vec2f {vv.x, static_cast<float>(image->GetHeight()) - vv.y};
    }

    Canvas::Canvas(Image* i)
        : fillStyle(Color::Black)
        , image(i)
        , transform(mat3f::Identity())
        , building_path(false)
    {
    }

    void
    Canvas::fillRect(int x, int y, int w, int h) const
    {
        ASSERTX(w > 0, w);
        ASSERTX(h > 0, h);
        DrawRect(
                image,
                fillStyle,
                Recti::FromTopLeftWidthHeight(vec2i{x, image->GetHeight() - y}, w, h));
    }

    void
    Canvas::translate(float x, float y)
    {
        const auto m = mat3f::FromTranslation2d(vec2f {x, y});
        transform = transform * m;
    }

    void
    Canvas::rotate(float r)
    {
        transform = transform * mat3f{mat2f::FromRotation(angle::from_radians(r))};
    }

    void
    Canvas::beginPath()
    {
        ASSERT(!building_path);
        path.resize(0);
        building_path = true;
    }

    void
    Canvas::closePath()
    {
        ASSERT(building_path);
        building_path = false;
    }

    void
    Canvas::moveTo(float x, float y)
    {
        ASSERT(building_path);
        ASSERT(path.empty());
        path.push_back(C(vec2f(x, y)));
    }

    void
    Canvas::lineTo(float dx, float dy)
    {
        ASSERT(building_path);
        if(path.empty())
        {
            path.push_back(C(vec2f::Zero()));
        }
        path.push_back(C(vec2f(dx, dy)));
    }

    void
    Canvas::fill() const
    {
        ASSERT(!building_path);
        FillPoly(image, fillStyle, path);
    }
}

