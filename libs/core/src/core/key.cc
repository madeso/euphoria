// Euphoria - Copyright (c) Gustav

#include "core/key.h"


#include <map>

#include "core/stringutils.h"

namespace eu::core
{
    namespace
    {
        template <typename T>
        struct KeyNamer
        {
            KeyNamer(T unbound, T invalid)
                : invalid_key(invalid)
                , unbound_key(unbound)
            {
                add(invalid_key, "<invalid>");
                add(unbound_key, "<unbound>");
            }

            KeyNamer&
            add(T key, const std::string& name)
            {
                string_to_key.insert(std::make_pair(to_lower(name), key));
                key_to_string.insert(std::make_pair(key, name));

                return *this;
            }

            [[nodiscard]] std::string
            from_key_to_string(T key) const
            {
                auto r = key_to_string.find(key);
                if(r == key_to_string.end())
                {
                    return "Unknown";
                }
                else
                {
                    return r->second;
                }
            }

            [[nodiscard]] T
            from_string_to_key(const std::string& name) const
            {
                if(name.empty())
                {
                    return unbound_key;
                }
                auto r = string_to_key.find(to_lower(name));
                if(r == string_to_key.end())
                {
                    return invalid_key;
                }
                else
                {
                    return r->second;
                }
            }

            std::map<std::string, T> string_to_key;
            std::map<T, std::string> key_to_string;
            const T invalid_key;
            const T unbound_key;
        };

