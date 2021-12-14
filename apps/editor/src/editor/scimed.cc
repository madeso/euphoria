#include "editor/scimed.h"

#include "core/range.h"
#include "core/canvaslogic.h"
#include "core/str.h"
#include "core/numeric.h"

#include "render/texture.h"
#include "render/texturecache.h"

#include "window/imgui_extra.h"
#include "window/imgui_icons.h"

#include <iomanip>
#include <algorithm>
#include <optional>

using namespace euphoria::core;
using namespace euphoria::window;

namespace euphoria::editor
{
    bool
    is_close_to(float a, float b, float c = 3)
    {
        return core::abs(a - b) < c;
    }

    bool
    kinda_the_same(int a, int b)
    {
        return std::abs(a - b) < 5;
    }

    using position_classification = std::optional<int>;

    /// Extra calculated data from the corresponding data row (either row or column)
    /// in the ScalingSprite struct. Like the data, this can be viewed as the space
    /// between the dividing lines.
    struct space_data
    {
        space_data(int aleft, int aright, int i)
            : left(aleft)
            , right(aright)
            , index(i)
        {
        }

        int left;
        int right;
        int index;
    };


    /// The visual representation of a line, dividing or splitting a area
    /// The actual data is either the row or col array of the ScalingSprite struct
    struct split_data
    {
        split_data(int p, int mi, int ma)
            : position(p)
            , min_value(mi)
            , max_value(ma)
        {
        }

        // defines the position of the split
        int position;

        // the split area
        int min_value;
        int max_value;
    };


    std::string
    pixels_or_percentage_string(int data_value, int total_percentage)
    {
        if(data_value > 0)
        {
            return StringBuilder {} << data_value << "px";
        }
        else
        {
            const auto p = (core::c_int_to_float(-data_value) * 100.0f) / core::c_int_to_float(total_percentage);
            return StringBuilder {} << std::setprecision(3) << p << "%";
        }
    }


    int
    get_total_percentage(const std::vector<int>& data)
    {
        int total = 0;
        for(int i: data)
        {
            if(i < 0)
            {
                total += -i;
            }
        }
        return total;
    }


    std::vector<space_data>
    calculate_all_spaces(const std::vector<int>& data)
    {
        std::vector<space_data> ret;
        int x = 0;

        for(unsigned int index = 0; index < data.size(); ++index)
        {
            const int value = data[index];
            const int step = abs(value);
            ret.emplace_back(x, x + step, index);
            x += step;
        }

        return ret;
    }


    int
    find_space_index(const std::vector<int>& data_ptr, int x)
    {
        const auto data = calculate_all_spaces(data_ptr);
        int last = 0;
        int index = 0;

        for(const auto& d: data)
        {
            last = index;
            if(x < d.right)
            {
                return last;
            }
            index += 1;
        }

        return last;
    }


    position_classification
    find_space_index_or_null(const std::vector<int>& data, int y, int image_size)
    {
        const bool within_image = y > 0 && y < image_size;

        if(within_image)
        {
            return find_space_index(data, y);
        }
        else
        {
            return std::nullopt;
        }
    }


