#pragma once

namespace euphoria::gui
{
    struct layout_data
    {
        layout_data() = default;

        int column = 0;
        int row = 0;
        float preferred_width = 0.0f;
        float preferred_height = 0.0f;
    };
}
