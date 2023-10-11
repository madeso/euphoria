#include "files/game.h"


namespace eu::files::game
{
    JSON_PARSE_FUNC(vec2f)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(Number, x);
        JSON_VAL(Number, y);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Sprite)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, path);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(VarNumber)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(Number, value);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Var)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);

        // only one is valid
        JSON_VAL_OPT_OBJ(VarNumber, number);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Custom)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_ARRAY(Var, arguments);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Component)
    {
        JSON_BEGIN_OBJECT();

        // only one is valid
        JSON_VAL_OPT_OBJ(vec2f, position);
        JSON_VAL_OPT_OBJ(Sprite, sprite);
        JSON_VAL_OPT_OBJ(Custom, custom);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Template)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_ARRAY(Component, components);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(ViewportType)
    {
        JSON_BEGIN_ENUM(ViewportType);

        JSON_ENUM_VAL(fit_with_black_bars);
        JSON_ENUM_VAL(extend);
        JSON_ENUM_VAL(screen_pixel);

        JSON_END_ENUM();
    }

    JSON_PARSE_FUNC(Viewport)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL_OBJ(ViewportType, type);
        JSON_VAL(Number, width);
        JSON_VAL(Number, height);

        JSON_END_OBJECT();
    }

    ////////////////////////////////////////////////////////////////////////////////

    JSON_PARSE_FUNC(KeyBind)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_VAL(String, key);

        JSON_END_OBJECT();
    }

    ////////////////////////////////////////////////////////////////////////////////

    JSON_PARSE_FUNC(Color)
    {
        JSON_BEGIN_OBJECT();

        // todo(Gustav): change to a rgb property
        JSON_OPT_VAL(String, hex);

        JSON_END_OBJECT();
    }

    JSON_PARSE_FUNC(Game)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, title);
        JSON_VAL_OPT_OBJ(Color, clear_color);
        JSON_VAL_OBJ(Viewport, viewport);
        JSON_ARRAY(KeyBind, binds);
        JSON_ARRAY(Template, templates);

        JSON_END_OBJECT();
    }
}
