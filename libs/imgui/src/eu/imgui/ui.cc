#include "eu/imgui/ui.h"

#include "eu/core/ui.h"
#include "eu/core/scurve.h"

#include "eu/render/texture.h"
#include "eu/render/dependency_glad.h"

#include <iostream>
#include <numbers>
#include <cmath>

#include "dear_imgui/imgui.h"
#include "dear_imgui/misc/cpp/imgui_stdlib.h"
#include "dear_imgui/imgui_internal.h"

namespace eu::imgui
{
    // opengl code copied from the imgui opengl3 backend with minor modifications
    // todo(Gustav): should we use the imgui backend code for "backend" rendering or use our own shader?
    
    namespace
    {
        bool check_imgui_shader(GLuint handle, const char* desc)
        {
            GLint status = 0;
            GLint log_length = 0;

            glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
            if (status == GL_FALSE)
            {
                std::cerr << "ERROR: Shader code: failed to compile " << desc << "!\n";
            }

            if (log_length > 1)
            {
                std::vector<GLchar> buf;
                buf.resize(sizet_from_int(log_length + 1));
                glGetShaderInfoLog(handle, log_length, nullptr, buf.data());
                std::cerr << buf.data();
            }
            return status == GL_TRUE;
        }

        bool imgui_check_program(GLuint handle, const char* desc)
        {
            GLint status = 0;
            GLint log_length = 0;
            glGetProgramiv(handle, GL_LINK_STATUS, &status);
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
            if (status == GL_FALSE)
            {
                std::cerr << "ERROR: Shader program: failed to link " << desc << "!\n";
            }

            if (log_length > 1)
            {
                std::vector<GLchar> buf;
                buf.resize(sizet_from_int(log_length + 1));
                glGetProgramInfoLog(handle, log_length, nullptr, buf.data());
                std::cerr << buf.data();
            }
            return status == GL_TRUE;
        }

        void imgui_destroy_shader(ImguiShaderProgram* bd)
        {
            if (bd->program_handle == 0)
            {
                return;
            }

            glDeleteProgram(bd->program_handle);
            bd->program_handle = 0;
        }

        ImguiShaderProgram imgui_load_shader(const char* glsl_version_string, const char* vertex_shader, const char* fragment_shader)
        {
            // Create shaders
            const GLchar* const vertex_shader_with_version[2] = { glsl_version_string, vertex_shader };
            const auto vert_handle = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vert_handle, 2, vertex_shader_with_version, nullptr);
            glCompileShader(vert_handle);
            if (!check_imgui_shader(vert_handle, "vertex shader"))
            {
                return {};
            }

            const GLchar* const fragment_shader_with_version[2] = { glsl_version_string, fragment_shader };
            const auto frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(frag_handle, 2, fragment_shader_with_version, nullptr);
            glCompileShader(frag_handle);

            if (!check_imgui_shader(frag_handle, "fragment shader"))
            {
                return {};
            }

            // Link
            ImguiShaderProgram prog;
            prog.program_handle = glCreateProgram();
            glAttachShader(prog.program_handle, vert_handle);
            glAttachShader(prog.program_handle, frag_handle);
            glLinkProgram(prog.program_handle);
            if (!imgui_check_program(prog.program_handle, "shader program"))
            {
                imgui_destroy_shader(&prog);
                return {};
            }

            glDetachShader(prog.program_handle, vert_handle);
            glDetachShader(prog.program_handle, frag_handle);
            glDeleteShader(vert_handle);
            glDeleteShader(frag_handle);

            prog.texture_attrib = glGetUniformLocation(prog.program_handle, "Texture");
            prog.projection_attrib = glGetUniformLocation(prog.program_handle, "ProjMtx");
            return prog;
        }
    
        void imgui_set_shared_shader_params(ImguiShaderProgram* prog)
        {
            const ImDrawData* draw_data = ImGui::GetDrawData();

            const auto le = draw_data->DisplayPos.x;
            const auto ri = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
            const auto to = draw_data->DisplayPos.y;
            const auto bo = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

            const float ortho_projection[4][4] = {
                {2.0f / (ri - le), 0.0f, 0.0f, 0.0f},
                {0.0f, 2.0f / (to - bo), 0.0f, 0.0f},
                {0.0f, 0.0f, -1.0f, 0.0f},
                {(ri + le) / (le - ri), (to + bo) / (bo - to), 0.0f, 1.0f},
            };
            glUseProgram(prog->program_handle);
            glUniform1i(prog->texture_attrib, 0);
            glUniformMatrix4fv(prog->projection_attrib, 1, GL_FALSE, &ortho_projection[0][0]);
        }

        void im_draw_callback_linear_to_gamma(const ImDrawList*, const ImDrawCmd* cmd)
        {
            auto* prog = static_cast<ImguiShaderProgram*>(cmd->UserCallbackData);

            imgui_set_shared_shader_params(prog);
        }

        void im_draw_callback_depth_ortho(const ImDrawList*, const ImDrawCmd* cmd)
        {
            auto* prog = static_cast<ImguiShaderProgram*>(cmd->UserCallbackData);

            imgui_set_shared_shader_params(prog);
        }
    
        void draw_imgui_image(const render::FrameBuffer& img, const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& border_col, ImguiShaderCache* cache, ImageShader shader)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            // set shader
            switch (shader)
            {
            case ImageShader::tonemap_and_gamma:
                draw_list->AddCallback(im_draw_callback_linear_to_gamma, &cache->linear_to_gamma_shader);
                break;
            case ImageShader::depth_ortho:
                draw_list->AddCallback(im_draw_callback_depth_ortho, &cache->depth_ortho_shader);
                break;
            case ImageShader::none:
                // no shader
                break;
            }

            // draw image
            ImGui::ImageWithBg(img.id, image_size, uv0, uv1, border_col);

