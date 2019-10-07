// Euphoria - Copyright (c) Gustav

#include "core/key.h"

#include <string>
#include <map>

#include "core/stringutils.h"

namespace euphoria::core
{
    namespace
    {
        template <typename T>
        class Keyname
        {
        public:
            Keyname(T unbound, T invalid) : invalid_(invalid), unbound_(unbound)
            {
                (*this)(invalid_, "<invalid>")(unbound_, "<unbound>");
            }

            Keyname&
            operator()(T key, const std::string& name)
            {
                strtokey.insert(std::make_pair(ToLower(name), key));
                keytostr.insert(std::make_pair(key, name));

                return *this;
            }

            std::string
            FromKey(T key) const
            {
                auto r = keytostr.find(key);
                if (r == keytostr.end())
                {
                    return "Unknown";
                }
                else
                {
                    return r->second;
                }
            }

            T
            FromString(const std::string& keyname) const
            {
                if (keyname == "")
                {
                    return unbound_;
                }
                auto r = strtokey.find(ToLower(keyname));
                if (r == strtokey.end())
                {
                    return invalid_;
                }
                else
                {
                    return r->second;
                }
            }

            std::map<std::string, T> strtokey;
            std::map<T, std::string> keytostr;
            const T                  invalid_;
            const T                  unbound_;
        };

