#include "gui/layout.h"

#include "base/numeric.h"
#include "log/log.h"
#include "base/stringmerger.h"
#include "base/stringutils.h"
#include "base/cint.h"

#include "gui/widget.h"

#include <numeric>
#include <algorithm>


namespace eu::gui
{
    Layout::Layout() = default;


    Layout::~Layout() = default;


    TableLayout::TableLayout
    (
            const std::vector<bool>& er,
            const std::vector<bool>& ec,
            float cp
    )
        : expandable_rows(er)
        , expandable_cols(ec)
        , combined_padding(cp)
    {
    }


    namespace
    {
        template <typename T>
        void
        update_max(T* t, T value)
        {
            *t = max(*t, value);
        }
    }


    size2f
    TableLayout::calc_minimum_area
    (
        const std::vector<std::shared_ptr<Widget>>& widgets
    ) const
    {
        // todo(Gustav): include padding
        std::vector<float> width(expandable_cols.size(), 0.0f);
        std::vector<float> height(expandable_rows.size(), 0.0f);

        for(const auto& w: widgets)
        {
            const auto& d = w->layout;
            const auto& s = w->get_preferred_size();
            update_max(&width[d.column], s.width);
            update_max(&height[d.row], s.height);
        }

        const auto s = size2f::create_from_width_height
        (
            std::accumulate(width.begin(), width.end(), 0.0f),
            std::accumulate(height.begin(), height.end(), 0.0f)
        );

        return s;
    }


    void
    TableLayout::lay_out
    (
        std::vector<std::shared_ptr<Widget>>* widgets,
        const Rectf& area
    ) const
    {
        // todo(Gustav): include padding
        std::vector<float> width(expandable_cols.size(), 0.0f);
        std::vector<float> height(expandable_rows.size(), 0.0f);

        for(const auto& w: *widgets)
        {
            const auto& d = w->layout;
            const auto& s = w->get_preferred_size();
            update_max(&width[d.column], s.width);
            update_max(&height[d.row], s.height);
        }

        const float total_width = std::accumulate(width.begin(), width.end(), 0.0f);
        const float total_height = std::accumulate(height.begin(), height.end(), 0.0f);

        const float leftover_width = area.get_width() - total_width;
        const float leftover_height = area.get_height() - total_height;

        const int expandable_rows_count = c_sizet_to_int
        (
            std::count(expandable_rows.begin(), expandable_rows.end(), true)
        );
        const int expandable_cols_count = c_sizet_to_int
        (
            std::count(expandable_cols.begin(), expandable_cols.end(), true)
        );

        if(expandable_rows_count != 0)
        {
            const float extra = leftover_height / c_int_to_float(expandable_rows_count);
            for(unsigned int row_index = 0; row_index < expandable_rows.size(); ++row_index)
            {
                if(expandable_rows[row_index])
                {
                    height[row_index] += extra;
                }
            }
        }

        if(expandable_cols_count != 0)
        {
            const float extra = leftover_width / c_int_to_float(expandable_cols_count);
            for(unsigned int column_index = 0; column_index < expandable_cols.size(); ++column_index)
            {
                if(expandable_cols[column_index])
                {
                    width[column_index] += extra;
                }
            }
        }

        for(const auto& w: *widgets)
        {
            const LayoutData& d = w->layout;
            const auto topleft = area.get_top_left();
            float x = topleft.x;
            float y = topleft.y;

            for(int column_index = 0; column_index < d.column; ++column_index)
            {
                x += width[column_index];
            }
            for(int row_index = 0; row_index < d.row; ++row_index)
            {
                y -= height[row_index];
            }

            w->set_rect
            (
                Rectf::from_top_left_width_height
                (
                    vec2f{x, y},
                    width[d.column],
                    height[d.row]
                )
            );
        }
    }


    SingleRowLayout::SingleRowLayout(float p)
        : padding(p)
    {
    }


    size2f
    SingleRowLayout::calc_minimum_area
    (
        const std::vector<std::shared_ptr<Widget>>& widgets
    ) const
    {
        float width = 0;
        float height = 0;
        bool first = false;

        for(const auto& w: widgets)
        {
            const auto& s = w->get_preferred_size();
            width += s.width;
            if(!first)
            {
                width += padding;
            }
            first = false;
            update_max(&height, s.height);
        }

        const auto s = size2f::create_from_width_height(width, height);
        return s;
    }


    void
    SingleRowLayout::lay_out
    (
        std::vector<std::shared_ptr<Widget>>* widgets,
        const Rectf& area
    ) const
    {
        const auto tl = area.get_top_left();
        float x = tl.x;
        for(const auto& w: *widgets)
        {
            const auto& s = w->get_preferred_size();
            w->set_rect
            (
                Rectf::from_top_left_width_height
                (
                    vec2f{x, tl.y},
                    s.width,
                    s.height
                )
            );
            x += s.width + padding;
        }
    }
}