            // reset shader
            if (shader != ImageShader::none)
            {
                draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
            }
        }

        v2 v2_from_vec(const ImVec2& v)
        {
            return { v.x, v.y };
        }


        void image_tooltip(const render::FrameBuffer& texture_id, const ImVec2 texture_size, const float& region_size, const float& hover_size,
            const ImVec2 mouse_pos, const ImVec2 widget_size, const ImVec2 pos, const ImVec4 border_col, ImguiShaderCache* cache, ImageShader shader)
        {
            const auto region = core::calculate_region(v2_from_vec(mouse_pos), v2_from_vec(pos), v2_from_vec(texture_size), v2_from_vec(widget_size), region_size);
            const auto flipped_region_y = texture_size.y - region.y;
            const auto uv0 = ImVec2{ region.x / texture_size.x, flipped_region_y / texture_size.y };
            const auto uv1 = ImVec2{ (region.x + region_size) / texture_size.x, (flipped_region_y - region_size) / texture_size.y };

            // todo(Gustav): can we display pixel value instead of where we are looking? is the region important information?
            imgui_text(fmt::format("UL: {} {}", region.x, region.y));
            imgui_text(fmt::format("LR: {} {}", region.x + region_size, region.y + region_size));
            draw_imgui_image(texture_id, ImVec2(hover_size, hover_size), uv0, uv1, border_col, cache, shader);
        }
    
        float length2(const ImVec2& v)
        {
            return v.x * v.x + v.y * v.y;
        }

        float length(const ImVec2& v)
        {
            return std::sqrt(v.x * v.x + v.y * v.y);
        }

        ImVec2 normalize(const ImVec2& v, float len)
        {
            return v / len;
        }

        ImVec2 normalize(const ImVec2& v)
        {
            return normalize(v, length(v));
        }

        float dot(const ImVec2& lhs, const ImVec2& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        bool is_positive(float f)
        {
            return f >= 0.0f;
        }

        struct GearState
        {
            ImGuiID id;
            ImVec2 center;
            ImVec2 pos;
            int turns;
            float orig;
            std::optional<float> initial_angle;
        };

        // dl functions are copied directly from ImDrawList with an additional starting angle and "special cases" removed.

        void dl_path_arc_to_n(ImDrawList* dl, const ImVec2& center, float radius, float a_min, float a_max, int num_segments, float ang)
        {
            if (radius < 0.5f)
            {
                dl->_Path.push_back(center);
                return;
            }

            // Note that we are adding a point at both a_min and a_max.
            // If you are trying to draw a full closed circle you don't want the overlapping points!
            dl->_Path.reserve(dl->_Path.Size + (num_segments + 1));
            for (int segment_index = 0; segment_index <= num_segments; segment_index++)
            {
                const float a = a_min + (static_cast<float>(segment_index) / static_cast<float>(num_segments)) * (a_max - a_min);
                dl->_Path.push_back(ImVec2(center.x + ImCos(a + ang) * radius, center.y + ImSin(a + ang) * radius));
            }
        }

        void dl_path_arc_to(ImDrawList* dl, const ImVec2& center, float radius, float a_min, float a_max, int num_segments, float ang)
        {
            if (radius < 0.5f)
            {
                dl->_Path.push_back(center);
                return;
            }

            dl_path_arc_to_n(dl, center, radius, a_min, a_max, num_segments, ang);
        }

        void dl_add_circle(ImDrawList* dl, const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness, float ang)
        {
            if ((col & IM_COL32_A_MASK) == 0 || radius < 0.5f)
            {
                return;
            }

            // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
            num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);

            // Because we are filling a closed shape we remove 1 from the count of segments/points
            const float a_max = (IM_PI * 2.0f) * (static_cast<float>(num_segments) - 1.0f) / static_cast<float>(num_segments);
            dl_path_arc_to(dl, center, radius - 0.5f, 0.0f, a_max, num_segments - 1, ang);

            dl->PathStroke(col, ImDrawFlags_Closed, thickness);
        }


        ImVec2 calc_button_size(const ImVec2& label_size, const ImVec2& size_arg = ImVec2(0, 0))
        {
            const auto style = ImGui::GetStyle();
            const auto size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
            return size;
        }

        // todo(Gustav): remove
        // ImVec2 calc_button_size(const char* label, const ImVec2& size_arg = ImVec2(0, 0))
        // {
        //     return calc_button_size(ImGui::CalcTextSize(label, nullptr, true), size_arg);
        // }

        // https://anttweakbar.sourceforge.io/doc/tools_anttweakbar_rotoslider.html
        bool gear_functionality(float* drag)
        {
            static std::optional<GearState> state = std::nullopt;

            // config
            const auto circle_col = ImColor(100, 100, 255);
            const float min_radius = 20.0f;
            const float radius = min_radius;
            const float one_turn = 10.0f;
            constexpr float circle_thickness = 1.0f;
            constexpr int gear_segments = 6;

            const auto mp = ImGui::GetMousePos();

            const auto id = ImGui::GetItemID();
            const auto active = ImGui::IsItemActive();

            if (active && state && state->id != id)
            {
                // another id is active, but so are we => treat it as the previous lost the activity
                state = std::nullopt;
            }

            auto orig = state ? state->orig : *drag;
            const auto center = state.has_value() ? state->center : mp;

            // interaction
            bool changed = false;
            if (state.has_value() == false || state->id == id)
            {
                // only change state if we are the interactive item or there is no interactive item
                if (active)
                {
                    int turns = state ? state->turns : 0;
                    const auto input = mp - center;
                    std::optional<float> initial_angle = state ? state->initial_angle : std::nullopt;
                    if (length2(input) > (min_radius * min_radius))
                    {
                        const auto dir_cur = normalize(input);
                        const auto ang_right = std::acos(dot(dir_cur, { 1, 0 })) * (180.0f / std::numbers::pi_v<float>);
                        const auto ang = dir_cur.y < 0 ? ang_right : 360 - ang_right;

                        if (initial_angle.has_value() == false)
                        {
                            initial_angle = ang;
                        }

                        if (state.has_value())
                        {
                            const auto changed_y = is_positive(input.y) != is_positive(state->pos.y);
                            const auto on_right_side = input.x > 0 && state->pos.x > 0;
                            const auto changed_dir = changed_y && on_right_side;
                            if (changed_dir)
                            {
                                turns += is_positive(input.y) ? -1 : 1;
                            }
                        }
                        const auto new_ang = ang - *initial_angle + static_cast<float>(turns) * 360.0f;
                        const auto val = orig - (new_ang / 360.0f) * one_turn;

                        if (drag != nullptr)
                        {
                            *drag = val;
                            changed = true;
                        }
                    }
                    else
                    {
                        initial_angle = std::nullopt;
                        turns = 0;
                        orig = *drag;
                    }

                    state = GearState
                    {
                        .id = id,
                        .center = center,
                        .pos = input,
                        .turns = turns,
                        .orig = orig,
                        .initial_angle = initial_angle
                    };
                }
                else
                {
                    state = std::nullopt;
                }
            }

            // drawing
            {
                // draw gear
                if (active)
                {
                    auto* fg = ImGui::GetForegroundDrawList();
                    // fg->AddCircle(center, radius, circle_col, gear_segments, thickness);
                    constexpr auto segment_factor = 1.0f / static_cast<float>(gear_segments - 1);
                    const auto turn_factor = std::fmodf(*drag, one_turn) / one_turn;
                    const auto ang = (turn_factor + segment_factor) * -2.0f * IM_PI;
                    dl_add_circle(fg, center, radius, circle_col, gear_segments, circle_thickness, ang);

                    // draw wrench instead?
                    fg->AddLine(center, mp, circle_col);
                }
            }

            return changed;
        }

        struct CompoundWidget
        {
            const char* label;
            int item_count;

            explicit CompoundWidget(const char* l, int ic = 3)
                : label(l)
                , item_count(ic)
            {
                ImGui::BeginGroup();
                ImGui::PushID(label);
            }

            void setup(float extra_x = -1.0f)
            {
                auto& style = ImGui::GetStyle();
                const float w_items = ImMax(0.0f, ImGui::CalcItemWidth() - style.ItemInnerSpacing.x * (item_count*2.0f - 1.0f) - extra_x * item_count);
                for (int item_index = 0; item_index < item_count; item_index += 1)
                {
                    if (extra_x > -0.5f)
                    {
                        ImGui::PushItemWidth(extra_x);
                    }
                    ImGui::PushItemWidth(w_items / static_cast<float>(item_count));
                }
            }

            void begin_widget(int i)
            {
                ImGui::PushID(i);

                if (i > 0)
                {
                    auto& style = ImGui::GetStyle();
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);
                }
            }

            void next_sub_widget()
            {
                auto& style = ImGui::GetStyle();
                ImGui::PopItemWidth();
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
            }

            void end_widget()
            {
                ImGui::PopItemWidth();
                ImGui::PopID();
            }

            ~CompoundWidget()
            {
                auto& style = ImGui::GetStyle();

                ImGui::PopID();

                const char* label_end = ImGui::FindRenderedTextEnd(label);
                if (label != label_end)
                {
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);
                    ImGui::TextEx(label, label_end);
                }

                ImGui::EndGroup();
            }

            CompoundWidget(const CompoundWidget&) = delete;
            CompoundWidget(CompoundWidget&&) = delete;
            void operator=(const CompoundWidget&) = delete;
            void operator=(CompoundWidget&&) = delete;
        };

        constexpr float c(int r)
        {
            return static_cast<float>(r) * (1.0f / 255.0f);
        }

        constexpr ImVec4 rgb(int r, int g, int b, float a = 1.0f)
        {
            return { c(r), c(g), c(b), a};
        }

        namespace colors
        {
            constexpr auto base = rgb(25, 28, 33);
            constexpr auto tab = rgb(29, 32, 37);
            constexpr auto tab_other = rgb(25, 28, 33);
            constexpr auto edit_bg = rgb(11, 14, 19);
            constexpr auto button_bg = rgb(33, 37, 43);
            constexpr auto header_bg = rgb(29, 32, 37);
            constexpr auto button_hover = rgb(42, 48, 56);
            constexpr auto button_active = rgb(31, 45, 24);
            constexpr auto border = rgb(66, 73, 54, 0.3f);
            constexpr auto lime = rgb(158, 233, 57);
            constexpr auto scroll_bg = rgb(16, 20, 24);
            constexpr auto scroll_grab = rgb(63, 69, 81);
            constexpr auto scroll_hover = rgb(39, 43, 51);
            constexpr auto scroll_active = rgb(132, 204, 22);
            constexpr auto text_selected_bg = rgb(31, 45, 24);
            constexpr auto text = rgb(225, 226, 233);
            constexpr auto text_disabled = rgb(113, 113, 122);
            constexpr auto transparent = rgb(0, 0, 0, 0.0f);

            constexpr ImVec4 text_label = rgb(139, 148, 125);
            constexpr ImVec4 button_active_border = rgb(158, 233, 5);
            constexpr ImVec4 button_default_border = rgb(48, 53, 64);

            constexpr auto graph_background = rgb(11, 14, 19);
            constexpr auto graph_major_line = rgb(39, 43, 51);
            constexpr auto graph_minor_line = rgb(21, 25, 33);

            constexpr auto graph_line = rgb(132, 204, 22);
            constexpr auto graph_alt_line = rgb(158, 233, 57);

            constexpr auto graph_point_idle = rgb(39, 43, 51);
            constexpr auto graph_point_idle_border = rgb(63, 63, 70);

            constexpr auto graph_active_point = rgb(158, 233, 57);
            constexpr auto graph_active_point_middle = rgb(255, 255, 255);
            constexpr auto graph_active_point_border = rgb(92, 131, 46);

            constexpr auto graph_outline_point = rgb(190, 242, 100);
            constexpr auto graph_outline_point_border = rgb(16, 20, 24);
        }
    }
}



