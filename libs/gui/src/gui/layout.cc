#include "gui/layout.h"

#include "core/numeric.h"
#include "core/log.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"
#include "core/cint.h"

#include "gui/widget.h"

#include <numeric>
#include <algorithm>


namespace euphoria::gui
{
    Layout::Layout() = default;


    Layout::~Layout() = default;


    TableLayout::TableLayout
    (
            const std::vector<bool>& expandable_rows,
            const std::vector<bool>& expandable_cols,
            float combined_padding
    )
        : expandable_rows_(expandable_rows)
        , expandable_cols_(expandable_cols)
        , combined_padding_(combined_padding)
    {
    }


    namespace
    {
        template <typename T>
        void
        UpdateMax(T* t, T value)
        {
            *t = core::max(*t, value);
        }
    }


    core::Sizef
    TableLayout::CalculateMinimumArea
    (
        const std::vector<std::shared_ptr<Widget>>& widgets
    ) const
    {
        // todo(Gustav): include padding
        std::vector<float> width(expandable_cols_.size(), 0.0f);
        std::vector<float> height(expandable_rows_.size(), 0.0f);

        for(const auto& w: widgets)
        {
            const auto& d = w->layout;
            const auto& s = w->GetPreferredSize();
            UpdateMax(&width[d.GetColumn()], s.width);
            UpdateMax(&height[d.GetRow()], s.height);
        }

        const auto s = core::Sizef::create_from_width_height
        (
            std::accumulate(width.begin(), width.end(), 0.0f),
            std::accumulate(height.begin(), height.end(), 0.0f)
        );

        LOG_INFO("Calculate minumum area {0}", s);
        return s;
    }


    namespace
    {
        std::string
        DebugVector(const std::vector<float>& float_vector)
        {
            const auto string_vector = core::to_string_vector
            (
                float_vector
            );
            return core::string_mergers::array.merge(string_vector);
        }
    }

    void
    TableLayout::DoLayout
    (
        std::vector<std::shared_ptr<Widget>>* widgets,
        const core::rectf& area
    ) const
    {
        LOG_INFO("Doing table layout in {0}", area);

        // todo(Gustav): include padding
        std::vector<float> width(expandable_cols_.size(), 0.0f);
        std::vector<float> height(expandable_rows_.size(), 0.0f);

        for(const auto& w: *widgets)
        {
            const auto& d = w->layout;
            const auto& s = w->GetPreferredSize();
            UpdateMax(&width[d.GetColumn()], s.width);
            UpdateMax(&height[d.GetRow()], s.height);
        }

        LOG_INFO("Table widths: {0}", DebugVector(width));
        LOG_INFO("Table heights: {0}", DebugVector(height));

        const float total_width = std::accumulate(width.begin(), width.end(), 0.0f);
        const float total_height = std::accumulate(height.begin(), height.end(), 0.0f);

        LOG_INFO("Width {0} height: {1}", total_width, total_height);

        const float leftover_width = area.get_width() - total_width;
        const float leftover_height = area.get_height() - total_height;

        const int expandable_rows_count = core::Csizet_to_int
        (
            std::count(expandable_rows_.begin(), expandable_rows_.end(), true)
        );
        const int expandable_cols_count = core::Csizet_to_int
        (
            std::count(expandable_cols_.begin(), expandable_cols_.end(), true)
        );

        if(expandable_rows_count != 0)
        {
            const float extra = leftover_height / core::Cint_to_float(expandable_rows_count);
            for(unsigned int i = 0; i < expandable_rows_.size(); ++i)
            {
                if(expandable_rows_[i])
                {
                    height[i] += extra;
                }
            }
        }

        if(expandable_cols_count != 0)
        {
            const float extra = leftover_width / core::Cint_to_float(expandable_cols_count);
            for(unsigned int i = 0; i < expandable_cols_.size(); ++i)
            {
                if(expandable_cols_[i])
                {
                    width[i] += extra;
                }
            }
        }

        for(const auto& w: *widgets)
        {
            const LayoutData& d = w->layout;
            const auto topleft = area.get_top_left();
            float x = topleft.x;
            float y = topleft.y;

            LOG_INFO
            (
                "widget '{0}' x {1} y {2}",
                w->name,
                x,
                y
            );

            for(int c = 0; c < d.GetColumn(); ++c)
            {
                x += width[c];
            }
            for(int r = 0; r < d.GetRow(); ++r)
            {
                y -= height[r];
            }

            w->SetRect
            (
                core::rectf::from_top_left_width_height
                (
                    core::vec2f{x, y},
                    width[d.GetColumn()],
                    height[d.GetRow()]
                )
            );
        }
    }


    SingleRowLayout::SingleRowLayout(float padding)
        : padding_(padding)
    {
    }


    core::Sizef
    SingleRowLayout::CalculateMinimumArea
    (
        const std::vector<std::shared_ptr<Widget>>& widgets
    ) const
    {
        float width = 0;
        float height = 0;
        bool first = false;

        for(const auto& w: widgets)
        {
            const auto& s = w->GetPreferredSize();
            width += s.width;
            if(!first)
            {
                width += padding_;
            }
            first = false;
            UpdateMax(&height, s.height);
        }

        const auto s = core::Sizef::create_from_width_height(width, height);
        LOG_INFO("Single row layout min size: {0}", s);
        return s;
    }


    void
    SingleRowLayout::DoLayout
    (
        std::vector<std::shared_ptr<Widget>>* widgets,
        const core::rectf& area
    ) const
    {
        LOG_INFO("Doing single row layout in {0}", area);
        const auto tl = area.get_top_left();
        float x = tl.x;
        for(const auto& w: *widgets)
        {
            const auto& s = w->GetPreferredSize();
            w->SetRect
            (
                core::rectf::from_top_left_width_height
                (
                    core::vec2f{x, tl.y},
                    s.width,
                    s.height
                )
            );
            x += s.width + padding_;
        }
    }
}

