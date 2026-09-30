#pragma once
#include "imgui.h"

namespace eu::imgui
{

enum class LogStyle
{
    basic, filter
};

struct ImLog
{
    ImGuiTextFilter filter;
    std::vector<std::string> messages;

    void begin();
    void add(std::string m);
    void draw(LogStyle style = LogStyle::basic);

    void draw_basic() const;
    void draw_filter();
};

struct ImTweak
{
    std::unordered_map<std::string, bool> booleans;

    bool get_bool(const std::string& name);
    void draw();
};


}
