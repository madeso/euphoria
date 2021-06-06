#include "core/vec2.h"

#include "core/random.h"
#include "core/angle.h"


namespace euphoria::core
{
    unit2f create_random_unit(random* random)
    {
        const auto angle = angle::random(random);
        const auto x = cos(angle);
        const auto y = sin(angle);
        return unit2f{x, y};
    }
}
