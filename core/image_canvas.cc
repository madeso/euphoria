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
    Canvas::C(const vec2f v) const
    {
        const auto vv = transform * vec3f {v, 1};
        return vec2f {vv.x, vv.y};
    }

    Canvas::Canvas(Image* i)
        : fillStyle(Color::Black)
        , image(i)
        , transform(mat3f::Identity())
        , building_path(false)
    {
    }

    void
    Canvas::fillRect(int x, int y, int w, int h)
    {
        ASSERTX(w > 0, w);
        ASSERTX(h > 0, h);
        LOG_INFO("fillRect {} {}: {} x {}", x, y, w, h);
        DrawRect(image, fillStyle, Recti::FromTopLeftWidthHeight(x, y, w, h));
    }

    void
    Canvas::translate(float x, float y)
    {
        LOG_INFO("translate {} {}", x, y);
        const auto m = mat3f::FromTranslation2d(vec2f {x, y});
        transform = transform * m;
    }

    void
    Canvas::rotate(float r)
    {
        LOG_INFO("rotate {}", r);
        // transform = transform * mat2f::FromRotation(Angle::FromDegrees(r));
    }

    void
    Canvas::beginPath()
    {
        // LOG_INFO("beginPath");
        ASSERT(!building_path);
        path.resize(0);
        building_path = true;
    }

    void
    Canvas::closePath()
    {
        // LOG_INFO("closePath");
        ASSERT(building_path);
        building_path = false;
    }

    void
    Canvas::moveTo(float x, float y)
    {
        LOG_INFO("moveTo {} {}", x, y);
        ASSERT(building_path);
        ASSERT(path.empty());
        path.push_back(C(vec2f(x, y)));
    }

    void
    Canvas::lineTo(float dx, float dy)
    {
        LOG_INFO("lineTo {} {}", dx, dy);
        ASSERT(building_path);
        if(path.empty())
        {
            path.push_back(C(vec2f::Zero()));
        }
        const auto last = *path.rbegin();
        path.push_back(C(vec2f(last.x + dx, last.y + dy)));
    }

    void
    Canvas::fill()
    {
        LOG_INFO("fill");
        LOG_INFO("Path is {}", StringMerger::Array().Generate(VectorToStringVector(path)));
        ASSERT(!building_path);
        FillPoly(image, fillStyle, path);
    }
}