    std::vector<split_data>
    calculate_all_splits(const std::vector<int>& data)
    {
        std::vector<split_data> ret;
        bool has_data = false;
        int x = 0;
        int last_x = 0;

        for(const auto i: data)
        {
            int dx = std::abs(i);
            if(has_data)
            {
                ret.emplace_back(split_data(x, last_x, x + dx));
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
    draw_line(const Canvas& canvas, int x, int y, int tx, int ty, ImU32 color)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto from = canvas.world_to_screen(ImVec2{static_cast<float>(x), static_cast<float>(y)});
        const auto to = canvas.world_to_screen(ImVec2{static_cast<float>(tx), static_cast<float>(ty)});
        draw_list->AddLine(from, to, color);
    }


    void
    draw_anchor_down(const Canvas& canvas, int x, int y, int size, ImU32 color)
    {
        draw_line(canvas, x, y, x, y + size, color);
    }


    void
    draw_anchor_left(const Canvas& canvas, int x, int y, int size, ImU32 color)
    {
        draw_line(canvas, x, y, x + size, y, color);
    }


    bool
    button_at(const Canvas& canvas, const ImVec2& p, const char* label, int id)
    {
        const auto backup = ImGui::GetCursorPos();
        ImVec2 pp = p;
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
    draw_horizontal_centered_text(const Canvas& canvas, int left_p, int right_p, int y_p, const std::string& s, int id)
    {
        const auto size = ImGui::CalcTextSize(s.c_str());
        const auto left = canvas.world_to_screen(ImVec2{static_cast<float>(left_p), static_cast<float>(y_p)});
        const auto right = canvas.world_to_screen(ImVec2{static_cast<float>(right_p), static_cast<float>(y_p)});
        const auto y = left.y;
        auto x = left.x + (right.x - left.x) / 2 - size.x / 2;
        const auto p = ImVec2 {x, y - size.y};
        return button_at(canvas, p, s.c_str(), id);
    }


    bool
    draw_vertical_centered_text(const Canvas& canvas, int top_p, int bottom_p, int x_p, const std::string& s, int id)
    {
        const auto size = ImGui::CalcTextSize(s.c_str());
        const auto top = canvas.world_to_screen(ImVec2{static_cast<float>(x_p), static_cast<float>(top_p)});
        const auto bottom = canvas.world_to_screen(ImVec2{static_cast<float>(x_p), static_cast<float>(bottom_p)});
        const auto x = bottom.x;
        const auto y = top.y + (bottom.y - top.y) / 2 - size.y / 2;
        const auto p = ImVec2 {x - size.x, y};
        return button_at(canvas, p, s.c_str(), id);
    }


    template
    <
        typename TAnchorFunction,
        typename TLineFunction,
        typename TButtonFunction
    >
    void
    draw_sizer_common
    (
        const scimed_config& sc,
        std::vector<int>* data_ptr,
        int end,
        TAnchorFunction anchor_function,
        TLineFunction line_function,
        TButtonFunction button_function
    )
    {
        auto& data = *data_ptr;
        const auto spaces = calculate_all_spaces(data);
        anchor_function(0, -sc.sizer_distance, sc.anchor_size);
        line_function(end, -sc.sizer_distance);
        anchor_function(end, -sc.sizer_distance, sc.anchor_size);

        const int total_percentage = get_total_percentage(data);
        int index = 0;

        for(const auto& t: spaces)
        {
            const bool clicked = button_function
            (
                t.left,
                t.right,
                -sc.sizer_text_distance,
                pixels_or_percentage_string(data[index], total_percentage)
            );
            if(clicked)
            {
                data[index] = -data[index];
            }
            index += 1;
        }
    }


    void
    draw_sizer
    (
        std::shared_ptr<render::Texture2> image,
        const scimed& sc,
        const scimed_config& scc,
        scalingsprite::ScalingSprite* sprite
    )
    {
        int sized_id = 0;
        draw_sizer_common
        (
            scc,
            &sprite->cols,
            image->width,
            [&sc, &scc](int position, int distance, int size)
            {
                draw_anchor_down(sc.canvas, position, distance, size, scc.sizer_color);
            },
            [&sc, &scc](int end, int distance)
            {
                draw_line(sc.canvas, 0, distance, end, distance, scc.sizer_color);
            },
            [&sc, &sized_id](int left, int right, int distance, const std::string &text) -> bool
            {
                sized_id += 1;
                return draw_horizontal_centered_text(sc.canvas, left, right, distance, text, sized_id);
            }
        );

        draw_sizer_common
        (
            scc,
            &sprite->rows,
            image->height,
            [&sc, &scc](int position, int distance, int size)
            {
                draw_anchor_left(sc.canvas, distance, position, size, scc.sizer_color);
            },
            [&sc, &scc](int end, int distance)
            {
                draw_line(sc.canvas, distance, 0, distance, end, scc.sizer_color);
            },
            [&sc, &sized_id](int left, int right, int distance, const std::string &text) -> bool
            {
                sized_id += 1;
                return draw_vertical_centered_text(sc.canvas, left, right, distance, text, sized_id);
            }
        );
    }


    template <typename TLineFunction, typename TCoordFunction>
    int
    draw_single_axis_splits
    (
        const std::vector<int>& data,
        const ImVec2& mouse,
        Canvas* canvas,
        TLineFunction line_function,
        TCoordFunction coord_function
    )
    {
        const auto splits = calculate_all_splits(data);

        int ret = -1;

        int i = 0;
        for(auto s: splits)
        {
            line_function(s.position);
            const auto p = canvas->world_to_screen
            (
                ImVec2{static_cast<float>(s.position), static_cast<float>(s.position)}
            );
            if(is_close_to(coord_function(mouse), coord_function(p)))
            {
                ret = i;
            }
            i += 1;
        }

        return ret;
    }


    line_hover_data
    draw_splits(scalingsprite::ScalingSprite* sprite, Canvas* canvas, const scimed_config& scc)
    {
        const auto mouse = ImGui::GetMousePos();
        line_hover_data ret;

        ret.vertical_index = draw_single_axis_splits
        (
            sprite->cols,
            mouse,
            canvas,
            [&](int position)
            {
                canvas->vertical_line(core::c_int_to_float(position), scc.split_color);
            },
            [](const ImVec2 &p) -> float
            {
                return p.x;
            }
        );
        ret.horizontal_index = draw_single_axis_splits
        (
            sprite->rows,
            mouse,
            canvas,
            [&](int position)
            {
                canvas->horizontal_line(core::c_int_to_float(position), scc.split_color);
            },
            [](const ImVec2 &p) -> float
            {
                return p.y;
            }
        );

        return ret;
    }


    void
    set_mouse_cursor_from_hover(const line_hover_data& hover)
    {
        if(hover.horizontal_index != -1 || hover.vertical_index != -1)
        {
            if(hover.horizontal_index != -1 && hover.vertical_index != -1)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            }
            else if(hover.horizontal_index != -1)
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
    move_split(int split_index, std::vector<int>* data_ptr, float world_position)
    {
        if(split_index == -1)
        {
            return;
        }

        const auto split = calculate_all_splits(*data_ptr)[split_index];
        const auto position = static_cast<int>(world_position);
        const auto max = std::max(split.min_value + 1, position);
        const int line_x = std::min(split.max_value - 1, max);
        const int left_size = line_x - split.min_value;
        const int right_size = split.max_value - line_x;

        // move split to new position
        auto& data = *data_ptr;
        data[split_index] = get_sign(data[split_index]) * left_size;
        data[split_index + 1] = get_sign(data[split_index + 1]) * right_size;
    }


    void
    split_space_in_two(std::vector<int>* data_ptr, int index, int x)
    {
        auto& data = *data_ptr;
        const auto size = calculate_all_spaces(data)[index];
        const int mouse = std::min(size.right - 1, std::max(size.left + 1, x));
        ASSERT(mouse > size.left);

        const int left_abs_val = mouse - size.left;
        const int old_value = data[index];
        const int sign = get_sign(old_value);
        const int left_val = left_abs_val * sign;
        data[index] = left_val;

        const int right_val = sign * (std::abs(old_value) - left_abs_val);
        data.insert(data.begin() + index + 1, right_val);
    }


    bool
    scimed::run(const CanvasConfig& cc, const scimed_config& scc)
    {
        canvas.begin(cc);
        canvas.show_grid(cc);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        if(!texture)
        {
            canvas.show_ruler(cc);
            canvas.end(cc);
            return false;
        }

        // draw texture
        ImTextureID tex_id = imgui::c_texture_to_imgui(texture.get()); // NOLINT: auto is preferred but a texture is a hidden pointer
        const auto pos = canvas.world_to_screen(ImVec2{0, 0});
        const auto size = canvas.world_to_screen
        (
            ImVec2{static_cast<float>(texture->width), static_cast<float>(texture->height)}
        );
        draw_list->AddImage(tex_id, pos, size);

        const auto current_hover = draw_splits(scaling.get(), &canvas, scc);
        draw_sizer(texture, *this, scc, scaling.get());

        canvas.show_ruler(cc);
        canvas.end(cc);

        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            hover = current_hover;
        }

        const bool is_tracking = hover.horizontal_index != -1 || hover.vertical_index != -1;

        if(is_tracking)
        {
            if(ImGui::IsMouseDown(0))
            {
                set_mouse_cursor_from_hover(hover);

                const auto me = canvas.screen_to_world(ImGui::GetMousePos());

                move_split(hover.horizontal_index, &scaling->rows, me.y);
                move_split(hover.vertical_index, &scaling->cols, me.x);
            }
            else if(ImGui::IsItemHovered())
            {
                set_mouse_cursor_from_hover(current_hover);
                hover = line_hover_data {};
            }
        }
        else if(ImGui::IsItemHovered())
        {
            set_mouse_cursor_from_hover(current_hover);
        }

        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {
            ImGui::OpenPopup("asd");
            const auto w = canvas.screen_to_world(ImGui::GetMousePos());
            mouse_popup = Vec2i {static_cast<int>(w.x), static_cast<int>(w.y)};
        }

        if(ImGui::BeginPopup("asd"))
        {
            const auto space_index_y = find_space_index_or_null(scaling->rows, mouse_popup.y, texture->height);
            const auto space_index_x = find_space_index_or_null(scaling->cols, mouse_popup.x, texture->width);

            constexpr auto label_x = ICON_MDI_VIEW_SPLIT_HORIZONTAL " New Horizontal divider";
            constexpr auto label_y = ICON_MDI_VIEW_SPLIT_VERTICAL " New Vertical divider";

            if (window::imgui::selectable_or_disabled(space_index_y.has_value(), label_x))
            {
                split_space_in_two(&scaling->rows, space_index_y.value(), mouse_popup.y);
            }

            if(window::imgui::selectable_or_disabled(space_index_x.has_value(), label_y))
            {
                split_space_in_two(&scaling->cols, space_index_x.value(), mouse_popup.x);
            }
            ImGui::EndPopup();
        }

        return false;
    }
}
