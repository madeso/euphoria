#pragma once

namespace euphoria::core
{
    struct Sensitivity
    {
        float multiplier;
        bool inverted;

        explicit Sensitivity(float v=1.0f, bool i=false);

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] float get_multiplier_with_sign() const;
    };
}

