#ifndef GUI_LAYOUTDATA_H
#define GUI_LAYOUTDATA_H

namespace euphoria::gui
{
    struct LayoutData
    {
    public:
        LayoutData();
        ~LayoutData();

        int
        GetColumn() const;

        LayoutData&
        SetColumn(int c);

        int
        GetRow() const;

        LayoutData&
        SetRow(int r);

        float
        GetPreferredWidth() const;

        LayoutData&
        SetPreferredWidth(float w);

        float
        GetPreferredHeight() const;

        LayoutData&
        SetPreferredHeight(float h);

    private:
        int column_;
        int row_;
        float preferred_width_;
        float preferred_height_;
    };
}

#endif  // GUI_LAYOUTDATA_H
