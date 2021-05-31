#pragma once

namespace euphoria::core
{
    struct sensitivity
    {
        explicit sensitivity(float v=1.0f, bool i=false);

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] float get_multiplier_with_sign() const;

        float multiplier;
        bool inverted;
    };
}

