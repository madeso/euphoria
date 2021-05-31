#include "core/sensitivity.h"

#include "core/numeric.h"


namespace euphoria::core
{
    sensitivity::sensitivity(float v, bool i)
        : multiplier(v)
        , inverted(i)
    { }

    
    bool
    sensitivity::is_valid() const
    {
        return multiplier > 0.0f;
    }


    float
    sensitivity::get_multiplier_with_sign() const
    {
        return multiplier * get_sign(inverted);
    }
}