namespace eu::imgui
{
constexpr auto dear_imgui_shader_version = "#version 330 core\n";

constexpr auto linear_to_gamma_glsl_vert = R"glsl(
layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;
uniform mat4 ProjMtx;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main()
{
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)glsl";

constexpr auto linear_to_gamma_glsl_frag = R"glsl(
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
layout (location = 0) out vec4 Out_Color;
void main()
{
    vec4 sample = texture(Texture, Frag_UV.st);
	vec3 color = sample.rgb / (sample.rgb + vec3(1.0f)); // reinhard tone mapping
	float gamma = 2.2f;
	vec3 gamma_corrected = pow(color.rgb, vec3(1.0f/gamma));
    Out_Color = Frag_Color * vec4(gamma_corrected, sample.a);
}
)glsl";


constexpr auto depth_ortho_glsl_vert = R"glsl(
layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;
uniform mat4 ProjMtx;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main()
{
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)glsl";

constexpr auto depth_ortho_glsl_frag = R"glsl(
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
layout (location = 0) out vec4 Out_Color;

void main()
{
    vec4 sample = texture(Texture, Frag_UV.st);
	Out_Color = Frag_Color * vec4(vec3(sample.r), 1);
}
)glsl";


ImguiShaderCache::ImguiShaderCache()
	: linear_to_gamma_shader(imgui_load_shader(dear_imgui_shader_version,
		linear_to_gamma_glsl_vert, linear_to_gamma_glsl_frag))
	, depth_ortho_shader(imgui_load_shader(dear_imgui_shader_version,
		depth_ortho_glsl_vert, depth_ortho_glsl_frag)
	  )
{
}

