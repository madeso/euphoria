#pragma once

namespace eu::imgui
{


struct ImLog
{
    std::vector<std::string> messages;

    void begin();
    void add(std::string m);
    void draw() const;
};

struct ImTweak
{
    std::unordered_map<std::string, bool> booleans;

    bool get_bool(const std::string& name);
    void draw();
};


}
