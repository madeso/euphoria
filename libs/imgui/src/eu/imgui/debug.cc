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

void ImLog::draw() const
{
    // todo(Gustav): take inspiration from the dear imgui log example
    for (const auto& m : messages)
    {
        ImGui::TextUnformatted(m.c_str());
    }
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