        const Keyname<Key>&
        AllKeys()
        {
            static Keyname<Key> Names = Keyname<Key>(
                    Key::UNBOUND, Key::INVALID)(Key::RETURN, "Return")(
                    Key::ESCAPE, "Escape")(Key::BACKSPACE, "Backspace")(
                    Key::TAB, "Tab")(Key::SPACE, "Space")(
                    Key::EXCLAIM, "Exclaim")(Key::QUOTEDBL, "Quotedbl")(
                    Key::HASH, "Hash")(Key::PERCENT, "Percent")(
                    Key::DOLLAR, "Dollar")(Key::AMPERSAND, "Ampersand")(
                    Key::QUOTE, "Quote")(Key::LEFT_PAREN, "Leftparen")(
                    Key::RIGHT_PAREN, "Rightparen")(Key::ASTERIX, "Asterisk")(
                    Key::PLUS, "Plus")(Key::COMMA, "Comma")(
                    Key::MINUS, "Minus")(Key::PERIOD, "Period")(
                    Key::SLASH, "Slash")(Key::NUM_0, "Num0")(
                    Key::NUM_1, "Num1")(Key::NUM_2, "Num2")(Key::NUM_3, "Num3")(
                    Key::NUM_4, "Num4")(Key::NUM_5, "Num5")(Key::NUM_6, "Num6")(
                    Key::NUM_7, "Num7")(Key::NUM_8, "Num8")(Key::NUM_9, "Num9")(
                    Key::COLON, "Colon")(Key::SEMICOLON, "Semicolon")(
                    Key::LESS, "Less")(Key::EQUALS, "Equals")(
                    Key::GREATER, "Greater")(Key::QUESTION, "Question")(
                    Key::AT, "At")(Key::LEFT_BRACKET, "Leftbracket")(
                    Key::BACKSLASH, "Backslash")(
                    Key::RIGHT_BRACKET, "Rightbracket")(Key::CARET, "Caret")(
                    Key::UNDERSCORE, "Underscore")(Key::BACKQUOTE, "Backquote")(
                    Key::A, "A")(Key::B, "B")(Key::C, "C")(Key::D, "D")(
                    Key::E, "E")(Key::F, "F")(Key::G, "G")(Key::H, "H")(
                    Key::I, "I")(Key::J, "J")(Key::K, "K")(Key::L, "L")(
                    Key::M, "M")(Key::N, "N")(Key::O, "O")(Key::P, "P")(
                    Key::Q, "Q")(Key::R, "R")(Key::S, "S")(Key::T, "T")(
                    Key::U, "U")(Key::V, "V")(Key::W, "W")(Key::X, "X")(
                    Key::Y, "Y")(Key::Z, "Z")(Key::CAPSLOCK, "Capslock")(
                    Key::F1, "F1")(Key::F2, "F2")(Key::F3, "F3")(Key::F4, "F4")(
                    Key::F5, "F5")(Key::F6, "F6")(Key::F7, "F7")(Key::F8, "F8")(
                    Key::F9, "F9")(Key::F10, "F10")(Key::F11, "F11")(
                    Key::F12, "F12")(Key::PRINTSCREEN, "Printscreen")(
                    Key::SCROLLLOCK, "Scrolllock")(Key::PAUSE, "Pause")(
                    Key::INSERT, "Insert")(Key::HOME, "Home")(
                    Key::PAGEUP, "Pageup")(Key::DELETE, "Delete")(
                    Key::END, "End")(Key::PAGEDOWN, "Pagedown")(
                    Key::RIGHT, "Right")(Key::LEFT, "Left")(Key::DOWN, "Down")(
                    Key::UP, "Up")(Key::NUMLOCK_CLEAR, "Numlockclear")(
                    Key::KEYPAD_DIVIDE, "Kp_Divide")(
                    Key::KEYPAD_MULTIPLY, "Kp_Multiply")(
                    Key::KEYPAD_MINUS, "Kp_Minus")(Key::KEYPAD_PLUS, "Kp_Plus")(
                    Key::KEYPAD_ENTER, "Kp_Enter")(Key::KEYPAD_1, "Kp_1")(
                    Key::KEYPAD_2, "Kp_2")(Key::KEYPAD_3, "Kp_3")(
                    Key::KEYPAD_4, "Kp_4")(Key::KEYPAD_5, "Kp_5")(
                    Key::KEYPAD_6, "Kp_6")(Key::KEYPAD_7, "Kp_7")(
                    Key::KEYPAD_8, "Kp_8")(Key::KEYPAD_9, "Kp_9")(
                    Key::KEYPAD_0, "Kp_0")(Key::KEYPAD_PERIOD, "Kp_Period")(
                    Key::APPLICATION, "Application")(Key::POWER, "Power")(
                    Key::KEYPAD_EQUALS, "Kp_Equals")(Key::F13, "F13")(
                    Key::F14, "F14")(Key::F15, "F15")(Key::F16, "F16")(
                    Key::F17, "F17")(Key::F18, "F18")(Key::F19, "F19")(
                    Key::F20, "F20")(Key::F21, "F21")(Key::F22, "F22")(
                    Key::F23, "F23")(Key::F24, "F24")(Key::EXECUTE, "Execute")(
                    Key::HELP, "Help")(Key::MENU, "Menu")(
                    Key::SELECT, "Select")(Key::STOP, "Stop")(
                    Key::AGAIN, "Again")(Key::UNDO, "Undo")(Key::CUT, "Cut")(
                    Key::COPY, "Copy")(Key::PASTE, "Paste")(Key::FIND, "Find")(
                    Key::MUTE, "Mute")(Key::VOLUME_UP, "Volumeup")(
                    Key::VOLUME_DOWN, "Volumedown")(
                    Key::KEYPAD_COMMA, "Kp_Comma")(
                    Key::KEYPAD_EQUALS_AS_400, "Kp_Equalsas400")(
                    Key::ALT_ERASE, "Alterase")(Key::SYSREQ, "Sysreq")(
                    Key::CANCEL, "Cancel")(Key::CLEAR, "Clear")(
                    Key::PRIOR, "Prior")(Key::SECOND_RETURN, "Return2")(
                    Key::SEPARATOR, "Separator")(Key::OUT, "Out")(
                    Key::OPER, "Oper")(Key::CLEAR_AGAIN, "Clearagain")(
                    Key::CRSEL, "Crsel")(Key::EXSEL, "Exsel")(
                    Key::KEYPAD_00, "Kp_00")(Key::KEYPAD_000, "Kp_000")(
                    Key::THOUSANDSEPARATOR, "Thousandsseparator")(
                    Key::DECIMALSEPARATOR, "Decimalseparator")(
                    Key::CURRENCY_UNIT, "Currencyunit")(
                    Key::CURRENCY_SUBUNIT, "Currencysubunit")(
                    Key::KEYPAD_LEFTPAREN, "Kp_Leftparen")(
                    Key::KEYPAD_RIGHTPAREN, "Kp_Rightparen")(
                    Key::KEYPAD_LEFTBRACE, "Kp_Leftbrace")(
                    Key::KEYPAD_RIGHTBRACE, "Kp_Rightbrace")(
                    Key::KEYPAD_TAB, "Kp_Tab")(
                    Key::KEYPAD_BACKSPACE, "Kp_Backspace")(
                    Key::KEYPAD_A, "Kp_A")(Key::KEYPAD_B, "Kp_B")(
                    Key::KEYPAD_C, "Kp_C")(Key::KEYPAD_D, "Kp_D")(
                    Key::KEYPAD_E, "Kp_E")(Key::KEYPAD_F, "Kp_F")(
                    Key::KEYPAD_XOR, "Kp_Xor")(Key::KEYPAD_POWER, "Kp_Power")(
                    Key::KEYPAD_PERCENT, "Kp_Percent")(
                    Key::KEYPAD_LESS, "Kp_Less")(
                    Key::KEYPAD_GREATER, "Kp_Greater")(
                    Key::KEYPAD_AMPERSAND, "Kp_Ampersand")(
                    Key::KEYPAD_DOUBLE_AMPERSAND, "Kp_Dblampersand")(
                    Key::KEYPAD_VERTICAL_BAR, "Kp_Verticalbar")(
                    Key::KEYPAD_DOUBLE_VERTICLE_BAR, "Kp_Dblverticalbar")(
                    Key::KEYPAD_COLON, "Kp_Colon")(Key::KEYPAD_HASH, "Kp_Hash")(
                    Key::KEYPAD_SPACE, "Kp_Space")(Key::KEYPAD_AT, "Kp_At")(
                    Key::KEYPAD_EXCLAM, "Kp_Exclam")(
                    Key::KEYPAD_MEM_STORE, "Kp_Memstore")(
                    Key::KEYPAD_MEM_RECALL, "Kp_Memrecall")(
                    Key::KEYPAD_MEM_CLEAR, "Kp_Memclear")(
                    Key::KEYPAD_MEM_ADD, "Kp_Memadd")(
                    Key::KEYPAD_MEM_SUBTRACT, "Kp_Memsubtract")(
                    Key::KEYPAD_MEM_MULTIPLY, "Kp_Memmultiply")(
                    Key::KEYPAD_MEM_DIVIDE, "Kp_Memdivide")(
                    Key::KEYPAD_PLUS_MINUS, "Kp_Plusminus")(
                    Key::KEYPAD_CLEAR, "Kp_Clear")(
                    Key::KEYPAD_CLEAR_ENTRY, "Kp_Clearentry")(
                    Key::KEYPAD_BINARY, "Kp_Binary")(
                    Key::KEYPAD_OCTAL, "Kp_Octal")(
                    Key::KEYPAD_DECIMAL, "Kp_Decimal")(
                    Key::KEYPAD_HEXADECIMAL, "Kp_Hexadecimal")(
                    Key::CTRL_LEFT, "Lctrl")(Key::SHIFT_LEFT, "Lshift")(
                    Key::ALT_LEFT, "Lalt")(Key::GUI_LEFT, "Lgui")(
                    Key::CTRL_RIGHT, "Rctrl")(Key::SHIFT_RIGHT, "Rshift")(
                    Key::ALT_RIGHT, "Ralt")(Key::GUI_RIGHT, "Rgui")(
                    Key::MODE, "Mode")(Key::AUDIO_NEXT, "Audionext")(
                    Key::AUDIO_PREV, "Audioprev")(Key::AUDIO_STOP, "Audiostop")(
                    Key::AUDIO_PLAY, "Audioplay")(Key::AUDIO_MUTE, "Audiomute")(
                    Key::MEDIASELECT, "Mediaselect")(Key::WWW, "Www")(
                    Key::MAIL, "Mail")(Key::CALCULATOR, "Calculator")(
                    Key::COMPUTER, "Computer")(Key::AC_SEARCH, "Ac_Search")(
                    Key::AC_HOME, "Ac_Home")(Key::AC_BACK, "Ac_Back")(
                    Key::AC_FORWARD, "Ac_Forward")(Key::AC_STOP, "Ac_Stop")(
                    Key::AC_REFRESH, "Ac_Refresh")(
                    Key::AC_BOOKMARKS, "Ac_Bookmarks")(
                    Key::BRIGHTNESS_DOWN, "Brightnessdown")(
                    Key::BRIGHTNESS_UP, "Brightnessup")(
                    Key::DISPLAYSWITCH, "Displayswitch")(
                    Key::KEYBOARD_ILLUM_TOGGLE, "Kbdillumtoggle")(
                    Key::KEYBOARD_ILLUM_DOWN, "Kbdillumdown")(
                    Key::KEYBOARD_ILLUM_UP, "Kbdillumup")(Key::EJECT, "Eject")(
                    Key::SLEEP, "Sleep");
            return Names;
        }

        const Keyname<MouseButton>&
        AllMouseButtons()
        {
            static Keyname<MouseButton> Buttons = Keyname<MouseButton>(
                    MouseButton::UNBOUND, MouseButton::INVALID)(
                    MouseButton::LEFT, "left")(MouseButton::MIDDLE, "middle")(
                    MouseButton::RIGHT, "right")(MouseButton::X1, "x1")(
                    MouseButton::X2, "x2");
            return Buttons;
        }

    }  // namespace

    std::string
    ToString(Key k)
    {
        return AllKeys().FromKey(k);
    }

    Key
    ToKey(const std::string& keyname)
    {
        return AllKeys().FromString(keyname);
    }

    std::string
    ToString(MouseButton k)
    {
        return AllMouseButtons().FromKey(k);
    }

    MouseButton
    ToMouseButton(const std::string& keyname)
    {
        return AllMouseButtons().FromString(keyname);
    }

}  // namespace euphoria::core
