#pragma once

#include "io/json.h"


namespace eu::files::game
{
    ////////////////////////////////////////////////////////////////////////////////

    struct vec2f
    {
        float x;
        float y;
    };

    struct Sprite
    {
        std::string path;
    };

    struct VarNumber
    {
        float value;
    };

    struct Var
    {
        std::string name;

        // only one is valid
        std::optional<VarNumber> number;
    };

    struct Custom
    {
        std::string name;
        std::vector<Var> arguments;
    };

    struct Component
    {
        // only one is valid
        std::optional<vec2f> position;
        std::optional<Sprite> sprite;
        std::optional<Custom> custom;
    };

    struct Template
    {
        std::string name;
        std::vector<Component> components;
    };

    // todo(Gustav): rename enum
    enum class ViewportType
    {
        fit_with_black_bars, extend, screen_pixel
    };

    struct Viewport
    {
        ViewportType type;
        float width = 1.0f;
        float height = 1.0f;
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct KeyBind
    {
        std::string name;
        std::string key;
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct Color
    {
        // todo(Gustav): change to a rgb property
        std::optional<std::string> hex;
    };

    struct Game
    {
        std::string title;
        std::optional<Color> clear_color;
        Viewport viewport;
        std::vector<KeyBind> binds;
        std::vector<Template> templates;
    };

    JSON_PARSE_FUNC(Game);
}
