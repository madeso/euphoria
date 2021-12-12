#include "window/key.h"

#include "core/assert.h"

namespace euphoria::window
{
    core::Key
    to_key(SDL_Keysym key)
    {
        switch(key.sym)
        {
        case SDLK_RETURN: return core::Key::return_key;
        case SDLK_ESCAPE: return core::Key::escape;
        case SDLK_BACKSPACE: return core::Key::backspace;
        case SDLK_TAB: return core::Key::tab;
        case SDLK_SPACE: return core::Key::space;
        case SDLK_EXCLAIM: return core::Key::exclaim;
        case SDLK_QUOTEDBL: return core::Key::quotedbl;
        case SDLK_HASH: return core::Key::hash;
        case SDLK_PERCENT: return core::Key::percent;
        case SDLK_DOLLAR: return core::Key::dollar;
        case SDLK_AMPERSAND: return core::Key::ampersand;
        case SDLK_QUOTE: return core::Key::quote;
        case SDLK_LEFTPAREN: return core::Key::left_paren;
        case SDLK_RIGHTPAREN: return core::Key::right_paren;
        case SDLK_ASTERISK: return core::Key::asterix;
        case SDLK_PLUS: return core::Key::plus;
        case SDLK_COMMA: return core::Key::comma;
        case SDLK_MINUS: return core::Key::minus;
        case SDLK_PERIOD: return core::Key::period;
        case SDLK_SLASH: return core::Key::slash;
        case SDLK_0: return core::Key::num_0;
        case SDLK_1: return core::Key::num_1;
        case SDLK_2: return core::Key::num_2;
        case SDLK_3: return core::Key::num_3;
        case SDLK_4: return core::Key::num_4;
        case SDLK_5: return core::Key::num_5;
        case SDLK_6: return core::Key::num_6;
        case SDLK_7: return core::Key::num_7;
        case SDLK_8: return core::Key::num_8;
        case SDLK_9: return core::Key::num_9;
        case SDLK_COLON: return core::Key::colon;
        case SDLK_SEMICOLON: return core::Key::semicolon;
        case SDLK_LESS: return core::Key::less;
        case SDLK_EQUALS: return core::Key::equals;
        case SDLK_GREATER: return core::Key::greater;
        case SDLK_QUESTION: return core::Key::question;
        case SDLK_AT: return core::Key::at;
        case SDLK_LEFTBRACKET: return core::Key::left_bracket;
        case SDLK_BACKSLASH: return core::Key::backslash;
        case SDLK_RIGHTBRACKET: return core::Key::right_bracket;
        case SDLK_CARET: return core::Key::caret;
        case SDLK_UNDERSCORE: return core::Key::underscore;
        case SDLK_BACKQUOTE: return core::Key::backquote;
        case SDLK_a: return core::Key::a;
        case SDLK_b: return core::Key::b;
        case SDLK_c: return core::Key::c;
        case SDLK_d: return core::Key::d;
        case SDLK_e: return core::Key::e;
        case SDLK_f: return core::Key::f;
        case SDLK_g: return core::Key::g;
        case SDLK_h: return core::Key::h;
        case SDLK_i: return core::Key::i;
        case SDLK_j: return core::Key::j;
        case SDLK_k: return core::Key::k;
        case SDLK_l: return core::Key::l;
        case SDLK_m: return core::Key::m;
        case SDLK_n: return core::Key::n;
        case SDLK_o: return core::Key::o;
        case SDLK_p: return core::Key::p;
        case SDLK_q: return core::Key::q;
        case SDLK_r: return core::Key::r;
        case SDLK_s: return core::Key::s;
        case SDLK_t: return core::Key::t;
        case SDLK_u: return core::Key::u;
        case SDLK_v: return core::Key::v;
        case SDLK_w: return core::Key::w;
        case SDLK_x: return core::Key::x;
        case SDLK_y: return core::Key::y;
        case SDLK_z: return core::Key::z;
        case SDLK_CAPSLOCK: return core::Key::capslock;
        case SDLK_F1: return core::Key::f1;
        case SDLK_F2: return core::Key::f2;
        case SDLK_F3: return core::Key::f3;
        case SDLK_F4: return core::Key::f4;
        case SDLK_F5: return core::Key::f5;
        case SDLK_F6: return core::Key::f6;
        case SDLK_F7: return core::Key::f7;
        case SDLK_F8: return core::Key::f8;
        case SDLK_F9: return core::Key::f9;
        case SDLK_F10: return core::Key::f10;
        case SDLK_F11: return core::Key::f11;
        case SDLK_F12: return core::Key::f12;
        case SDLK_PRINTSCREEN: return core::Key::print_screen;
        case SDLK_SCROLLLOCK: return core::Key::scroll_lock;
        case SDLK_PAUSE: return core::Key::pause;
        case SDLK_INSERT: return core::Key::insert;
        case SDLK_HOME: return core::Key::home;
        case SDLK_PAGEUP: return core::Key::page_up;
        case SDLK_DELETE: return core::Key::delete_key;
        case SDLK_END: return core::Key::end;
        case SDLK_PAGEDOWN: return core::Key::page_down;
        case SDLK_RIGHT: return core::Key::right;
        case SDLK_LEFT: return core::Key::left;
        case SDLK_DOWN: return core::Key::down;
        case SDLK_UP: return core::Key::up;
        case SDLK_NUMLOCKCLEAR: return core::Key::numlock_clear;
        case SDLK_KP_DIVIDE: return core::Key::keypad_divide;
        case SDLK_KP_MULTIPLY: return core::Key::keypad_multiply;
        case SDLK_KP_MINUS: return core::Key::keypad_minus;
        case SDLK_KP_PLUS: return core::Key::keypad_plus;
        case SDLK_KP_ENTER: return core::Key::keypad_enter;
        case SDLK_KP_1: return core::Key::keypad_1;
        case SDLK_KP_2: return core::Key::keypad_2;
        case SDLK_KP_3: return core::Key::keypad_3;
        case SDLK_KP_4: return core::Key::keypad_4;
        case SDLK_KP_5: return core::Key::keypad_5;
        case SDLK_KP_6: return core::Key::keypad_6;
        case SDLK_KP_7: return core::Key::keypad_7;
        case SDLK_KP_8: return core::Key::keypad_8;
        case SDLK_KP_9: return core::Key::keypad_9;
        case SDLK_KP_0: return core::Key::keypad_0;
        case SDLK_KP_PERIOD: return core::Key::keypad_period;
        case SDLK_APPLICATION: return core::Key::application;
        case SDLK_POWER: return core::Key::power;
        case SDLK_KP_EQUALS: return core::Key::keypad_equals;
        case SDLK_F13: return core::Key::f13;
        case SDLK_F14: return core::Key::f14;
        case SDLK_F15: return core::Key::f15;
        case SDLK_F16: return core::Key::f16;
        case SDLK_F17: return core::Key::f17;
        case SDLK_F18: return core::Key::f18;
        case SDLK_F19: return core::Key::f19;
        case SDLK_F20: return core::Key::f20;
        case SDLK_F21: return core::Key::f21;
        case SDLK_F22: return core::Key::f22;
        case SDLK_F23: return core::Key::f23;
        case SDLK_F24: return core::Key::f24;
        case SDLK_EXECUTE: return core::Key::execute;
        case SDLK_HELP: return core::Key::help;
        case SDLK_MENU: return core::Key::menu;
        case SDLK_SELECT: return core::Key::select;
        case SDLK_STOP: return core::Key::stop;
        case SDLK_AGAIN: return core::Key::again;
        case SDLK_UNDO: return core::Key::undo;
        case SDLK_CUT: return core::Key::cut;
        case SDLK_COPY: return core::Key::copy;
        case SDLK_PASTE: return core::Key::paste;
        case SDLK_FIND: return core::Key::find;
        case SDLK_MUTE: return core::Key::mute;
        case SDLK_VOLUMEUP: return core::Key::volume_up;
        case SDLK_VOLUMEDOWN: return core::Key::volume_down;
        case SDLK_KP_COMMA: return core::Key::keypad_comma;
        case SDLK_KP_EQUALSAS400: return core::Key::keypad_equals_as_400;
        case SDLK_ALTERASE: return core::Key::alt_erase;
        case SDLK_SYSREQ: return core::Key::sysreq;
        case SDLK_CANCEL: return core::Key::cancel;
        case SDLK_CLEAR: return core::Key::clear;
        case SDLK_PRIOR: return core::Key::prior;
        case SDLK_RETURN2: return core::Key::second_return;
        case SDLK_SEPARATOR: return core::Key::separator;
        case SDLK_OUT: return core::Key::out;
        case SDLK_OPER: return core::Key::oper;
        case SDLK_CLEARAGAIN: return core::Key::clear_again;
        case SDLK_CRSEL: return core::Key::crsel;
        case SDLK_EXSEL: return core::Key::exsel;
        case SDLK_KP_00: return core::Key::keypad_00;
        case SDLK_KP_000: return core::Key::keypad_000;
        case SDLK_THOUSANDSSEPARATOR: return core::Key::thousandseparator;
        case SDLK_DECIMALSEPARATOR: return core::Key::decimalseparator;
        case SDLK_CURRENCYUNIT: return core::Key::currency_unit;
        case SDLK_CURRENCYSUBUNIT: return core::Key::currency_subunit;
        case SDLK_KP_LEFTPAREN: return core::Key::keypad_leftparen;
        case SDLK_KP_RIGHTPAREN: return core::Key::keypad_rightparen;
        case SDLK_KP_LEFTBRACE: return core::Key::keypad_leftbrace;
        case SDLK_KP_RIGHTBRACE: return core::Key::keypad_rightbrace;
        case SDLK_KP_TAB: return core::Key::keypad_tab;
        case SDLK_KP_BACKSPACE: return core::Key::keypad_backspace;
        case SDLK_KP_A: return core::Key::keypad_a;
        case SDLK_KP_B: return core::Key::keypad_b;
        case SDLK_KP_C: return core::Key::keypad_c;
        case SDLK_KP_D: return core::Key::keypad_d;
        case SDLK_KP_E: return core::Key::keypad_e;
        case SDLK_KP_F: return core::Key::keypad_f;
        case SDLK_KP_XOR: return core::Key::keypad_xor;
        case SDLK_KP_POWER: return core::Key::keypad_power;
        case SDLK_KP_PERCENT: return core::Key::keypad_percent;
        case SDLK_KP_LESS: return core::Key::keypad_less;
        case SDLK_KP_GREATER: return core::Key::keypad_greater;
        case SDLK_KP_AMPERSAND: return core::Key::keypad_ampersand;
        case SDLK_KP_DBLAMPERSAND: return core::Key::keypad_double_ampersand;
        case SDLK_KP_VERTICALBAR: return core::Key::keypad_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR: return core::Key::keypad_double_verticle_bar;
        case SDLK_KP_COLON: return core::Key::keypad_colon;
        case SDLK_KP_HASH: return core::Key::keypad_hash;
        case SDLK_KP_SPACE: return core::Key::keypad_space;
        case SDLK_KP_AT: return core::Key::keypad_at;
        case SDLK_KP_EXCLAM: return core::Key::keypad_exclam;
        case SDLK_KP_MEMSTORE: return core::Key::keypad_mem_store;
        case SDLK_KP_MEMRECALL: return core::Key::keypad_mem_recall;
        case SDLK_KP_MEMCLEAR: return core::Key::keypad_mem_clear;
        case SDLK_KP_MEMADD: return core::Key::keypad_mem_add;
        case SDLK_KP_MEMSUBTRACT: return core::Key::keypad_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return core::Key::keypad_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return core::Key::keypad_mem_divide;
        case SDLK_KP_PLUSMINUS: return core::Key::keypad_plus_minus;
        case SDLK_KP_CLEAR: return core::Key::keypad_clear;
        case SDLK_KP_CLEARENTRY: return core::Key::keypad_clear_entry;
        case SDLK_KP_BINARY: return core::Key::keypad_binary;
        case SDLK_KP_OCTAL: return core::Key::keypad_octal;
        case SDLK_KP_DECIMAL: return core::Key::keypad_decimal;
        case SDLK_KP_HEXADECIMAL: return core::Key::keypad_hexadecimal;
        case SDLK_LCTRL: return core::Key::ctrl_left;
        case SDLK_LSHIFT: return core::Key::shift_left;
        case SDLK_LALT: return core::Key::alt_left;
        case SDLK_LGUI: return core::Key::gui_left;
        case SDLK_RCTRL: return core::Key::ctrl_right;
        case SDLK_RSHIFT: return core::Key::shift_right;
        case SDLK_RALT: return core::Key::alt_right;
        case SDLK_RGUI: return core::Key::gui_right;
        case SDLK_MODE: return core::Key::mode;
        case SDLK_AUDIONEXT: return core::Key::audio_next;
        case SDLK_AUDIOPREV: return core::Key::audio_prev;
        case SDLK_AUDIOSTOP: return core::Key::audio_stop;
        case SDLK_AUDIOPLAY: return core::Key::audio_play;
        case SDLK_AUDIOMUTE: return core::Key::audio_mute;
        case SDLK_MEDIASELECT: return core::Key::media_select;
        case SDLK_WWW: return core::Key::www;
        case SDLK_MAIL: return core::Key::mail;
        case SDLK_CALCULATOR: return core::Key::calculator;
        case SDLK_COMPUTER: return core::Key::computer;
        case SDLK_AC_SEARCH: return core::Key::ac_search;
        case SDLK_AC_HOME: return core::Key::ac_home;
        case SDLK_AC_BACK: return core::Key::ac_back;
        case SDLK_AC_FORWARD: return core::Key::ac_forward;
        case SDLK_AC_STOP: return core::Key::ac_stop;
        case SDLK_AC_REFRESH: return core::Key::ac_refresh;
        case SDLK_AC_BOOKMARKS: return core::Key::ac_bookmarks;
        case SDLK_BRIGHTNESSDOWN: return core::Key::brightness_down;
        case SDLK_BRIGHTNESSUP: return core::Key::brightness_up;
        case SDLK_DISPLAYSWITCH: return core::Key::displayswitch;
        case SDLK_KBDILLUMTOGGLE: return core::Key::keyboard_illum_toggle;
        case SDLK_KBDILLUMDOWN: return core::Key::keyboard_illum_down;
        case SDLK_KBDILLUMUP: return core::Key::keyboard_illum_up;
        case SDLK_EJECT: return core::Key::eject;
        case SDLK_SLEEP: return core::Key::sleep;
        default:
            // todo(Gustav): die should have a x version too
            ASSERTX(false && "Invalid keyboard button", key.sym);
            return core::Key::invalid;
        }
    }

    core::MouseButton
    to_key(SDL_MouseButtonEvent mb)
    {
        switch(mb.button)
        {
        case SDL_BUTTON_LEFT: return core::MouseButton::left;
        case SDL_BUTTON_MIDDLE: return core::MouseButton::middle;
        case SDL_BUTTON_RIGHT: return core::MouseButton::right;
        case SDL_BUTTON_X1: return core::MouseButton::x1;
        case SDL_BUTTON_X2: return core::MouseButton::x2;
        default:
            ASSERTX(false && "Invalid mouse button", mb.button);
            return core::MouseButton::invalid;
        }
    }

}
