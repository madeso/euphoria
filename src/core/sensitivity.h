#ifndef EUPHORIA_CORE_SENSITIVITY_H
#define EUPHORIA_CORE_SENSITIVITY_H

namespace euphoria::core
{
    struct Sensitivity
    {
        explicit Sensitivity(float v=1.0f, bool i=false);

        bool IsValid() const;
        float GetValueWithSign() const;

        float value;
        bool inverted;
    };
}

#endif  // EUPHORIA_CORE_SENSITIVITY_H
