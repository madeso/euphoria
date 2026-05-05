#pragma once

namespace eu::runner
{

// todo(Gustav): introduce archetypes
// todo(Gustav): expand with more inputs
// todo(Gustav): split keybind and controller files

// must match SDL_Keycode
using KeyboardKey = i32;

struct KeyBind
{
    KeyboardKey sdl_keyboard_key;
    std::size_t bind_action;
};

struct Action
{
    std::string name;
    bool is_down;
    bool last_down;
};

struct ScriptBind
{
    std::string name;
    float current;
    float last;
};

struct InputFrame
{
    std::vector<ScriptBind> binds;
};

struct Input
{
    std::vector<Action> actions;
    std::unordered_map<std::string, std::size_t> index_from_action;
    std::unordered_map<KeyboardKey, KeyBind> bind_from_key;

    bool load_from_file(const std::string& path);

    void update();
    void on_key(KeyboardKey key, bool state);
    InputFrame get_current_frame() const;
};

}
