#include "core/sensitivity.h"

#include "base/numeric.h"


namespace eu::core
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
