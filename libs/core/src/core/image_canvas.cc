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
    canvas::C(const vec2f& v) const
    {
        const auto vv = transform * vec3f {v, 1};
        return vec2f {vv.x, static_cast<float>(target_image->get_height()) - vv.y};
    }

    canvas::canvas(image* i)
        : fill_style(Color::Black)
        , target_image(i)
        , transform(mat3f::Identity())
        , building_path(false)
    {
    }

    void
    canvas::fill_rect(int x, int y, int w, int h) const
    {
        ASSERTX(w > 0, w);
        ASSERTX(h > 0, h);
        DrawRect(
                target_image,
                fill_style,
                Recti::FromTopLeftWidthHeight(vec2i{x, target_image->get_height() - y}, w, h));
    }

    void
    canvas::translate(float x, float y)
    {
        const auto m = mat3f::FromTranslation2d(vec2f {x, y});
        transform = transform * m;
    }

    void
    canvas::rotate(float r)
    {
        transform = transform * mat3f{mat2f::from_rotation(angle::from_radians(r))};
    }

    void
    canvas::begin_path()
    {
        ASSERT(!building_path);
        path.resize(0);
        building_path = true;
    }

    void
    canvas::close_path()
    {
        ASSERT(building_path);
        building_path = false;
    }

    void
    canvas::move_to(float x, float y)
    {
        ASSERT(building_path);
        ASSERT(path.empty());
        path.push_back(C(vec2f(x, y)));
    }

    void
    canvas::line_to(float dx, float dy)
    {
        ASSERT(building_path);
        if(path.empty())
        {
            path.push_back(C(vec2f::Zero()));
        }
        path.push_back(C(vec2f(dx, dy)));
    }

    void
    canvas::fill() const
    {
        ASSERT(!building_path);
        FillPoly(target_image, fill_style, path);
    }
}

