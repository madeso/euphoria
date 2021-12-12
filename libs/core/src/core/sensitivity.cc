#include "core/sensitivity.h"

#include "core/numeric.h"


namespace euphoria::core
{
    Sensitivity::Sensitivity(float v, bool i)
        : multiplier(v)
        , inverted(i)
    { }

    
    bool
    Sensitivity::is_valid() const
    {
        return multiplier > 0.0f;
    }


    float
    Sensitivity::get_multiplier_with_sign() const
    {
        return multiplier * get_sign(inverted);
    }
}