ImguiShaderCache::~ImguiShaderCache()
{
	imgui_destroy_shader(&linear_to_gamma_shader);
	imgui_destroy_shader(&depth_ortho_shader);
}

void setup_colors()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // initially inspired and based on by Catppuccin Mocha Palette
    // https://github.com/ocornut/imgui/issues/707#issuecomment-3592676777
    
    constexpr auto base = colors::base;
    constexpr auto tab = colors::tab;
    constexpr auto tab_other = colors::tab_other;
    constexpr auto edit_bg = colors::edit_bg;
    constexpr auto button_bg = colors::button_bg;
    constexpr auto header_bg = colors::header_bg;
    constexpr auto button_hover = colors::button_hover;
    constexpr auto button_active = colors::button_active;
    constexpr auto border = colors::border;
    constexpr auto lime = colors::lime;
    constexpr auto scroll_bg = colors::scroll_bg;
    constexpr auto scroll_grab = colors::scroll_grab;
    constexpr auto scroll_hover = colors::scroll_hover;
    constexpr auto scroll_active = colors::scroll_active;
    constexpr auto text_selected_bg = colors::text_selected_bg;
    constexpr auto text = colors::text;
    constexpr auto text_disabled = colors::text_disabled;
    constexpr auto transparent = colors::transparent;
    constexpr auto table_border_strong = colors::text_label;

    // Main window and backgrounds
    colors[ImGuiCol_Text] = text;
    colors[ImGuiCol_TextDisabled] = text_disabled;
    colors[ImGuiCol_WindowBg] = base;                      // Background of normal windows
    colors[ImGuiCol_ChildBg] = base;                       // Background of child windows
    colors[ImGuiCol_PopupBg] = base;                   // Background of popups, menus, tooltips windows
    colors[ImGuiCol_Border] = border;
    colors[ImGuiCol_BorderShadow] = transparent;
    colors[ImGuiCol_FrameBg] = edit_bg;                   // Background of checkbox, radio button, plot, slider, text input
    colors[ImGuiCol_FrameBgHovered] = edit_bg;
    colors[ImGuiCol_FrameBgActive] = edit_bg;
    colors[ImGuiCol_TitleBg] = base;                     // Title bar
    colors[ImGuiCol_TitleBgActive] = base;             // Title bar when focused
    colors[ImGuiCol_TitleBgCollapsed] = base;            // Title bar when collapsed
    colors[ImGuiCol_MenuBarBg] = base;
    colors[ImGuiCol_ScrollbarBg] = scroll_bg;
    colors[ImGuiCol_ScrollbarGrab] = scroll_grab;
    colors[ImGuiCol_ScrollbarGrabHovered] = scroll_hover;
    colors[ImGuiCol_ScrollbarGrabActive] = scroll_active;
    colors[ImGuiCol_CheckMark] = lime;                    // Checkbox tick and RadioButton circle
    colors[ImGuiCol_SliderGrab] = scroll_grab;
    colors[ImGuiCol_SliderGrabActive] = scroll_active;
    colors[ImGuiCol_Button] = button_bg;
    colors[ImGuiCol_ButtonHovered] = button_hover;
    colors[ImGuiCol_ButtonActive] = button_active;
    colors[ImGuiCol_Header] = header_bg;                    // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
    colors[ImGuiCol_HeaderHovered] = header_bg;
    colors[ImGuiCol_HeaderActive] = header_bg;
    colors[ImGuiCol_Separator] = border;
    colors[ImGuiCol_SeparatorHovered] = border;
    colors[ImGuiCol_SeparatorActive] = border;
    colors[ImGuiCol_ResizeGrip] = scroll_grab;                // Resize grip in lower-right and lower-left corners of windows.
    colors[ImGuiCol_ResizeGripHovered] = scroll_hover;
    colors[ImGuiCol_ResizeGripActive] = scroll_active;
    colors[ImGuiCol_InputTextCursor] = text;               // InputText cursor/caret
    colors[ImGuiCol_TabHovered] = tab;                 // Tab background, when hovered
    colors[ImGuiCol_Tab] = tab_other;                        // Tab background, when tab-bar is focused & tab is unselected
    colors[ImGuiCol_TabSelected] = tab;                // Tab background, when tab-bar is focused & tab is selected
    colors[ImGuiCol_TabSelectedOverline] = lime;        // Tab horizontal overline, when tab-bar is focused & tab is selected
    colors[ImGuiCol_TabDimmed] = tab;                  // Tab background, when tab-bar is unfocused & tab is unselected
    colors[ImGuiCol_TabDimmedSelected] = tab;          // Tab background, when tab-bar is unfocused & tab is selected
    colors[ImGuiCol_TabDimmedSelectedOverline] = lime;  // horizontal overline, when tab-bar is unfocused & tab is selected
    // colors[ImGuiCol_DockingPreview] = sapphire;
    // colors[ImGuiCol_DockingEmptyBg] = base;
    colors[ImGuiCol_PlotLines] = lime;
    colors[ImGuiCol_PlotLinesHovered] = lime;
    colors[ImGuiCol_PlotHistogram] = lime;
    colors[ImGuiCol_PlotHistogramHovered] = lime;
    colors[ImGuiCol_TableHeaderBg] = base;
    colors[ImGuiCol_TableBorderStrong] = table_border_strong;
    colors[ImGuiCol_TableBorderLight] = base;
    colors[ImGuiCol_TableRowBg] = transparent;
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = text_selected_bg;
    colors[ImGuiCol_DragDropTarget] = lime;
    colors[ImGuiCol_UnsavedMarker] = lime;         // Unsaved Document marker (in window title and tabs)
    colors[ImGuiCol_NavCursor] = lime;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);

    // Rounded corners
    style.WindowRounding = 6.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    // Padding and spacing
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 21.0f;
    style.ScrollbarSize = 18.0f;
    style.GrabMinSize = 10.0f;

    // Borders
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.TabBorderSize = 1.0f;
}


