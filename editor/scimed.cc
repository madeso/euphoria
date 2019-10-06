#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"
#include "core/str.h"
#include "core/numeric.h"
#include "core/optionalindex.h"

#include "render/texture.h"
#include "render/texturecache.h"

#include "window/imgui_ext.h"
#include "window/imgui_icons.h"

// #include "window/canvas.h"

#include <iomanip>
#include <algorithm>

using namespace euphoria::core;
using namespace euphoria::window;

namespace euphoria::editor
{
    bool
    IsCloseTo(float a, float b, float c = 3)
    {
        return Abs(a - b) < c;
    }

    bool
    KindaTheSame(int a, int b)
    {
        return std::abs(a - b) < 5;
    }

    using PositionClassification = OptionalIndex<int>;

    /// Extra calculated data from the corresponding data row (either row or column)
    /// in the ScalingSprite struct. Like the data, this can be viewed as the space
    /// between the dividing lines.
    class SpaceData
    {
        public:
        SpaceData(int aleft, int aright, int i)
            : left(aleft), right(aright), index(i)
        {}
        int left;
        int right;
        int index;
    };


    /// The visual representation of a line, dividing or splitting a area
    /// The actual data is either the row or col array of the ScalingSprite struct
    class SplitData
    {
        public:
        SplitData(int p, int mi, int ma)
            : position(p), min_value(mi), max_value(ma)
        {}
        // defines the position of the split
        int position;

        // the split area
        int min_value;
        int max_value;
    };

    std::string
    PixelsOrPercentageString(int data_value, int total_percentage)
    {
        if (data_value > 0)
        {
            return Str {} << data_value << "px";
        }
        else
        {
            const auto p = (-data_value * 100.0f) / total_percentage;
            return Str {} << std::setprecision(3) << p << "%";
        }
    }

    int
    GetTotalPercentage(const std::vector<int>& data)
    {
        int total = 0;
        for (int i: data)
        {
            if (i < 0)
            {
                total += -i;
            }
        }
        return total;
    }


    std::vector<SpaceData>
    CalculateAllSpaces(const std::vector<int>& data)
    {
        std::vector<SpaceData> ret;
        int                    x = 0;
        for (unsigned int index = 0; index < data.size(); ++index)
        {
            const int value = data[index];
            const int step  = abs(value);
            ret.emplace_back(x, x + step, index);
            x += step;
        }
        return ret;
    }

    int
    FindSpaceIndex(const std::vector<int>& data_ptr, int x)
    {
        const auto data  = CalculateAllSpaces(data_ptr);
        int        last  = 0;
        int        index = 0;
        for (const auto& d: data)
        {
            last = index;
            if (x < d.right)
            {
                return last;
            }
            index += 1;
        }
        return last;
    }

    PositionClassification
    FindSpaceIndexOrNull(const std::vector<int>& data, int y, int image_size)
    {
        const bool within_image = y > 0 && y < image_size;

        if (within_image)
        {
            return PositionClassification::FromIndex(FindSpaceIndex(data, y));
        }
        else
        {
            return PositionClassification::Null();
        }
    }

    std::vector<SplitData>
    CalculateAllSplits(const std::vector<int>& data)
    {
        std::vector<SplitData> ret;
        bool                   has_data = false;
        int                    x        = 0;
        int                    last_x   = 0;
        for (const auto i: data)
        {
            int dx = std::abs(i);
            if (has_data)
            {
                ret.emplace_back(SplitData(x, last_x, x + dx));
            }
            else
            {
                has_data = true;
            }
            last_x = x;
            x += dx;
        }
        return ret;
    }


