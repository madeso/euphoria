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

        const keynamer<key>&
        all_keys()
        {
            static keynamer<key> keys = keynamer<key>
                (key::unbound, key::invalid)
                (key::return_key, "Return")
                (key::escape, "Escape")
                (key::backspace, "Backspace")
                (key::tab, "Tab")
                (key::space, "Space")
                (key::exclaim, "Exclaim")
                (key::quotedbl, "Quotedbl")
                (key::hash, "Hash")
                (key::percent, "Percent")
                (key::dollar, "Dollar")
                (key::ampersand, "Ampersand")
                (key::quote, "Quote")
                (key::left_paren, "Leftparen")
                (key::right_paren, "Rightparen")
                (key::asterix, "Asterisk")
                (key::plus, "Plus")
                (key::comma, "Comma")
                (key::minus, "Minus")
                (key::period, "Period")
                (key::slash, "Slash")
                (key::num_0, "Num0")
                (key::num_1, "Num1")
                (key::num_2, "Num2")
                (key::num_3, "Num3")
                (key::num_4, "Num4")
                (key::num_5, "Num5")
                (key::num_6, "Num6")
                (key::num_7, "Num7")
                (key::num_8, "Num8")
                (key::num_9, "Num9")
                (key::colon, "Colon")
                (key::semicolon, "Semicolon")
                (key::less, "Less")
                (key::equals, "Equals")
                (key::greater, "Greater")
                (key::question, "Question")
                (key::at, "At")
                (key::left_bracket, "Leftbracket")
                (key::backslash, "Backslash")
                (key::right_bracket, "Rightbracket")
                (key::caret, "Caret")
                (key::underscore, "Underscore")
                (key::backquote, "Backquote")
                (key::a, "A")
                (key::b, "B")
                (key::c, "C")
                (key::d, "D")
                (key::e, "E")
                (key::f, "F")
                (key::g, "G")
                (key::h, "H")
                (key::i, "I")
                (key::j, "J")
                (key::k, "K")
                (key::l, "L")
                (key::m, "M")
                (key::n, "N")
                (key::o, "O")
                (key::p, "P")
                (key::q, "Q")
                (key::r, "R")
                (key::s, "S")
                (key::t, "T")
                (key::u, "U")
                (key::v, "V")
                (key::w, "W")
                (key::x, "X")
                (key::y, "Y")
                (key::z, "Z")
                (key::capslock, "Capslock")
                (key::f1, "F1")
                (key::f2, "F2")
                (key::f3, "F3")
                (key::f4, "F4")
                (key::f5, "F5")
                (key::f6, "F6")
                (key::f7, "F7")
                (key::f8, "F8")
                (key::f9, "F9")
                (key::f10, "F10")
                (key::f11, "F11")
                (key::f12, "F12")
                (key::print_screen, "Printscreen")
                (key::scroll_lock, "Scrolllock")
                (key::pause, "Pause")
                (key::insert, "Insert")
                (key::home, "Home")
                (key::page_up, "Pageup")
                (key::delete_key, "Delete")
                (key::end, "End")
                (key::page_down, "Pagedown")
                (key::right, "Right")
                (key::left, "Left")
                (key::down, "Down")
                (key::up, "Up")
                (key::numlock_clear, "Numlockclear")
                (key::keypad_divide, "Kp_Divide")
                (key::keypad_multiply, "Kp_Multiply")
                (key::keypad_minus, "Kp_Minus")
                (key::keypad_plus, "Kp_Plus")
                (key::keypad_enter, "Kp_Enter")
                (key::keypad_1, "Kp_1")
                (key::keypad_2, "Kp_2")
                (key::keypad_3, "Kp_3")
                (key::keypad_4, "Kp_4")
                (key::keypad_5, "Kp_5")
                (key::keypad_6, "Kp_6")
                (key::keypad_7, "Kp_7")
                (key::keypad_8, "Kp_8")
                (key::keypad_9, "Kp_9")
                (key::keypad_0, "Kp_0")
                (key::keypad_period, "Kp_Period")
                (key::application, "Application")
                (key::power, "Power")
                (key::keypad_equals, "Kp_Equals")
                (key::f13, "F13")
                (key::f14, "F14")
                (key::f15, "F15")
                (key::f16, "F16")
                (key::f17, "F17")
                (key::f18, "F18")
                (key::f19, "F19")
                (key::f20, "F20")
                (key::f21, "F21")
                (key::f22, "F22")
                (key::f23, "F23")
                (key::f24, "F24")
                (key::execute, "Execute")
                (key::help, "Help")
                (key::menu, "Menu")
                (key::select, "Select")
                (key::stop, "Stop")
                (key::again, "Again")
                (key::undo, "Undo")
                (key::cut, "Cut")
                (key::copy, "Copy")
                (key::paste, "Paste")
                (key::find, "Find")
                (key::mute, "Mute")
                (key::volume_up, "Volumeup")
                (key::volume_down, "Volumedown")
                (key::keypad_comma, "Kp_Comma")
                (key::keypad_equals_as_400, "Kp_Equalsas400")
                (key::alt_erase, "Alterase")
                (key::sysreq, "Sysreq")
                (key::cancel, "Cancel")
                (key::clear, "Clear")
                (key::prior, "Prior")
                (key::second_return, "Return2")
                (key::separator, "Separator")
                (key::out, "Out")
                (key::oper, "Oper")
                (key::clear_again, "Clearagain")
                (key::crsel, "Crsel")
                (key::exsel, "Exsel")
                (key::keypad_00, "Kp_00")
                (key::keypad_000, "Kp_000")
                (key::thousandseparator, "Thousandsseparator")
                (key::decimalseparator, "Decimalseparator")
                (key::currency_unit, "Currencyunit")
                (key::currency_subunit, "Currencysubunit")
                (key::keypad_leftparen, "Kp_Leftparen")
                (key::keypad_rightparen, "Kp_Rightparen")
                (key::keypad_leftbrace, "Kp_Leftbrace")
                (key::keypad_rightbrace, "Kp_Rightbrace")
                (key::keypad_tab, "Kp_Tab")
                (key::keypad_backspace, "Kp_Backspace")
                (key::keypad_a, "Kp_A")
                (key::keypad_b, "Kp_B")
                (key::keypad_c, "Kp_C")
                (key::keypad_d, "Kp_D")
                (key::keypad_e, "Kp_E")
                (key::keypad_f, "Kp_F")
                (key::keypad_xor, "Kp_Xor")
                (key::keypad_power, "Kp_Power")
                (key::keypad_percent, "Kp_Percent")
                (key::keypad_less, "Kp_Less")
                (key::keypad_greater, "Kp_Greater")
                (key::keypad_ampersand, "Kp_Ampersand")
                (key::keypad_double_ampersand, "Kp_Dblampersand")
                (key::keypad_vertical_bar, "Kp_Verticalbar")
                (key::keypad_double_verticle_bar, "Kp_Dblverticalbar")
                (key::keypad_colon, "Kp_Colon")
                (key::keypad_hash, "Kp_Hash")
                (key::keypad_space, "Kp_Space")
                (key::keypad_at, "Kp_At")
                (key::keypad_exclam, "Kp_Exclam")
                (key::keypad_mem_store, "Kp_Memstore")
                (key::keypad_mem_recall, "Kp_Memrecall")
                (key::keypad_mem_clear, "Kp_Memclear")
                (key::keypad_mem_add, "Kp_Memadd")
                (key::keypad_mem_subtract, "Kp_Memsubtract")
                (key::keypad_mem_multiply, "Kp_Memmultiply")
                (key::keypad_mem_divide, "Kp_Memdivide")
                (key::keypad_plus_minus, "Kp_Plusminus")
                (key::keypad_clear, "Kp_Clear")
                (key::keypad_clear_entry, "Kp_Clearentry")
                (key::keypad_binary, "Kp_Binary")
                (key::keypad_octal, "Kp_Octal")
                (key::keypad_decimal, "Kp_Decimal")
                (key::keypad_hexadecimal, "Kp_Hexadecimal")
                (key::ctrl_left, "Lctrl")
                (key::shift_left, "Lshift")
                (key::alt_left, "Lalt")
                (key::gui_left, "Lgui")
                (key::ctrl_right, "Rctrl")
                (key::shift_right, "Rshift")
                (key::alt_right, "Ralt")
                (key::gui_right, "Rgui")
                (key::mode, "Mode")
                (key::audio_next, "Audionext")
                (key::audio_prev, "Audioprev")
                (key::audio_stop, "Audiostop")
                (key::audio_play, "Audioplay")
                (key::audio_mute, "Audiomute")
                (key::media_select, "Mediaselect")
                (key::www, "Www")
                (key::mail, "Mail")
                (key::calculator, "Calculator")
                (key::computer, "Computer")
                (key::ac_search, "Ac_Search")
                (key::ac_home, "Ac_Home")
                (key::ac_back, "Ac_Back")
                (key::ac_forward, "Ac_Forward")
                (key::ac_stop, "Ac_Stop")
                (key::ac_refresh, "Ac_Refresh")
                (key::ac_bookmarks, "Ac_Bookmarks")
                (key::brightness_down, "Brightnessdown")
                (key::brightness_up, "Brightnessup")
                (key::displayswitch, "Displayswitch")
                (key::keyboard_illum_toggle, "Kbdillumtoggle")
                (key::keyboard_illum_down, "Kbdillumdown")
                (key::keyboard_illum_up, "Kbdillumup")
                (key::eject, "Eject")
                (key::sleep, "Sleep")
                ;
            return keys;
        }

        const keynamer<mouse_button>&
        all_mouse_buttons()
        {
            static keynamer<mouse_button> buttons = keynamer<mouse_button>
                (mouse_button::unbound, mouse_button::invalid)
                (mouse_button::left, "left")
                (mouse_button::middle, "middle")
                (mouse_button::right, "right")
                (mouse_button::x1, "x1")
                (mouse_button::x2, "x2")
                ;
            return buttons;
        }

    }

    std::string
    to_string(key k)
    {
        return all_keys().from_key_to_string(k);
    }

    key
    to_key(const std::string& name)
    {
        return all_keys().from_string_to_key(name);
    }

    std::string
    to_string(mouse_button button)
    {
        return all_mouse_buttons().from_key_to_string(button);
    }

    mouse_button
    to_mouse_button(const std::string& button)
    {
        return all_mouse_buttons().from_string_to_key(button);
    }

}