void label(const char* label)
{
    ImGui::PushStyleColor(ImGuiCol_Text, colors::text_label);
    ImGui::TextUnformatted(label);
    const auto used_size = ImGui::GetItemRectSize();
    const ImVec2 text_size = ImGui::CalcTextSize(label);

    if (used_size.x < text_size.x ||
        used_size.y < text_size.y )
    {
        ImGui::SetTooltip("%s", label);
    }

    ImGui::PopStyleColor();
}

void set_hover_cursor()
{
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
}

bool button(const char* label)
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 7.0f)); // todo(Gustav): make configurable

    const auto clicked = ImGui::Button(label);

    // todo(Gustav): when active, switch text color to lime

    {
        const auto min = ImGui::GetItemRectMin();
        const auto max = ImGui::GetItemRectMax();

        auto* draw = ImGui::GetWindowDrawList();
        draw->AddRect(min, max, ImColor{
            ImGui::IsItemHovered() ? colors::button_active_border : colors::button_default_border }, style.ChildRounding);
    }

    ImGui::PopStyleVar();

    set_hover_cursor();

    return clicked;
}

bool centered_button(const char* label)
{
    const auto window_width = ImGui::GetContentRegionAvail().x;
    const auto text_width = ImGui::CalcTextSize(label).x;
    const auto padding_hor = ImGui::GetStyle().FramePadding.x * 2; // padding on both sides
    const auto whitespace = window_width - text_width - padding_hor;
    const float x = whitespace * 0.5f;
    ImGui::SetCursorPosX(x);
    return button(label);
}


bool header(const char* label, bool* closeable)
{
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    const auto ret = closeable != nullptr
        ? ImGui::CollapsingHeader(label, closeable, flags)
        : ImGui::CollapsingHeader(label, flags);
    return ret;
}


void imgui_text(const std::string& str)
{
    ImGui::Text("%s", str.c_str());
}

void imgui_readonly(const char* label, const std::string& text)
{
    auto temp = text;
    // ImGuiCol_FrameBg
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::base);
    ImGui::PushStyleColor(ImGuiCol_Text, colors::text_label);
    ImGui::InputText(label, &temp, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleColor(2);
}

