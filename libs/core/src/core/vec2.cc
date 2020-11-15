#include "core/vec2.h"

#include "core/random.h"
#include "core/angle.h"


namespace euphoria::core
{
    unit2f RandomUnit(Random* random)
    {
        const auto angle = Angle::Random(random);
        const auto x = Cos(angle);
        const auto y = Sin(angle);
        return unit2f{x, y};
    }
}
