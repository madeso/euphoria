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
        struct keynamer
        {
            keynamer(T unbound, T invalid)
                : invalid_key(invalid)
                , unbound_key(unbound)
            {
                (*this)
                    (invalid_key, "<invalid>")
                    (unbound_key, "<unbound>")
                    ;
            }

            keynamer&
            operator()(T key, const std::string& name)
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

        const keynamer<Key>&
        all_keys()
        {
            static keynamer<Key> keys = keynamer<Key>
                (Key::unbound, Key::invalid)
                (Key::return_key, "Return")
                (Key::escape, "Escape")
                (Key::backspace, "Backspace")
                (Key::tab, "Tab")
                (Key::space, "Space")
                (Key::exclaim, "Exclaim")
                (Key::quotedbl, "Quotedbl")
                (Key::hash, "Hash")
                (Key::percent, "Percent")
                (Key::dollar, "Dollar")
                (Key::ampersand, "Ampersand")
                (Key::quote, "Quote")
                (Key::left_paren, "Leftparen")
                (Key::right_paren, "Rightparen")
                (Key::asterix, "Asterisk")
                (Key::plus, "Plus")
                (Key::comma, "Comma")
                (Key::minus, "Minus")
                (Key::period, "Period")
                (Key::slash, "Slash")
                (Key::num_0, "Num0")
                (Key::num_1, "Num1")
                (Key::num_2, "Num2")
                (Key::num_3, "Num3")
                (Key::num_4, "Num4")
                (Key::num_5, "Num5")
                (Key::num_6, "Num6")
                (Key::num_7, "Num7")
                (Key::num_8, "Num8")
                (Key::num_9, "Num9")
                (Key::colon, "Colon")
                (Key::semicolon, "Semicolon")
                (Key::less, "Less")
                (Key::equals, "Equals")
                (Key::greater, "Greater")
                (Key::question, "Question")
                (Key::at, "At")
                (Key::left_bracket, "Leftbracket")
                (Key::backslash, "Backslash")
                (Key::right_bracket, "Rightbracket")
                (Key::caret, "Caret")
                (Key::underscore, "Underscore")
                (Key::backquote, "Backquote")
                (Key::a, "A")
                (Key::b, "B")
                (Key::c, "C")
                (Key::d, "D")
                (Key::e, "E")
                (Key::f, "F")
                (Key::g, "G")
                (Key::h, "H")
                (Key::i, "I")
                (Key::j, "J")
                (Key::k, "K")
                (Key::l, "L")
                (Key::m, "M")
                (Key::n, "N")
                (Key::o, "O")
                (Key::p, "P")
                (Key::q, "Q")
                (Key::r, "R")
                (Key::s, "S")
                (Key::t, "T")
                (Key::u, "U")
                (Key::v, "V")
                (Key::w, "W")
                (Key::x, "X")
                (Key::y, "Y")
                (Key::z, "Z")
                (Key::capslock, "Capslock")
                (Key::f1, "F1")
                (Key::f2, "F2")
                (Key::f3, "F3")
                (Key::f4, "F4")
                (Key::f5, "F5")
                (Key::f6, "F6")
                (Key::f7, "F7")
                (Key::f8, "F8")
                (Key::f9, "F9")
                (Key::f10, "F10")
                (Key::f11, "F11")
                (Key::f12, "F12")
                (Key::print_screen, "Printscreen")
                (Key::scroll_lock, "Scrolllock")
                (Key::pause, "Pause")
                (Key::insert, "Insert")
                (Key::home, "Home")
                (Key::page_up, "Pageup")
                (Key::delete_key, "Delete")
                (Key::end, "End")
                (Key::page_down, "Pagedown")
                (Key::right, "Right")
                (Key::left, "Left")
                (Key::down, "Down")
                (Key::up, "Up")
                (Key::numlock_clear, "Numlockclear")
                (Key::keypad_divide, "Kp_Divide")
                (Key::keypad_multiply, "Kp_Multiply")
                (Key::keypad_minus, "Kp_Minus")
                (Key::keypad_plus, "Kp_Plus")
                (Key::keypad_enter, "Kp_Enter")
                (Key::keypad_1, "Kp_1")
                (Key::keypad_2, "Kp_2")
                (Key::keypad_3, "Kp_3")
                (Key::keypad_4, "Kp_4")
                (Key::keypad_5, "Kp_5")
                (Key::keypad_6, "Kp_6")
                (Key::keypad_7, "Kp_7")
                (Key::keypad_8, "Kp_8")
                (Key::keypad_9, "Kp_9")
                (Key::keypad_0, "Kp_0")
                (Key::keypad_period, "Kp_Period")
                (Key::application, "Application")
                (Key::power, "Power")
                (Key::keypad_equals, "Kp_Equals")
                (Key::f13, "F13")
                (Key::f14, "F14")
                (Key::f15, "F15")
                (Key::f16, "F16")
                (Key::f17, "F17")
                (Key::f18, "F18")
                (Key::f19, "F19")
                (Key::f20, "F20")
                (Key::f21, "F21")
                (Key::f22, "F22")
                (Key::f23, "F23")
                (Key::f24, "F24")
                (Key::execute, "Execute")
                (Key::help, "Help")
                (Key::menu, "Menu")
                (Key::select, "Select")
                (Key::stop, "Stop")
                (Key::again, "Again")
                (Key::undo, "Undo")
                (Key::cut, "Cut")
                (Key::copy, "Copy")
                (Key::paste, "Paste")
                (Key::find, "Find")
                (Key::mute, "Mute")
                (Key::volume_up, "Volumeup")
                (Key::volume_down, "Volumedown")
                (Key::keypad_comma, "Kp_Comma")
                (Key::keypad_equals_as_400, "Kp_Equalsas400")
                (Key::alt_erase, "Alterase")
                (Key::sysreq, "Sysreq")
                (Key::cancel, "Cancel")
                (Key::clear, "Clear")
                (Key::prior, "Prior")
                (Key::second_return, "Return2")
                (Key::separator, "Separator")
                (Key::out, "Out")
                (Key::oper, "Oper")
                (Key::clear_again, "Clearagain")
                (Key::crsel, "Crsel")
                (Key::exsel, "Exsel")
                (Key::keypad_00, "Kp_00")
                (Key::keypad_000, "Kp_000")
                (Key::thousandseparator, "Thousandsseparator")
                (Key::decimalseparator, "Decimalseparator")
                (Key::currency_unit, "Currencyunit")
                (Key::currency_subunit, "Currencysubunit")
                (Key::keypad_leftparen, "Kp_Leftparen")
                (Key::keypad_rightparen, "Kp_Rightparen")
                (Key::keypad_leftbrace, "Kp_Leftbrace")
                (Key::keypad_rightbrace, "Kp_Rightbrace")
                (Key::keypad_tab, "Kp_Tab")
                (Key::keypad_backspace, "Kp_Backspace")
                (Key::keypad_a, "Kp_A")
                (Key::keypad_b, "Kp_B")
                (Key::keypad_c, "Kp_C")
                (Key::keypad_d, "Kp_D")
                (Key::keypad_e, "Kp_E")
                (Key::keypad_f, "Kp_F")
                (Key::keypad_xor, "Kp_Xor")
                (Key::keypad_power, "Kp_Power")
                (Key::keypad_percent, "Kp_Percent")
                (Key::keypad_less, "Kp_Less")
                (Key::keypad_greater, "Kp_Greater")
                (Key::keypad_ampersand, "Kp_Ampersand")
                (Key::keypad_double_ampersand, "Kp_Dblampersand")
                (Key::keypad_vertical_bar, "Kp_Verticalbar")
                (Key::keypad_double_verticle_bar, "Kp_Dblverticalbar")
                (Key::keypad_colon, "Kp_Colon")
                (Key::keypad_hash, "Kp_Hash")
                (Key::keypad_space, "Kp_Space")
                (Key::keypad_at, "Kp_At")
                (Key::keypad_exclam, "Kp_Exclam")
                (Key::keypad_mem_store, "Kp_Memstore")
                (Key::keypad_mem_recall, "Kp_Memrecall")
                (Key::keypad_mem_clear, "Kp_Memclear")
                (Key::keypad_mem_add, "Kp_Memadd")
                (Key::keypad_mem_subtract, "Kp_Memsubtract")
                (Key::keypad_mem_multiply, "Kp_Memmultiply")
                (Key::keypad_mem_divide, "Kp_Memdivide")
                (Key::keypad_plus_minus, "Kp_Plusminus")
                (Key::keypad_clear, "Kp_Clear")
                (Key::keypad_clear_entry, "Kp_Clearentry")
                (Key::keypad_binary, "Kp_Binary")
                (Key::keypad_octal, "Kp_Octal")
                (Key::keypad_decimal, "Kp_Decimal")
                (Key::keypad_hexadecimal, "Kp_Hexadecimal")
                (Key::ctrl_left, "Lctrl")
                (Key::shift_left, "Lshift")
                (Key::alt_left, "Lalt")
                (Key::gui_left, "Lgui")
                (Key::ctrl_right, "Rctrl")
                (Key::shift_right, "Rshift")
                (Key::alt_right, "Ralt")
                (Key::gui_right, "Rgui")
                (Key::mode, "Mode")
                (Key::audio_next, "Audionext")
                (Key::audio_prev, "Audioprev")
                (Key::audio_stop, "Audiostop")
                (Key::audio_play, "Audioplay")
                (Key::audio_mute, "Audiomute")
                (Key::media_select, "Mediaselect")
                (Key::www, "Www")
                (Key::mail, "Mail")
                (Key::calculator, "Calculator")
                (Key::computer, "Computer")
                (Key::ac_search, "Ac_Search")
                (Key::ac_home, "Ac_Home")
                (Key::ac_back, "Ac_Back")
                (Key::ac_forward, "Ac_Forward")
                (Key::ac_stop, "Ac_Stop")
                (Key::ac_refresh, "Ac_Refresh")
                (Key::ac_bookmarks, "Ac_Bookmarks")
                (Key::brightness_down, "Brightnessdown")
                (Key::brightness_up, "Brightnessup")
                (Key::displayswitch, "Displayswitch")
                (Key::keyboard_illum_toggle, "Kbdillumtoggle")
                (Key::keyboard_illum_down, "Kbdillumdown")
                (Key::keyboard_illum_up, "Kbdillumup")
                (Key::eject, "Eject")
                (Key::sleep, "Sleep")
                ;
            return keys;
        }

        const keynamer<MouseButton>&
        all_mouse_buttons()
        {
            static keynamer<MouseButton> buttons = keynamer<MouseButton>
                (MouseButton::unbound, MouseButton::invalid)
                (MouseButton::left, "left")
                (MouseButton::middle, "middle")
                (MouseButton::right, "right")
                (MouseButton::x1, "x1")
                (MouseButton::x2, "x2")
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