        const KeyNamer<Key>&
        all_keys()
        {
            static const KeyNamer<Key> keys =
                KeyNamer<Key>{Key::unbound, Key::invalid}
                .add(Key::return_key, "Return")
                .add(Key::escape, "Escape")
                .add(Key::backspace, "Backspace")
                .add(Key::tab, "Tab")
                .add(Key::space, "Space")
                .add(Key::exclaim, "Exclaim")
                .add(Key::quotedbl, "Quotedbl")
                .add(Key::hash, "Hash")
                .add(Key::percent, "Percent")
                .add(Key::dollar, "Dollar")
                .add(Key::ampersand, "Ampersand")
                .add(Key::quote, "Quote")
                .add(Key::left_paren, "Leftparen")
                .add(Key::right_paren, "Rightparen")
                .add(Key::asterix, "Asterisk")
                .add(Key::plus, "Plus")
                .add(Key::comma, "Comma")
                .add(Key::minus, "Minus")
                .add(Key::period, "Period")
                .add(Key::slash, "Slash")
                .add(Key::num_0, "Num0")
                .add(Key::num_1, "Num1")
                .add(Key::num_2, "Num2")
                .add(Key::num_3, "Num3")
                .add(Key::num_4, "Num4")
                .add(Key::num_5, "Num5")
                .add(Key::num_6, "Num6")
                .add(Key::num_7, "Num7")
                .add(Key::num_8, "Num8")
                .add(Key::num_9, "Num9")
                .add(Key::colon, "Colon")
                .add(Key::semicolon, "Semicolon")
                .add(Key::less, "Less")
                .add(Key::equals, "Equals")
                .add(Key::greater, "Greater")
                .add(Key::question, "Question")
                .add(Key::at, "At")
                .add(Key::left_bracket, "Leftbracket")
                .add(Key::backslash, "Backslash")
                .add(Key::right_bracket, "Rightbracket")
                .add(Key::caret, "Caret")
                .add(Key::underscore, "Underscore")
                .add(Key::backquote, "Backquote")
                .add(Key::a, "A")
                .add(Key::b, "B")
                .add(Key::c, "C")
                .add(Key::d, "D")
                .add(Key::e, "E")
                .add(Key::f, "F")
                .add(Key::g, "G")
                .add(Key::h, "H")
                .add(Key::i, "I")
                .add(Key::j, "J")
                .add(Key::k, "K")
                .add(Key::l, "L")
                .add(Key::m, "M")
                .add(Key::n, "N")
                .add(Key::o, "O")
                .add(Key::p, "P")
                .add(Key::q, "Q")
                .add(Key::r, "R")
                .add(Key::s, "S")
                .add(Key::t, "T")
                .add(Key::u, "U")
                .add(Key::v, "V")
                .add(Key::w, "W")
                .add(Key::x, "X")
                .add(Key::y, "Y")
                .add(Key::z, "Z")
                .add(Key::capslock, "Capslock")
                .add(Key::f1, "F1")
                .add(Key::f2, "F2")
                .add(Key::f3, "F3")
                .add(Key::f4, "F4")
                .add(Key::f5, "F5")
                .add(Key::f6, "F6")
                .add(Key::f7, "F7")
                .add(Key::f8, "F8")
                .add(Key::f9, "F9")
                .add(Key::f10, "F10")
                .add(Key::f11, "F11")
                .add(Key::f12, "F12")
                .add(Key::print_screen, "Printscreen")
                .add(Key::scroll_lock, "Scrolllock")
                .add(Key::pause, "Pause")
                .add(Key::insert, "Insert")
                .add(Key::home, "Home")
                .add(Key::page_up, "Pageup")
                .add(Key::delete_key, "Delete")
                .add(Key::end, "End")
                .add(Key::page_down, "Pagedown")
                .add(Key::right, "Right")
                .add(Key::left, "Left")
                .add(Key::down, "Down")
                .add(Key::up, "Up")
                .add(Key::numlock_clear, "Numlockclear")
                .add(Key::keypad_divide, "Kp_Divide")
                .add(Key::keypad_multiply, "Kp_Multiply")
                .add(Key::keypad_minus, "Kp_Minus")
                .add(Key::keypad_plus, "Kp_Plus")
                .add(Key::keypad_enter, "Kp_Enter")
                .add(Key::keypad_1, "Kp_1")
                .add(Key::keypad_2, "Kp_2")
                .add(Key::keypad_3, "Kp_3")
                .add(Key::keypad_4, "Kp_4")
                .add(Key::keypad_5, "Kp_5")
                .add(Key::keypad_6, "Kp_6")
                .add(Key::keypad_7, "Kp_7")
                .add(Key::keypad_8, "Kp_8")
                .add(Key::keypad_9, "Kp_9")
                .add(Key::keypad_0, "Kp_0")
                .add(Key::keypad_period, "Kp_Period")
                .add(Key::application, "Application")
                .add(Key::power, "Power")
                .add(Key::keypad_equals, "Kp_Equals")
                .add(Key::f13, "F13")
                .add(Key::f14, "F14")
                .add(Key::f15, "F15")
                .add(Key::f16, "F16")
                .add(Key::f17, "F17")
                .add(Key::f18, "F18")
                .add(Key::f19, "F19")
                .add(Key::f20, "F20")
                .add(Key::f21, "F21")
                .add(Key::f22, "F22")
                .add(Key::f23, "F23")
                .add(Key::f24, "F24")
                .add(Key::execute, "Execute")
                .add(Key::help, "Help")
                .add(Key::menu, "Menu")
                .add(Key::select, "Select")
                .add(Key::stop, "Stop")
                .add(Key::again, "Again")
                .add(Key::undo, "Undo")
                .add(Key::cut, "Cut")
                .add(Key::copy, "Copy")
                .add(Key::paste, "Paste")
                .add(Key::find, "Find")
                .add(Key::mute, "Mute")
                .add(Key::volume_up, "Volumeup")
                .add(Key::volume_down, "Volumedown")
                .add(Key::keypad_comma, "Kp_Comma")
                .add(Key::keypad_equals_as_400, "Kp_Equalsas400")
                .add(Key::alt_erase, "Alterase")
                .add(Key::sysreq, "Sysreq")
                .add(Key::cancel, "Cancel")
                .add(Key::clear, "Clear")
                .add(Key::prior, "Prior")
                .add(Key::second_return, "Return2")
                .add(Key::separator, "Separator")
                .add(Key::out, "Out")
                .add(Key::oper, "Oper")
                .add(Key::clear_again, "Clearagain")
                .add(Key::crsel, "Crsel")
                .add(Key::exsel, "Exsel")
                .add(Key::keypad_00, "Kp_00")
                .add(Key::keypad_000, "Kp_000")
                .add(Key::thousandseparator, "Thousandsseparator")
                .add(Key::decimalseparator, "Decimalseparator")
                .add(Key::currency_unit, "Currencyunit")
                .add(Key::currency_subunit, "Currencysubunit")
                .add(Key::keypad_leftparen, "Kp_Leftparen")
                .add(Key::keypad_rightparen, "Kp_Rightparen")
                .add(Key::keypad_leftbrace, "Kp_Leftbrace")
                .add(Key::keypad_rightbrace, "Kp_Rightbrace")
                .add(Key::keypad_tab, "Kp_Tab")
                .add(Key::keypad_backspace, "Kp_Backspace")
                .add(Key::keypad_a, "Kp_A")
                .add(Key::keypad_b, "Kp_B")
                .add(Key::keypad_c, "Kp_C")
                .add(Key::keypad_d, "Kp_D")
                .add(Key::keypad_e, "Kp_E")
                .add(Key::keypad_f, "Kp_F")
                .add(Key::keypad_xor, "Kp_Xor")
                .add(Key::keypad_power, "Kp_Power")
                .add(Key::keypad_percent, "Kp_Percent")
                .add(Key::keypad_less, "Kp_Less")
                .add(Key::keypad_greater, "Kp_Greater")
                .add(Key::keypad_ampersand, "Kp_Ampersand")
                .add(Key::keypad_double_ampersand, "Kp_Dblampersand")
                .add(Key::keypad_vertical_bar, "Kp_Verticalbar")
                .add(Key::keypad_double_verticle_bar, "Kp_Dblverticalbar")
                .add(Key::keypad_colon, "Kp_Colon")
                .add(Key::keypad_hash, "Kp_Hash")
                .add(Key::keypad_space, "Kp_Space")
                .add(Key::keypad_at, "Kp_At")
                .add(Key::keypad_exclam, "Kp_Exclam")
                .add(Key::keypad_mem_store, "Kp_Memstore")
                .add(Key::keypad_mem_recall, "Kp_Memrecall")
                .add(Key::keypad_mem_clear, "Kp_Memclear")
                .add(Key::keypad_mem_add, "Kp_Memadd")
                .add(Key::keypad_mem_subtract, "Kp_Memsubtract")
                .add(Key::keypad_mem_multiply, "Kp_Memmultiply")
                .add(Key::keypad_mem_divide, "Kp_Memdivide")
                .add(Key::keypad_plus_minus, "Kp_Plusminus")
                .add(Key::keypad_clear, "Kp_Clear")
                .add(Key::keypad_clear_entry, "Kp_Clearentry")
                .add(Key::keypad_binary, "Kp_Binary")
                .add(Key::keypad_octal, "Kp_Octal")
                .add(Key::keypad_decimal, "Kp_Decimal")
                .add(Key::keypad_hexadecimal, "Kp_Hexadecimal")
                .add(Key::ctrl_left, "Lctrl")
                .add(Key::shift_left, "Lshift")
                .add(Key::alt_left, "Lalt")
                .add(Key::gui_left, "Lgui")
                .add(Key::ctrl_right, "Rctrl")
                .add(Key::shift_right, "Rshift")
                .add(Key::alt_right, "Ralt")
                .add(Key::gui_right, "Rgui")
                .add(Key::mode, "Mode")
                .add(Key::audio_next, "Audionext")
                .add(Key::audio_prev, "Audioprev")
                .add(Key::audio_stop, "Audiostop")
                .add(Key::audio_play, "Audioplay")
                .add(Key::audio_mute, "Audiomute")
                .add(Key::media_select, "Mediaselect")
                .add(Key::www, "Www")
                .add(Key::mail, "Mail")
                .add(Key::calculator, "Calculator")
                .add(Key::computer, "Computer")
                .add(Key::ac_search, "Ac_Search")
                .add(Key::ac_home, "Ac_Home")
                .add(Key::ac_back, "Ac_Back")
                .add(Key::ac_forward, "Ac_Forward")
                .add(Key::ac_stop, "Ac_Stop")
                .add(Key::ac_refresh, "Ac_Refresh")
                .add(Key::ac_bookmarks, "Ac_Bookmarks")
                .add(Key::brightness_down, "Brightnessdown")
                .add(Key::brightness_up, "Brightnessup")
                .add(Key::displayswitch, "Displayswitch")
                .add(Key::keyboard_illum_toggle, "Kbdillumtoggle")
                .add(Key::keyboard_illum_down, "Kbdillumdown")
                .add(Key::keyboard_illum_up, "Kbdillumup")
                .add(Key::eject, "Eject")
                .add(Key::sleep, "Sleep")
                ;
            return keys;
        }

        const KeyNamer<MouseButton>&
        all_mouse_buttons()
        {
            static const KeyNamer<MouseButton> buttons =
                KeyNamer<MouseButton>{MouseButton::unbound, MouseButton::invalid}
                .add(MouseButton::left, "left")
                .add(MouseButton::middle, "middle")
                .add(MouseButton::right, "right")
                .add(MouseButton::x1, "x1")
                .add(MouseButton::x2, "x2")
                ;
            return buttons;
        }

    }

    std::string
    to_string(Key k)
    {
        return all_keys().from_key_to_string(k);
    }

    Key
    to_key(const std::string& name)
    {
        return all_keys().from_string_to_key(name);
    }

    std::string
    to_string(MouseButton button)
    {
        return all_mouse_buttons().from_key_to_string(button);
    }

    MouseButton
    to_mouse_button(const std::string& button)
    {
        return all_mouse_buttons().from_string_to_key(button);
    }

}
