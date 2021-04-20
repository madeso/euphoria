#include "window/imgui_ext.h"


#include "core/angle.h"
#include "core/assert.h"
#include "core/rgb.h"

#include "imgui/imgui.h"

#include "render/texture.h"

#include "window/imgui_icons.h"


namespace euphoria::window
{
    bool
    InputText(const char* label, std::string* str)
    {
        constexpr int size = 500;
        char buffer[size] = { 0, };

        strncpy(buffer, str->c_str(), size);
        const bool r = ImGui::InputText(label, buffer, size);
        if(r)
        {
            *str = buffer;
        }

        return r;
    }


    void
    HelpMarker(const char* desc)
    {
        ImGui::SameLine();
        ImGui::TextDisabled( ICON_MDI_HELP_CIRCLE );
        HelpText(desc);
    }


    void
    HelpText(const char* desc)
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }


    void
    ImguiLabel(const std::string& str)
    {
        ImGui::Text("%s", str.c_str());
    }


    bool
    ImguiAngleSlider
    (
        const char* const name,
        core::Angle* angle,
        const core::Angle& mindeg,
        const core::Angle& maxdeg
    )
    {
        ASSERT(angle);

        float degrees = angle->InDegrees();
        const auto value_was_changed = ImGui::SliderFloat
        (
            name,
            &degrees,
            mindeg.InDegrees(),
            maxdeg.InDegrees()
        );

        if(value_was_changed)
        {
            *angle = core::Angle::FromDegrees(degrees);
        }

        return value_was_changed;
    }


    bool
    ToggleButton(const char* const label, bool down, const ImVec2& size)
    {
        if (down)
        {
            const auto c = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
            ImGui::PushStyleColor(ImGuiCol_Button, c);
            const auto r = ImGui::Button(label, size);
            ImGui::PopStyleColor(1);

            return r;
        }
        else
        {
            return ImGui::Button(label, size);
        }
    }

    bool
    ImGuiColorEdit(const char* const name, core::rgb* c)
    {
        return ImGui::ColorEdit3(name, &c->r);
    }

    bool
    ImGuiColorEdit(const char* const name, core::rgba* c)
    {
        return ImGui::ColorEdit4(name, &c->r);
    }

    bool
    ImGuiColorEdit(const char* const name, core::rgbai* c)
    {
        auto cc = crgba(*c);
        const auto changed = ImGui::ColorEdit4(name, &cc.r);
        if(changed)
        {
            *c = crgbai(cc);
        }
        return changed;
    }


    void
    ImguiImage(render::texture2d* texture)
    {
        auto        tex_w  = static_cast<float>(texture->get_width());
        auto        tex_h  = static_cast<float>(texture->get_height());
        ImTextureID tex_id = reinterpret_cast<ImTextureID>(texture->get_id());

        ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
        ImGui::Text("%.0fx%.0f", tex_w, tex_h);
        ImGui::Image(
                tex_id,
                ImVec2(tex_w, tex_h),
                ImVec2(0, 0),
                ImVec2(1, 1),
                ImColor(255, 255, 255, 255),
                ImColor(255, 255, 255, 128));
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            float focus_sz = 32.0f;
            float focus_x  = ImGui::GetMousePos().x - tex_screen_pos.x
                            - focus_sz * 0.5f;
            if(focus_x < 0.0f)
            {
                focus_x = 0.0f;
            }
            else if(focus_x > tex_w - focus_sz)
            {
                focus_x = tex_w - focus_sz;
            }
            float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y
                            - focus_sz * 0.5f;
            if(focus_y < 0.0f)
            {
                focus_y = 0.0f;
            }
            else if(focus_y > tex_h - focus_sz)
            {
                focus_y = tex_h - focus_sz;
            }
            ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
            ImGui::Text(
                    "Max: (%.2f, %.2f)",
                    focus_x + focus_sz,
                    focus_y + focus_sz);
            ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
            ImVec2 uv1 = ImVec2(
                    (focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
            ImGui::Image(
                    tex_id,
                    ImVec2(128, 128),
                    uv0,
                    uv1,
                    ImColor(255, 255, 255, 255),
                    ImColor(255, 255, 255, 128));
            ImGui::EndTooltip();
        }
    }


    // stolen from ShowExampleAppFixedOverlay function in imgui_demo
    bool
    BeginFixedOverlay
    (
        ImguiCorner corner,
        const std::string& title,
        float a_distance,
        float a_distance_y
    )
    {
        const int   corner_int = static_cast<int>(corner);
        // const float distance   = 10.0f;
        const auto distance_x = a_distance;
        const auto distance_y = a_distance_y > 0 ? a_distance_y : a_distance;
        const auto size = ImGui::GetIO().DisplaySize;
        const ImVec2 window_pos = corner == ImguiCorner::Center
            ? ImVec2(size.x / 2, size.y / 2)
            : ImVec2
            (
                (corner_int & 1)
                    ? size.x - distance_x
                    : distance_x,
                (corner_int & 2)
                    ? size.y - distance_y
                    : distance_y
            );
        const ImVec2 window_pos_pivot = corner == ImguiCorner::Center
            ? ImVec2(0.5f, 0.5f)
            : ImVec2
            (
                (corner_int & 1) ? 1.0f : 0.0f,
                (corner_int & 2) ? 1.0f : 0.0f
            );
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

        return ImGui::Begin(
                title.c_str(),
                nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_AlwaysAutoResize
                        | ImGuiWindowFlags_NoMove
                        | ImGuiWindowFlags_NoSavedSettings);
    }


    ImVec2
    C(const core::vec2f& v)
    {
        return ImVec2 {v.x, v.y};
    }


    core::vec2f
    C(const ImVec2& v)
    {
        return core::vec2f {v.x, v.y};
    }


    // from https://github.com/ocornut/imgui/issues/211
    ImguiDisabled::ImguiDisabled()
    {
        // ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(
                ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }


    ImguiDisabled::~ImguiDisabled()
    {
        // ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }


    bool
    ImguiSelectableOrDisabled(bool enabled, const char* label)
    {
        if(enabled)
        {
            if(ImGui::Selectable(label))
            {
                return true;
            }
        }
        else
        {
            ImguiDisabled disabled;
            ImGui::TextUnformatted(label);
        }

        return false;
    }
}  // namespace euphoria::window
