#pragma once

namespace euphoria::gui
{
    struct LayoutData
    {
        LayoutData() = default;

        // todo(Gustav): cleanup this horrible get/set structure

        [[nodiscard]] int
        GetColumn() const;

        LayoutData&
        SetColumn(int c);

        [[nodiscard]] int
        GetRow() const;

        LayoutData&
        SetRow(int r);

        [[nodiscard]] float
        GetPreferredWidth() const;

        LayoutData&
        SetPreferredWidth(float w);

        [[nodiscard]] float
        GetPreferredHeight() const;

        LayoutData&
        SetPreferredHeight(float h);

        int column_;
        int row_;
        float preferred_width_;
        float preferred_height_;
    };
}