    void
    DrawLine(const Canvas& canvas, int x, int y, int tx, int ty, ImU32 color)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto  from      = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(x), static_cast<float>(y)});
        const auto to = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(tx), static_cast<float>(ty)});
        draw_list->AddLine(from, to, color);
    }

    void
    DrawAnchorDown(const Canvas& canvas, int x, int y, int size, ImU32 color)
    {
        DrawLine(canvas, x, y, x, y + size, color);
    }

    void
    DrawAnchorLeft(const Canvas& canvas, int x, int y, int size, ImU32 color)
    {
        DrawLine(canvas, x, y, x + size, y, color);
    }

    bool
    ButtonAt(const Canvas& canvas, const ImVec2& p, const char* label, int id)
    {
        const auto backup = ImGui::GetCursorPos();
        ImVec2     pp     = p;
        pp.x -= canvas.position.x;
        pp.y -= canvas.position.y;
        ImGui::SetCursorPos(pp);
        ImGui::PushID(id);
        const bool clicked = ImGui::Button(label);
        ImGui::PopID();
        ImGui::SetCursorPos(backup);
        return clicked;
    }

    bool
    DrawHorizontalCenteredText(
            const Canvas&      canvas,
            int                left_p,
            int                right_p,
            int                y_p,
            const std::string& s,
            int                id)
    {
        const auto size = ImGui::CalcTextSize(s.c_str());
        const auto left = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(left_p), static_cast<float>(y_p)});
        const auto right = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(right_p), static_cast<float>(y_p)});
        const auto y = left.y;
        auto       x = left.x + (right.x - left.x) / 2 - size.x / 2;
        const auto p = ImVec2 {x, y - size.y};
        return ButtonAt(canvas, p, s.c_str(), id);
    }

    bool
    DrawVerticalCenteredText(
            const Canvas&      canvas,
            int                top_p,
            int                bottom_p,
            int                x_p,
            const std::string& s,
            int                id)
    {
        const auto size = ImGui::CalcTextSize(s.c_str());
        const auto top  = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(x_p), static_cast<float>(top_p)});
        const auto bottom = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(x_p), static_cast<float>(bottom_p)});
        const auto x = bottom.x;
        const auto y = top.y + (bottom.y - top.y) / 2 - size.y / 2;
        const auto p = ImVec2 {x - size.x, y};
        return ButtonAt(canvas, p, s.c_str(), id);
    }


    template <
            typename TAnchorFunction,
            typename TLineFunction,
            typename TButtonFunction>
    void
    DrawSizerCommon(
            std::shared_ptr<render::Texture2d> image,
            const ScimedConfig&                sc,
            std::vector<int>*                  data_ptr,
            int                                end,
            TAnchorFunction                    anchor_function,
            TLineFunction                      line_function,
            TButtonFunction                    button_function)
    {
        auto&      data   = *data_ptr;
        const auto spaces = CalculateAllSpaces(data);
        anchor_function(0, -sc.sizer_distance, sc.anchor_size);
        line_function(end, -sc.sizer_distance);
        anchor_function(end, -sc.sizer_distance, sc.anchor_size);

        const int total_percentage = GetTotalPercentage(data);

        int index = 0;
        for (const auto& t: spaces)
        {
            const bool clicked = button_function(
                    t.left,
                    t.right,
                    -sc.sizer_text_distance,
                    PixelsOrPercentageString(data[index], total_percentage));
            if (clicked)
            {
                data[index] = -data[index];
            }
            index += 1;
        }
    }

    void
    DrawSizer(
            std::shared_ptr<render::Texture2d> image,
            const Scimed&                      sc,
            const ScimedConfig&                scc,
            scalingsprite::ScalingSprite*      sprite)
    {
        int sized_id = 0;
        DrawSizerCommon(
                image,
                scc,
                &sprite->cols,
                image->GetWidth(),
                [&sc, &scc](int position, int distance, int size) {
                    DrawAnchorDown(
                            sc.canvas,
                            position,
                            distance,
                            size,
                            scc.sizer_color);
                },
                [&sc, &scc](int end, int distance) {
                    DrawLine(
                            sc.canvas,
                            0,
                            distance,
                            end,
                            distance,
                            scc.sizer_color);
                },
                [&sc, &sized_id](
                        int                left,
                        int                right,
                        int                distance,
                        const std::string& text) -> bool {
                    sized_id += 1;
                    return DrawHorizontalCenteredText(
                            sc.canvas, left, right, distance, text, sized_id);
                });

        DrawSizerCommon(
                image,
                scc,
                &sprite->rows,
                image->GetHeight(),
                [&sc, &scc](int position, int distance, int size) {
                    DrawAnchorLeft(
                            sc.canvas,
                            distance,
                            position,
                            size,
                            scc.sizer_color);
                },
                [&sc, &scc](int end, int distance) {
                    DrawLine(
                            sc.canvas,
                            distance,
                            0,
                            distance,
                            end,
                            scc.sizer_color);
                },
                [&sc, &sized_id](
                        int                left,
                        int                right,
                        int                distance,
                        const std::string& text) -> bool {
                    sized_id += 1;
                    return DrawVerticalCenteredText(
                            sc.canvas, left, right, distance, text, sized_id);
                });
    }


    template <typename TLineFunction, typename TCoordFunction>
    int
    DrawSingleAxisSplits(
            const std::vector<int>& data,
            const ImVec2&           mouse,
            Canvas*                 canvas,
            TLineFunction           line_function,
            TCoordFunction          coord_function)
    {
        const auto splits = CalculateAllSplits(data);

        int ret = -1;

        int i = 0;
        for (auto s: splits)
        {
            line_function(s.position);
            const auto p = canvas->WorldToScreen(
                    ImVec2 {static_cast<float>(s.position),
                            static_cast<float>(s.position)});
            if (IsCloseTo(coord_function(mouse), coord_function(p)))
            {
                ret = i;
            }
            i += 1;
        }

        return ret;
    }

    LineHoverData
    DrawSplits(
            scalingsprite::ScalingSprite* sprite,
            Canvas*                       canvas,
            const ScimedConfig&           scc)
    {
        const auto    mouse = ImGui::GetMousePos();
        LineHoverData ret;

        ret.vertical_index = DrawSingleAxisSplits(
                sprite->cols,
                mouse,
                canvas,
                [&](int position) {
                    canvas->VerticalLine(position, scc.split_color);
                },
                [](const ImVec2& p) -> float { return p.x; });
        ret.horizontal_index = DrawSingleAxisSplits(
                sprite->rows,
                mouse,
                canvas,
                [&](int position) {
                    canvas->HorizontalLine(position, scc.split_color);
                },
                [](const ImVec2& p) -> float { return p.y; });

        return ret;
    }

    void
    SetMouseCursorFromHover(const LineHoverData& hover)
    {
        if (hover.horizontal_index != -1 || hover.vertical_index != -1)
        {
            if (hover.horizontal_index != -1 && hover.vertical_index != -1)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            }
            else if (hover.horizontal_index != -1)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            }
            else
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
        }
    }


    void
    MoveSplit(int split_index, std::vector<int>* data_ptr, float world_position)
    {
        if (split_index == -1)
        {
            return;
        }

        const auto split      = CalculateAllSplits(*data_ptr)[split_index];
        const auto position   = static_cast<int>(world_position);
        const auto max        = std::max(split.min_value + 1, position);
        const int  line_x     = std::min(split.max_value - 1, max);
        const int  left_size  = line_x - split.min_value;
        const int  right_size = split.max_value - line_x;

        // move split to new position
        auto& data            = *data_ptr;
        data[split_index]     = Sign(data[split_index]) * left_size;
        data[split_index + 1] = Sign(data[split_index + 1]) * right_size;
    }

    void
    SplitSpaceInTwo(
            std::vector<int>*             data_ptr,
            const PositionClassification& optional_space_index,
            int                           x)
    {
        ASSERT(optional_space_index);
        const auto index = optional_space_index.GetIndex();
        auto&      data  = *data_ptr;
        const auto size  = CalculateAllSpaces(data)[index];
        const int  mouse = std::min(size.right - 1, std::max(size.left + 1, x));
        ASSERT(mouse > size.left);
        const int left_abs_val = mouse - size.left;
        const int old_value    = data[index];
        const int sign         = Sign(old_value);
        const int left_val     = left_abs_val * sign;
        data[index]            = left_val;
        const int right_val    = sign * (std::abs(old_value) - left_abs_val);
        data.insert(data.begin() + index + 1, right_val);
    }

    bool
    Scimed::Run(const CanvasConfig& cc, const ScimedConfig& scc)
    {
        canvas.Begin(cc);
        canvas.ShowGrid(cc);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const auto mouse = ImGui::GetMousePos();

        if (!texture)
        {
            canvas.ShowRuler(cc);
            canvas.End(cc);
            return false;
        }

        // draw texture
        auto       tex_id = reinterpret_cast<ImTextureID>(texture->GetId());
        const auto pos    = canvas.WorldToScreen(ImVec2 {0, 0});
        const auto size   = canvas.WorldToScreen(
                ImVec2 {static_cast<float>(texture->GetWidth()),
                        static_cast<float>(texture->GetHeight())});
        draw_list->AddImage(tex_id, pos, size);

        const auto current_hover = DrawSplits(scaling.get(), &canvas, scc);
        DrawSizer(texture, *this, scc, scaling.get());

        canvas.ShowRuler(cc);
        canvas.End(cc);

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            hover = current_hover;
        }

        const bool is_tracking
                = hover.horizontal_index != -1 || hover.vertical_index != -1;

        if (is_tracking)
        {
            if (ImGui::IsMouseDown(0))
            {
                SetMouseCursorFromHover(hover);

                const auto me = canvas.ScreenToWorld(ImGui::GetMousePos());

                MoveSplit(hover.horizontal_index, &scaling->rows, me.y);
                MoveSplit(hover.vertical_index, &scaling->cols, me.x);
            }
            else if (ImGui::IsItemHovered())
            {
                SetMouseCursorFromHover(current_hover);
                hover = LineHoverData {};
            }
        }
        else if (ImGui::IsItemHovered())
        {
            SetMouseCursorFromHover(current_hover);
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {
            ImGui::OpenPopup("asd");
            const auto w = canvas.ScreenToWorld(ImGui::GetMousePos());
            mouse_popup  = vec2i {static_cast<int>(w.x), static_cast<int>(w.y)};
        }

        if (ImGui::BeginPopup("asd"))
        {
            const auto space_index_y = FindSpaceIndexOrNull(
                    scaling->rows, mouse_popup.y, texture->GetHeight());
            const auto space_index_x = FindSpaceIndexOrNull(
                    scaling->cols, mouse_popup.x, texture->GetWidth());

            if (window::ImguiSelectableOrDisabled(
                        space_index_y,
                        ICON_FK_ARROWS_H " New Horizontal divider"))
            {
                SplitSpaceInTwo(&scaling->rows, space_index_y, mouse_popup.y);
            }

            if (window::ImguiSelectableOrDisabled(
                        space_index_x,
                        ICON_FK_ARROWS_V " New Vertical divider"))
            {
                SplitSpaceInTwo(&scaling->cols, space_index_x, mouse_popup.x);
            }
            ImGui::EndPopup();
        }

        return false;
    }
}  // namespace euphoria::editor
