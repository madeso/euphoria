#include "core/vec2.h"

#include "core/random.h"
#include "core/angle.h"


namespace euphoria::core
{
    Unit2f create_random_unit(Random* random)
    {
        const auto angle = Angle::random(random);
        const auto x = cos(angle);
        const auto y = sin(angle);
        return Unit2f{x, y};
    }
}
