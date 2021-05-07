#include "core/sensitivity.h"

#include "core/numeric.h"


namespace euphoria::core
{
    Sensitivity::Sensitivity(float v, bool i)
        : value(v)
        , inverted(i)
    { }

    
    bool
    Sensitivity::IsValid() const
    {
        return value > 0.0f;
    }


    float
    Sensitivity::GetValueWithSign() const
    {
        return value * get_sign(inverted);
    }
}
