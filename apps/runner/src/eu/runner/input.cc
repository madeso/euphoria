#include "eu/runner/input.h"

#include <SDL_keycode.h>

#include "eu/io/file.h"
#include "eu/kdl/kdl.h"

namespace eu::runner
{

std::string lower(const std::string& str)
{
    std::string result;
    result.reserve(str.size());
    for (char c : str)
    {
        result.push_back(static_cast<char>(std::tolower(c)));
    }
    return result;
}

std::optional<KeyboardKey> key_from_name(const std::string& name)
{
    static const std::unordered_map<std::string, KeyboardKey> key_map = {
#define K(name){lower(#name), SDLK_##name}
    K(RETURN), K(ESCAPE), K(BACKSPACE), K(TAB), K(SPACE), K(EXCLAIM), K(QUOTEDBL),
    K(HASH), K(PERCENT), K(DOLLAR), K(AMPERSAND), K(QUOTE), K(LEFTPAREN),
    K(RIGHTPAREN), K(ASTERISK), K(PLUS), K(COMMA), K(MINUS), K(PERIOD), K(SLASH),
    K(0), K(1), K(2), K(3), K(4), K(5), K(6), K(7), K(8), K(9),
    K(COLON), K(SEMICOLON), K(LESS), K(EQUALS), K(GREATER), K(QUESTION), K(AT),
    K(LEFTBRACKET), K(BACKSLASH), K(RIGHTBRACKET), K(CARET), K(UNDERSCORE), K(BACKQUOTE),
    K(a), K(b), K(c), K(d), K(e), K(f), K(g), K(h), K(i), K(j),
    K(k), K(l), K(m), K(n), K(o), K(p), K(q), K(r), K(s), K(t),
    K(u), K(v), K(w), K(x), K(y), K(z),
    K(CAPSLOCK), K(F1), K(F2), K(F3), K(F4), K(F5), K(F6), K(F7), K(F8), K(F9), K(F10), K(F11), K(F12),
    K(PRINTSCREEN), K(SCROLLLOCK), K(PAUSE), K(INSERT), K(HOME), K(PAGEUP), K(DELETE),
    K(END), K(PAGEDOWN), K(RIGHT), K(LEFT), K(DOWN), K(UP), K(NUMLOCKCLEAR),
    K(KP_DIVIDE), K(KP_MULTIPLY), K(KP_MINUS), K(KP_PLUS), K(KP_ENTER),
    K(KP_1), K(KP_2), K(KP_3), K(KP_4), K(KP_5), K(KP_6), K(KP_7), K(KP_8), K(KP_9), K(KP_0),
    K(KP_PERIOD), K(APPLICATION), K(POWER), K(KP_EQUALS),
    K(F13), K(F14), K(F15), K(F16), K(F17), K(F18), K(F19), K(F20), K(F21), K(F22), K(F23), K(F24),
    K(EXECUTE), K(HELP), K(MENU), K(SELECT), K(STOP), K(AGAIN), K(UNDO),
    K(CUT), K(COPY), K(PASTE), K(FIND), K(MUTE), K(VOLUMEUP), K(VOLUMEDOWN),
    K(KP_COMMA), K(KP_EQUALSAS400), K(ALTERASE), K(SYSREQ), K(CANCEL), K(CLEAR),
    K(PRIOR), K(RETURN2), K(SEPARATOR), K(OUT), K(OPER), K(CLEARAGAIN), K(CRSEL),
    K(EXSEL), K(KP_00), K(KP_000), K(THOUSANDSSEPARATOR), K(DECIMALSEPARATOR),
    K(CURRENCYUNIT), K(CURRENCYSUBUNIT), K(KP_LEFTPAREN), K(KP_RIGHTPAREN), K(KP_LEFTBRACE),
    K(KP_RIGHTBRACE), K(KP_TAB), K(KP_BACKSPACE), K(KP_A), K(KP_B), K(KP_C), K(KP_D),
    K(KP_E), K(KP_F), K(KP_XOR), K(KP_POWER), K(KP_PERCENT), K(KP_LESS), K(KP_GREATER),
    K(KP_AMPERSAND), K(KP_DBLAMPERSAND), K(KP_VERTICALBAR), K(KP_DBLVERTICALBAR), K(KP_COLON),
    K(KP_HASH), K(KP_SPACE), K(KP_AT), K(KP_EXCLAM), K(KP_MEMSTORE), K(KP_MEMRECALL),
    K(KP_MEMCLEAR), K(KP_MEMADD), K(KP_MEMSUBTRACT), K(KP_MEMMULTIPLY), K(KP_MEMDIVIDE),
    K(KP_PLUSMINUS), K(KP_CLEAR), K(KP_CLEARENTRY), K(KP_BINARY), K(KP_OCTAL), K(KP_DECIMAL),
    K(KP_HEXADECIMAL), K(LCTRL), K(LSHIFT), K(LALT), K(LGUI), K(RCTRL), K(RSHIFT), K(RALT),
    K(RGUI), K(MODE), K(AUDIONEXT), K(AUDIOPREV), K(AUDIOSTOP), K(AUDIOPLAY), K(AUDIOMUTE),
    K(MEDIASELECT), K(WWW), K(MAIL), K(CALCULATOR), K(COMPUTER), K(AC_SEARCH), K(AC_HOME), K(AC_BACK),
    K(AC_FORWARD), K(AC_STOP), K(AC_REFRESH), K(AC_BOOKMARKS), K(BRIGHTNESSDOWN),
    K(BRIGHTNESSUP), K(DISPLAYSWITCH), K(KBDILLUMTOGGLE), K(KBDILLUMDOWN), K(KBDILLUMUP),
    K(EJECT), K(SLEEP), K(APP1), K(APP2), K(AUDIOREWIND), K(AUDIOFASTFORWARD),
    K(SOFTLEFT), K(SOFTRIGHT), K(CALL), K(ENDCALL),
#undef K
    };

    const auto it = key_map.find(lower(name));
    if (it != key_map.end())
    {
        return it->second;
    }

    LOG_WARN("Unknown key name: '{}'", name);
    return std::nullopt;
}
    
bool Input::load_from_file(const std::string& path)
{
    const auto doc = kdl::parse(eu::io::string_from_file(path));

    bool ok = true;

    for (const auto& node : doc)
    {
        if (node.name() == "action")
        {
            const auto name = node.args()[0].as_string();
            
            const auto existing = index_from_action.find(name);
            if (existing != index_from_action.end())
            {
                LOG_WARN("Duplicate action found: '{}'", name);
                ok = false;
                continue;
            }
            const auto new_index = actions.size();
            actions.emplace_back(Action{
                .name = name,
                .is_down = false,
                .last_down = false
            });
            index_from_action[name] = new_index;
        }
        else if (node.name() == "key")
        {
            const auto action_name = node.args()[0].as_string();
            const auto found_action = index_from_action.find(action_name);
            if (found_action == index_from_action.end())
            {
                LOG_WARN("Invalid action name: {}", action_name);
                ok = false;
                continue;
            }

            const auto key_name = node.args()[1].as_string();
            const auto key = key_from_name(key_name);
            if (!key)
            {
                LOG_WARN("Invalid key name: {}", key_name);
                ok = false;
                continue;
            }
            
            const auto bind = KeyBind{
                .sdl_keyboard_key = *key,
                .bind_action = found_action->second
            };
            bind_from_key[*key] = bind;
        }
        else
        {
            LOG_WARN("Unknown node in input file: '{}'", node.name());
            ok = false;
        }
    }

    return ok && !bind_from_key.empty();
}


void Input::update()
{
    for (auto& action : actions)
    {
        action.last_down = action.is_down;
    }
}


void Input::on_key(int key, bool state)
{
    const auto found = bind_from_key.find(key);
    if (found == bind_from_key.end())
    {
        return;
    }

    const KeyBind& bind = found->second;
    auto& action = actions[bind.bind_action];
    action.is_down = state;
}


InputFrame Input::get_current_frame() const
{
    InputFrame ret;
    for (const auto& a: actions)
    {
        ret.binds.emplace_back(ScriptBind{
            .name = a.name,
            .current = a.is_down ? 1.0f : 0.0f,
            .last = a.last_down ? 1.0f : 0.0f
        });
    }
    return ret;
}



}
