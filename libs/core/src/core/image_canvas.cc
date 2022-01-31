#include "core/image_canvas.h"

#include "core/image.h"
#include "assert/assert.h"
#include "core/numeric.h"
#include "core/mat2.h"
#include "core/image_draw.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

#include "log/log.h"


namespace euphoria::core
{
    Vec2f
    Canvas::transform_position(const Vec2f& v) const
    {
        const auto vv = transform * Vec3f {v, 1};
        return Vec2f {vv.x, static_cast<float>(target_image->height) - vv.y};
    }

    Canvas::Canvas(Image* i)
        : fill_style(NamedColor::black)
        , target_image(i)
        , transform(Mat3f::identity())
        , building_path(false)
    {
    }

    void
    Canvas::fill_rect(int x, int y, int w, int h) const
    {
        ASSERTX(w > 0, w);
        ASSERTX(h > 0, h);
        draw_rect(
                target_image,
                fill_style,
                Recti::from_top_left_width_height(Vec2i{x, target_image->height - y}, w, h));
    }

    void
    Canvas::translate(float x, float y)
    {
        const auto m = Mat3f::from_translation2d(Vec2f {x, y});
        transform = transform * m;
    }

    void
    Canvas::rotate(float r)
    {
        transform = transform * Mat3f{Mat2f::from_rotation(Angle::from_radians(r))};
    }

    void
    Canvas::begin_path()
    {
        ASSERT(!building_path);
        path.resize(0);
        building_path = true;
    }

    void
    Canvas::close_path()
    {
        ASSERT(building_path);
        building_path = false;
    }

    void
    Canvas::move_to(float x, float y)
    {
        ASSERT(building_path);
        ASSERT(path.empty());
        path.push_back(transform_position(Vec2f(x, y)));
    }

    void
    Canvas::line_to(float dx, float dy)
    {
        ASSERT(building_path);
        if(path.empty())
        {
            path.push_back(transform_position(Vec2f::zero()));
        }
        path.push_back(transform_position(Vec2f(dx, dy)));
    }

    void
    Canvas::fill() const
    {
        ASSERT(!building_path);
        fill_poly(target_image, fill_style, path);
    }
}

