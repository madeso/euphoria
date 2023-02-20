#pragma once

namespace euphoria::gui
{
    struct LayoutData
    {
        int column = 0;
        int row = 0;
        float preferred_width = 0.0f;
        float preferred_height = 0.0f;

        LayoutData() = default;
    };
}