void imgui_readonly(const char* label, const v3& v)
{
    auto temp = v;
    ImGui::PushStyleColor(ImGuiCol_FrameBg, colors::base);
    ImGui::PushStyleColor(ImGuiCol_Text, colors::text_label);
    ImGui::InputFloat3(label, temp.get_data_ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleColor(2);
}

void imgui_readonly(const char* label, const Ypr& y)
{
    const auto temp = v3{ y.yaw.as_degrees() ,y.pitch.as_degrees(), y.roll.as_degrees() };
    imgui_readonly(label, temp);
}

bool imgui_edit(const char* label, std::string* str)
{
    return ImGui::InputText(label, str);
}

void imgui_image(const char* name, const render::FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader)
{
	const auto texture_size = ImVec2{float_from_int(img.size.width), float_from_int(img.size.height)};

	// todo(Gustav): make the arguments widget_size and zoom level AND make them configurable (with scrolling)
	static float widget_height = 100.0f;
	static float region_size = 32.0f;
	static float hover_size = 128.0f;
	const auto& io = ImGui::GetIO();

	imgui_text(fmt::format("{}: {}x{}", name, texture_size.x, texture_size.y));

	const auto widget_width = (texture_size.x / texture_size.y) * widget_height;
	const auto widget_size = ImVec2{widget_width, widget_height};

	const auto pos = ImGui::GetCursorScreenPos();
	constexpr auto uv_min = ImVec2{0.0f, 1.0f};	 // Top-left
	constexpr auto uv_max = ImVec2{1.0f, 0.0f};	 // Lower-right
	const auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	static ImVec2 latest_tooltip;
	static ImGuiID current_id = 0;

	const constexpr char* const popup_id = "image config popup";

	draw_imgui_image(img, widget_size, uv_min, uv_max, border_col, cache, shader);
	const auto id = ImGui::GetID(name);

	if (id == current_id && ImGui::BeginPopupContextItem(popup_id))
	{
		ImGui::DragFloat("Base", &widget_height, 1.0f);
		ImGui::DragFloat("Size", &region_size, 0.01f);
		ImGui::DragFloat("Scale", &hover_size, 1.0f);
		image_tooltip(img, texture_size, region_size, hover_size, latest_tooltip, widget_size, pos, border_col, cache, shader);
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::OpenPopupOnItemClick(popup_id, ImGuiPopupFlags_MouseButtonRight);

	// todo(Gustav): look into SetItemTooltip and BeginItemTooltip from https://github.com/ocornut/imgui/releases/tag/v1.89.7
	if (ImGui::IsItemHovered())
	{
		current_id = id;
		latest_tooltip = io.MousePos;
		if (ImGui::BeginTooltip())
		{
			image_tooltip(img, texture_size, region_size, hover_size, io.MousePos, widget_size, pos, border_col, cache, shader);
			ImGui::EndTooltip();
		}
	}
}

struct Drags
{
    float v_speed = 1.0f;
    float v_min = 0.0f;
    float v_max = 0.0f;
};

struct ButtonStateStyle
{
    ImVec4 background;
    ImVec4 text;
    ImVec4 border;
};
struct ButtonStyle
{
    ButtonStateStyle idle;
    ButtonStateStyle hover;
    ButtonStateStyle active;
};

bool styled_button(const ButtonStyle& style, const char* label, const std::optional<ImVec2>& button_size = std::nullopt)
{
    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    const ImVec2 size = button_size.value_or(calc_button_size(label_size));
    const auto pos = ImGui::GetCursorScreenPos();
    const auto ret = ImGui::InvisibleButton(label, size);
    const auto min = ImGui::GetItemRectMin();
    const auto max = ImGui::GetItemRectMax();

    const auto& imstyle = ImGui::GetStyle();
    const auto is_active = ImGui::IsItemActive();
    const auto is_hover = ImGui::IsItemHovered();
    auto* draw = ImGui::GetWindowDrawList();

    const auto& color = is_active ? style.active :
        (is_hover ? style.hover : style.idle);

    // background
    draw->AddRectFilled(min, max, ImColor{ color.background }, imstyle.ChildRounding);

    // label
    {
        const auto bb = ImRect{ pos, pos + size };
        ImGui::PushStyleColor(ImGuiCol_Text, color.text);
        ImGui::RenderTextClipped(bb.Min + imstyle.FramePadding, bb.Max - imstyle.FramePadding, label, nullptr, &label_size, imstyle.ButtonTextAlign, &bb);
        ImGui::PopStyleColor();
    }

    // border
    draw->AddRect(min, max, ImColor{color.border}, imstyle.ChildRounding);

    set_hover_cursor();

    return ret;
}

static ImVec2 calculate_square_button_size()
{
    const float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    const ImVec2 button_size = { line_height + 3.0f, line_height };
    return button_size;
}

static bool imgui_drag_with_reset(
    float* xval, float* yval, float* zval,
    float reset_value, std::optional<float> extra_spacing,
    const char* xlabel, const char* ylabel, const char* zlabel,
    const Drags& drags = Drags{}
)
{
    bool changed = false;

    const auto button_size = calculate_square_button_size();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
    const auto full_width = ImGui::CalcItemWidth();
    if (extra_spacing.has_value())
    {
        ImGui::PushItemWidth(*extra_spacing);
    }
    ImGui::PushMultiItemsWidths(3, full_width - 3 * button_size.x - extra_spacing.value_or(0.0f));

    constexpr auto red_style = ButtonStyle
    {
        .idle = {
            .background = rgb(76, 5, 25),
            .text = rgb(251, 113, 133),
            .border = rgb(159, 18, 57)
        },
        .hover = {
            .background = rgb(244, 63, 94),
            .text = rgb(11, 14, 19),
            .border = rgb(251, 113, 133)
        },
        .active = {
            .background = rgb(69, 36, 46),
            .text = rgb(251, 113, 133),
            .border = rgb(244, 63, 94)
        }
    };
    constexpr auto green_style = ButtonStyle
    {
        .idle = {
            .background = rgb(2, 44, 34),
            .text = rgb(158, 233, 57),
            .border = rgb(6, 95, 70)
        },
        .hover = {
            .background = rgb(158, 233, 57),
            .text = rgb(11, 14, 19),
            .border = rgb(190, 242, 100)
        },
        .active = {
            .background = rgb(52, 70, 38),
            .text = rgb(158, 233, 57),
            .border = rgb(158, 233, 57)
        }
    };
    constexpr auto blue_style = ButtonStyle
    {
        .idle = {
            .background = rgb(8, 47, 73),
            .text = rgb(56, 189, 248),
            .border = rgb(7, 89, 133)
        },
        .hover = {
            .background = rgb(14, 165, 233),
            .text = rgb(11, 14, 19),
            .border = rgb(56, 189, 248)
        },
        .active = {
            .background = rgb(23, 56, 74),
            .text = rgb(56, 189, 248),
            .border = rgb(14, 165, 233)
        }
    };

    const auto draw_component = [&button_size, reset_value, &changed, &drags](const ButtonStyle& style, const char* label, float* val, const char* id)
        {
            // ImGui::Button(label, button_size);
            const auto button_clicked = styled_button(style, label, button_size);

            if (gear_functionality(val))
            {
                changed = true;
            }

            if (button_clicked)
            {
                *val = reset_value;
                changed = true;
            }
            const auto min = ImGui::GetItemRectMin();
            ImGui::SameLine();
            const auto dragging = ImGui::DragFloat(id, val, drags.v_speed, drags.v_min, drags.v_max, "%.2f");
            if (dragging)
            {
                changed = true;
            }
            const auto max = ImGui::GetItemRectMax();

            if (dragging || ImGui::IsItemActive())
            {
                auto* draw = ImGui::GetWindowDrawList();
                draw->AddRect(min, max, ImColor{ style.active.border }, ImGui::GetStyle().ChildRounding);
            }

            ImGui::PopItemWidth();
        };

    draw_component(red_style, xlabel, xval, "##X");

    ImGui::SameLine();

    draw_component(green_style, ylabel, yval, "##Y");
    
    ImGui::SameLine();

    draw_component(blue_style, zlabel, zval, "##Z");

    ImGui::PopStyleVar();

    if (extra_spacing.has_value())
    {
        ImGui::SameLine();
    }

    return changed;
}

constexpr const char* default_format = "%.2f";

std::string format_float(float f)
{
    char text[32];
    ImFormatString(text, IM_ARRAYSIZE(text), default_format, static_cast<double>(f));
    return text;
}

float float_identity(float f)
{
    return f;
}

template<typename F>
bool flat_slider(const char* label, float* value, float min_value, float max_value, F display)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
    {
        return false;
    }

    const auto& style = ImGui::GetStyle();
    const auto id = window->GetID(label);

    const auto max_label_width = ImGui::CalcTextSize(format_float(display(max_value)).c_str()).x;
    const auto text = format_float(display(*value));

    const auto height = ImGui::GetFrameHeight();
    const auto radius = height * 0.3f;
    const auto ring_radius = height * 0.4f;

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total(pos, pos + ImVec2(ImGui::CalcItemWidth(), height));
    const ImRect track_bb(pos, ImVec2(total.Max.x - max_label_width - style.ItemInnerSpacing.x * 2, total.Max.y));

    ImGui::ItemSize(total, style.FramePadding.y);
    if (!ImGui::ItemAdd(total, id))
    {
        return false;
    }

    const bool is_hovering = ImGui::ItemHoverable(track_bb, id, ImGuiItemFlags_None);
    if ((is_hovering && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) || GImGui->NavActivateId == id)
    {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, radius * 2.0f);
    ImRect grab;
    const bool value_was_changed = ImGui::SliderBehavior(track_bb, id, ImGuiDataType_Float, value, &min_value, &max_value, default_format, 0, &grab);
    ImGui::PopStyleVar();
    if (value_was_changed)
    {
        ImGui::MarkItemEdited(id);
    }

    ImDrawList* draw = window->DrawList;
    const auto center_y = track_bb.GetCenter().y;
    const auto grab_center = grab.GetCenter().x;
    const auto is_active = GImGui->ActiveId == id;

    {
        const auto track_h = ImMax(2.0f, height * 0.2f);

        const auto left = track_bb.Min.x + radius;
        const auto right = track_bb.Max.x - radius;
        const auto top = center_y - track_h * 0.5f;
        const auto bottom = center_y + track_h * 0.5f;
        const auto half_circle_rounding = track_h * 0.5f;

        constexpr auto background_color = rgb(11, 14, 19);
        constexpr auto filled_color = rgb(132, 204, 22);

        const auto middle = std::max(left, grab_center);

        draw->AddRectFilled({ left, top }, { middle, bottom }, ImColor{filled_color}, half_circle_rounding);
        draw->AddRectFilled({ middle, top }, { right, bottom }, ImColor{background_color}, half_circle_rounding);
    }

    {
        constexpr auto default_color = rgb(225, 226, 233);
        constexpr auto hover_color = rgb(163, 230, 53);
        constexpr auto active_color = rgb(132, 204, 22);

        const auto grab_color = is_active ? active_color
            : is_hovering ? hover_color
            : default_color;

        const auto center = ImVec2{ grab_center, center_y };
        draw->AddCircleFilled(center, radius, ImColor{ grab_color });

        if (is_active || is_hovering)
        {
            constexpr auto line_thickness = 3.0f;
            constexpr auto ring_hover = rgb(190, 242, 100);
            constexpr auto ring_active = rgb(158, 233, 57);

            const auto ring_color = is_active ? ring_active : ring_hover;

            draw->AddCircle(center, ring_radius, ImColor{ ring_color }, 0, line_thickness);
        }
    }

    const auto text_size = ImGui::CalcTextSize(text.c_str());
    draw->AddText({ total.Max.x - text_size.x, center_y - text_size.y * 0.5f }, IM_COL32(170, 230, 60, 255), text.c_str());

    return value_was_changed;
}


bool simple_gamma_slider(const char* label, float* gamma, float curve, float min_gamma, float max_gamma)
{
    if (curve < 0.0f)
    {
        return flat_slider(label, gamma, min_gamma, max_gamma, float_identity);
    }

    // todo(Gustav): is this the correct way? it doesn't feel exactly right but perhaps that's just dear imgui
    const auto gamma_range = max_gamma - min_gamma;
    const auto t = (*gamma - min_gamma) / gamma_range;

    const auto backf = [min_gamma, gamma_range, curve](float f)
        {
            const auto perceptual = std::pow(f, curve);
            return min_gamma + perceptual * gamma_range;
        };

    auto slider_value = std::pow(t, 1.0f / curve);
    if (flat_slider(label, &slider_value, 0.0f, 1.0f, backf) == false)
    {
        return false;
    }

    *gamma = backf(slider_value);
    return true;
}

// -----
void row(const PropTable& t, const char* label)
{
    ASSERT(t);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    eu::imgui::label(label);
    ImGui::TableSetColumnIndex(1);
}

template<typename F>
bool trow(const PropTable& t, const char* label, const F& f)
{
    row(t, label);
    ImGui::PushID(label);
    ImGui::PushItemWidth(-std::numeric_limits<float>::min()); // place widget right edge on the edge of the table
    const auto ret = f();
    ImGui::PopItemWidth();
    ImGui::PopID();
    return ret;
}


PropTable::PropTable(PropTable&& r) noexcept
    : started(r.started)
{
    r.started = false;
}

PropTable PropTable::Begin(const char* label)
{
    constexpr ImGuiTableFlags table_flags = ImGuiTableFlags_SizingStretchProp;

    const auto started = ImGui::BeginTable(label, 2, table_flags);
    auto ret = PropTable{ started };
    if (started == false) return ret;

    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

    // Should we display header???
    ImGui::TableHeadersRow();

    return ret;
}

PropTable::~PropTable()
{
    if (started == false) return;
    ImGui::EndTable();
}

bool PropTable::check(const char* label, bool* v) const
{
    return trow(*this, label, [&]
        {
            return ImGui::Checkbox("", v);
        });
}

bool PropTable::slider(const char* label, float* v, float min, float max) const
{
    return trow(*this, label, [&]
        {
            const auto ret = flat_slider("", v, min, max, float_identity);
            return ret;
        });
}
bool PropTable::slider(const char* label, float* v) const
{
    return slider(label, v, 0.0f, 1.0f);
}

void PropTable::text(const char* label, const std::string& str) const
{
    trow(*this, label, [&]
        {
            eu::imgui::imgui_text(str);
            return false;
        });
}

bool PropTable::edit(const char* label, std::string* str) const
{
    return trow(*this, label, [&]
        {
            return eu::imgui::imgui_edit("", str);
        });
}

void PropTable::display(const char* label, bool b) const
{
    trow(*this, label, [&]
        {
            ImGui::TextUnformatted(b ? "TRUE" : "false");
            return false;
        });
}

void PropTable::readonly(const char* label, const std::string& text) const
{
    trow(*this, label, [&]
        {
            eu::imgui::imgui_readonly("", text);
            return false;
        });
}

void PropTable::readonly(const char* label, const eu::v3& v) const
{
    trow(*this, label, [&]
        {
            eu::imgui::imgui_readonly("", v);
            return false;
        });
}

void PropTable::readonly(const char* label, const eu::Ypr& y) const
{
    trow(*this, label, [&]
        {
            eu::imgui::imgui_readonly("", y);
            return false;
        });
}

void PropTable::image(const char* label, const render::FrameBuffer& img, ImguiShaderCache* cache, ImageShader shader) const
{
    trow(*this, label, [&]
        {
            imgui::imgui_image("", img, cache, shader);
            return false;
        });
}

bool PropTable::simple_gamma_slider(const char* label, float* gamma, float curve, float min_gamma, float max_gamma) const
{
    return trow(*this, label, [&]
        {
            return eu::imgui::simple_gamma_slider("", gamma, curve, min_gamma, max_gamma);
        });
}

bool PropTable::color(const char* label, Rgb* color) const
{
    return trow(*this, label, [&]
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            const auto button_size = calculate_square_button_size();
            // todo(Gustav): change so drag is 0-255
            // todo(Gustav): change colors?
            bool changed = imgui_drag_with_reset(&color->r, &color->g, &color->b, 0.0f, button_size.x,
                "R", "G", "B", Drags{ .v_speed = 0.1f, .v_min = 0.0f, .v_max = 1.0f }
                );
            constexpr const char* color_popup = "color-popup";
            if (ImGui::ColorButton("color", ImVec4{color->r, color->g, color->b, 1.0f}))
            {
                ImGui::OpenPopup(color_popup);
            }
            ImGui::PopStyleVar();
            if (ImGui::BeginPopup(color_popup))
            {
                // color dialog
                changed = ImGui::ColorPicker3(label, &color->r);
                ImGui::EndPopup();
            }
            return changed;
        });
}

