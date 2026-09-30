#include "eu/imgui/debug.h"

#include "imgui.h"

namespace eu::imgui
{


void ImLog::begin()
{
    messages.clear();
}


void ImLog::add(std::string m)
{
    messages.emplace_back(std::move(m));
}

void ImLog::draw(LogStyle style)
{
    switch (style)
    {
    case LogStyle::basic:
        draw_basic();
        break;
    case LogStyle::filter:
        draw_filter();
        break;
    default:
        DIE("Unhandled style");
        break;
    }
}

void ImLog::draw_basic() const
{
    for (const auto& m : messages)
    {
        ImGui::TextUnformatted(m.c_str());
    }
}

void ImLog::draw_filter()
{
    filter.Draw("filter");

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        if (filter.IsActive())
        {
            for (const auto& m : messages)
            {
                // todo(Gustav): optimize both filtering and clipper?
                const char* line_start = m.c_str();
                const char* line_end = m.c_str() + m.length() + 1;
                if (filter.PassFilter(line_start, line_end))
                {
                    ImGui::TextUnformatted(line_start, line_end);
                    ImGui::TextUnformatted(m.c_str());
                }
            }
        }
        else
        {
            ImGuiListClipper clipper;

            using clipper_int = int;
            ASSERT(std::cmp_less(messages.size(), std::numeric_limits<clipper_int>::max()));
            clipper.Begin(static_cast<clipper_int>(messages.size()));
            while (clipper.Step())
            {
                for (auto line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const auto& m = messages[line_no];
                    const char* line_start = m.c_str();
                    const char* line_end = m.c_str() + m.length() + 1;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}


bool ImTweak::get_bool(const std::string& name)
{
    auto found = booleans.find(name);
    if (found != booleans.end())
    {
        return found->second;
    }
    booleans[name] = false;
    return false;
}

void ImTweak::draw()
{
    ImGui::PushID("booleans");
    for (auto& [key, check] : booleans)
    {
        ImGui::Checkbox(key.c_str(), &check);
    }
    ImGui::PopID();
}


}
