#include "core/rect.h"

#include "core/random.h"


namespace euphoria::core
{

    template<typename T>
    Vec2<T>
    get_random_point_impl(Random* random, const Rect<T>& r)
    {
        const T x = get_random_in_range(random, r.get_width());
        const T y = get_random_in_range(random, r.get_height());
        return r.get_position_from_bottom_left(Vec2<T> {x, y});
    }

    Vec2f
    get_random_point(Random* random, const Rect<float>& r)
    {
        return get_random_point_impl(random, r);
    }


    Vec2i get_random_point(Random* random, const Rect<int>& r)
    {
        return get_random_point_impl(random, r);
    }
}