bool PropTable::drag(const char* label, eu::v3* drag, float reset_value) const
{
    return trow(*this, label, [&]
        {
            return imgui_drag_with_reset(&drag->x, &drag->y, &drag->z, reset_value, std::nullopt, "X", "Y", "Z");
        });
}

bool PropTable::drag(const char* label, eu::Ypr* drag) const
{
    return trow(*this, label, [&]
        {
            float angles[3] = {
                drag->yaw.as_degrees(),
                drag->pitch.as_degrees(),
                drag->roll.as_degrees()
            };

            // const auto changed = ImGui::DragFloat3(label, angles, 1.0f, -360.0f, 360.0f);
            const auto changed = imgui_drag_with_reset(&angles[0], &angles[1], &angles[2], 0.0f, std::nullopt,
                    "Y", "P", "R", Drags{.v_speed = 1.0f, .v_min = -360.0f, .v_max = 360.0f }
                );

            if (changed)
            {
                drag->yaw = An::from_degrees(angles[0]);
                drag->pitch = An::from_degrees(angles[1]);
                drag->roll = An::from_degrees(angles[2]);
            }

            return changed;
        });
}

bool PropTable::drag(const char* label, float* drag, float speed) const
{
    return trow(*this, label, [&]
        {
            return ImGui::DragFloat("", drag, speed);
        });
}
bool PropTable::drag(const char* label, int* drag) const
{
    return trow(*this, label, [&]
        {
            return ImGui::DragInt("", drag);
        });
}

bool PropTable::drag(const char* label, eu::Rui* drag) const
{
    return trow(*this, label, [&]
        {
            return imgui_drag_with_reset(&drag->right, &drag->up, &drag->in, 0.0f, std::nullopt, "R", "U", "I");
        });
}

void PropTable::transform(const eu::m4& m) const
{
    const auto t = eu::transform_from_matrix(m);

    readonly("Position", t.position);
    readonly("Rotation", eu::Ypr::From(t.rotation));
    readonly("Scale", t.scale);
}

bool PropTable::transform(eu::m4* m) const
{
    auto t = eu::transform_from_matrix(*m);

    ImGui::PushID(m);

    bool changed = false;

    if (drag("Position", &t.position))
    {
        changed = true;
        *m = matrix_from_transform(t);
    }
    auto y = eu::Ypr::From(t.rotation);
    if (drag("Rotation", &y))
    {
        changed = true;
        t.rotation = eu::Q::From(y);
        *m = matrix_from_transform(t);
    }
    if (drag("Scale", &t.scale, 1.0f))
    {
        changed = true;
        *m = matrix_from_transform(t);
    }

    ImGui::PopID();

    return changed;
}

bool PropTable::scurve(const char* label, eu::core::SCurve* scurve, eu::imgui::SCurveGuiState* gui, eu::imgui::FlipX flip_x, const ImVec2& requested_widget_size) const
{
    return trow(*this, label, [&]
        {
            return eu::imgui::imgui_s_curve_editor("", scurve, gui, flip_x, requested_widget_size);
        });
}

PropTable::PropTable(bool b) : started(b) {}




SCurveGuiState::SCurveGuiState(float x, float y)
    : drag(x, y)
{}

bool imgui_s_curve_editor(const char* title, core::SCurve* curve, SCurveGuiState* gui, FlipX flip_x, const ImVec2& requested_widget_size)
{
    imgui_text(fmt::format("{} ({} {})", title, curve->slope, curve->threshold));

    ImGui::InvisibleButton(title, requested_widget_size);
    const auto widget_min = ImGui::GetItemRectMin();
    const auto widget_max = ImGui::GetItemRectMax();
    const auto widget_size = ImGui::GetItemRectSize();
    const auto is_active = ImGui::IsItemActive();
    const auto is_visible = ImGui::IsItemVisible();

    if (is_visible)
    {
        const auto new_curve = core::s_curve_from_input(flip_x == FlipX::yes ? 1 - gui->drag.x : gui->drag.x, gui->drag.y);
        *curve = new_curve;
    }

    auto& points = gui->point_cache;
    bool changed = false;

    auto* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(widget_min, widget_max, ImColor{ colors::graph_background });


    for (std::size_t point_index = 0; point_index < SCurveGuiState::num_points; point_index += 1)
    {
        const float srcx = static_cast<float>(point_index) / static_cast<float>(SCurveGuiState::num_points - 1); // -1 here to include 1.0f when evaluating the curve
        const float x = flip_x == FlipX::yes ? 1 - srcx : srcx;
        const float y = core::calculate_s_curve(srcx, curve->slope, curve->threshold);
        points[point_index] = ImVec2{ x * widget_size.x, (1 - y) * widget_size.y } + widget_min;
    }
    draw->AddPolyline(points.data(), int_from_sizet(SCurveGuiState::num_points), ImColor{colors::graph_line}, ImDrawFlags_None, 1.0f);

    const auto is_hover = ImGui::IsItemHovered();
    const auto drag_position = ImVec2{ gui->drag.x * widget_size.x, (1 - gui->drag.y) * widget_size.y } + widget_min;

    const auto drag_radius = is_hover ? 6.0f : 4.0f;
    constexpr auto drag_hover_step = 4.0f;
    draw->AddCircleFilled(drag_position, drag_radius, ImColor{is_hover ? colors::graph_active_point : colors::graph_outline_point });
    draw->AddCircle(drag_position, drag_radius, ImColor{ is_hover ? colors::graph_active_point_middle : colors::graph_outline_point_border }, 0, is_hover ? drag_hover_step : 1.0f);
    if (is_hover)
    {
        draw->AddCircle(drag_position, drag_radius + drag_hover_step/2.0f, ImColor{ colors::graph_active_point_border}, 0, 2.0f);
    }

    if (is_active)
    {
        // mouse is down and drag started inside the component
        const auto mp = ImGui::GetMousePos();
        const auto n_x = (mp.x - widget_min.x) / widget_size.x;
        const auto n_y = 1 - ((mp.y - widget_min.y) / widget_size.y);
        gui->drag.x = std::min(1.0f, std::max(n_x, 0.0f));
        gui->drag.y = std::min(1.0f, std::max(n_y, 0.0f));
        changed = true;
    }

    return changed;
}





}

