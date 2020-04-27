#include "window/key.h"

#include "core/assert.h"

namespace euphoria::window
{
    core::Key
    ToKey(SDL_Keysym key)
    {
        switch(key.sym)
        {
        case SDLK_RETURN: return core::Key::RETURN;
        case SDLK_ESCAPE: return core::Key::ESCAPE;
        case SDLK_BACKSPACE: return core::Key::BACKSPACE;
        case SDLK_TAB: return core::Key::TAB;
        case SDLK_SPACE: return core::Key::SPACE;
        case SDLK_EXCLAIM: return core::Key::EXCLAIM;
        case SDLK_QUOTEDBL: return core::Key::QUOTEDBL;
        case SDLK_HASH: return core::Key::HASH;
        case SDLK_PERCENT: return core::Key::PERCENT;
        case SDLK_DOLLAR: return core::Key::DOLLAR;
        case SDLK_AMPERSAND: return core::Key::AMPERSAND;
        case SDLK_QUOTE: return core::Key::QUOTE;
        case SDLK_LEFTPAREN: return core::Key::LEFT_PAREN;
        case SDLK_RIGHTPAREN: return core::Key::RIGHT_PAREN;
        case SDLK_ASTERISK: return core::Key::ASTERIX;
        case SDLK_PLUS: return core::Key::PLUS;
        case SDLK_COMMA: return core::Key::COMMA;
        case SDLK_MINUS: return core::Key::MINUS;
        case SDLK_PERIOD: return core::Key::PERIOD;
        case SDLK_SLASH: return core::Key::SLASH;
        case SDLK_0: return core::Key::NUM_0;
        case SDLK_1: return core::Key::NUM_1;
        case SDLK_2: return core::Key::NUM_2;
        case SDLK_3: return core::Key::NUM_3;
        case SDLK_4: return core::Key::NUM_4;
        case SDLK_5: return core::Key::NUM_5;
        case SDLK_6: return core::Key::NUM_6;
        case SDLK_7: return core::Key::NUM_7;
        case SDLK_8: return core::Key::NUM_8;
        case SDLK_9: return core::Key::NUM_9;
        case SDLK_COLON: return core::Key::COLON;
        case SDLK_SEMICOLON: return core::Key::SEMICOLON;
        case SDLK_LESS: return core::Key::LESS;
        case SDLK_EQUALS: return core::Key::EQUALS;
        case SDLK_GREATER: return core::Key::GREATER;
        case SDLK_QUESTION: return core::Key::QUESTION;
        case SDLK_AT: return core::Key::AT;
        case SDLK_LEFTBRACKET: return core::Key::LEFT_BRACKET;
        case SDLK_BACKSLASH: return core::Key::BACKSLASH;
        case SDLK_RIGHTBRACKET: return core::Key::RIGHT_BRACKET;
        case SDLK_CARET: return core::Key::CARET;
        case SDLK_UNDERSCORE: return core::Key::UNDERSCORE;
        case SDLK_BACKQUOTE: return core::Key::BACKQUOTE;
        case SDLK_a: return core::Key::A;
        case SDLK_b: return core::Key::B;
        case SDLK_c: return core::Key::C;
        case SDLK_d: return core::Key::D;
        case SDLK_e: return core::Key::E;
        case SDLK_f: return core::Key::F;
        case SDLK_g: return core::Key::G;
        case SDLK_h: return core::Key::H;
        case SDLK_i: return core::Key::I;
        case SDLK_j: return core::Key::J;
        case SDLK_k: return core::Key::K;
        case SDLK_l: return core::Key::L;
        case SDLK_m: return core::Key::M;
        case SDLK_n: return core::Key::N;
        case SDLK_o: return core::Key::O;
        case SDLK_p: return core::Key::P;
        case SDLK_q: return core::Key::Q;
        case SDLK_r: return core::Key::R;
        case SDLK_s: return core::Key::S;
        case SDLK_t: return core::Key::T;
        case SDLK_u: return core::Key::U;
        case SDLK_v: return core::Key::V;
        case SDLK_w: return core::Key::W;
        case SDLK_x: return core::Key::X;
        case SDLK_y: return core::Key::Y;
        case SDLK_z: return core::Key::Z;
        case SDLK_CAPSLOCK: return core::Key::CAPSLOCK;
        case SDLK_F1: return core::Key::F1;
        case SDLK_F2: return core::Key::F2;
        case SDLK_F3: return core::Key::F3;
        case SDLK_F4: return core::Key::F4;
        case SDLK_F5: return core::Key::F5;
        case SDLK_F6: return core::Key::F6;
        case SDLK_F7: return core::Key::F7;
        case SDLK_F8: return core::Key::F8;
        case SDLK_F9: return core::Key::F9;
        case SDLK_F10: return core::Key::F10;
        case SDLK_F11: return core::Key::F11;
        case SDLK_F12: return core::Key::F12;
        case SDLK_PRINTSCREEN: return core::Key::PRINTSCREEN;
        case SDLK_SCROLLLOCK: return core::Key::SCROLLLOCK;
        case SDLK_PAUSE: return core::Key::PAUSE;
        case SDLK_INSERT: return core::Key::INSERT;
        case SDLK_HOME: return core::Key::HOME;
        case SDLK_PAGEUP: return core::Key::PAGEUP;
        case SDLK_DELETE: return core::Key::DELETE;
        case SDLK_END: return core::Key::END;
        case SDLK_PAGEDOWN: return core::Key::PAGEDOWN;
        case SDLK_RIGHT: return core::Key::RIGHT;
        case SDLK_LEFT: return core::Key::LEFT;
        case SDLK_DOWN: return core::Key::DOWN;
        case SDLK_UP: return core::Key::UP;
        case SDLK_NUMLOCKCLEAR: return core::Key::NUMLOCK_CLEAR;
        case SDLK_KP_DIVIDE: return core::Key::KEYPAD_DIVIDE;
        case SDLK_KP_MULTIPLY: return core::Key::KEYPAD_MULTIPLY;
        case SDLK_KP_MINUS: return core::Key::KEYPAD_MINUS;
        case SDLK_KP_PLUS: return core::Key::KEYPAD_PLUS;
        case SDLK_KP_ENTER: return core::Key::KEYPAD_ENTER;
        case SDLK_KP_1: return core::Key::KEYPAD_1;
        case SDLK_KP_2: return core::Key::KEYPAD_2;
        case SDLK_KP_3: return core::Key::KEYPAD_3;
        case SDLK_KP_4: return core::Key::KEYPAD_4;
        case SDLK_KP_5: return core::Key::KEYPAD_5;
        case SDLK_KP_6: return core::Key::KEYPAD_6;
        case SDLK_KP_7: return core::Key::KEYPAD_7;
        case SDLK_KP_8: return core::Key::KEYPAD_8;
        case SDLK_KP_9: return core::Key::KEYPAD_9;
        case SDLK_KP_0: return core::Key::KEYPAD_0;
        case SDLK_KP_PERIOD: return core::Key::KEYPAD_PERIOD;
        case SDLK_APPLICATION: return core::Key::APPLICATION;
        case SDLK_POWER: return core::Key::POWER;
        case SDLK_KP_EQUALS: return core::Key::KEYPAD_EQUALS;
        case SDLK_F13: return core::Key::F13;
        case SDLK_F14: return core::Key::F14;
        case SDLK_F15: return core::Key::F15;
        case SDLK_F16: return core::Key::F16;
        case SDLK_F17: return core::Key::F17;
        case SDLK_F18: return core::Key::F18;
        case SDLK_F19: return core::Key::F19;
        case SDLK_F20: return core::Key::F20;
        case SDLK_F21: return core::Key::F21;
        case SDLK_F22: return core::Key::F22;
        case SDLK_F23: return core::Key::F23;
        case SDLK_F24: return core::Key::F24;
        case SDLK_EXECUTE: return core::Key::EXECUTE;
        case SDLK_HELP: return core::Key::HELP;
        case SDLK_MENU: return core::Key::MENU;
        case SDLK_SELECT: return core::Key::SELECT;
        case SDLK_STOP: return core::Key::STOP;
        case SDLK_AGAIN: return core::Key::AGAIN;
        case SDLK_UNDO: return core::Key::UNDO;
        case SDLK_CUT: return core::Key::CUT;
        case SDLK_COPY: return core::Key::COPY;
        case SDLK_PASTE: return core::Key::PASTE;
        case SDLK_FIND: return core::Key::FIND;
        case SDLK_MUTE: return core::Key::MUTE;
        case SDLK_VOLUMEUP: return core::Key::VOLUME_UP;
        case SDLK_VOLUMEDOWN: return core::Key::VOLUME_DOWN;
        case SDLK_KP_COMMA: return core::Key::KEYPAD_COMMA;
        case SDLK_KP_EQUALSAS400: return core::Key::KEYPAD_EQUALS_AS_400;
        case SDLK_ALTERASE: return core::Key::ALT_ERASE;
        case SDLK_SYSREQ: return core::Key::SYSREQ;
        case SDLK_CANCEL: return core::Key::CANCEL;
        case SDLK_CLEAR: return core::Key::CLEAR;
        case SDLK_PRIOR: return core::Key::PRIOR;
        case SDLK_RETURN2: return core::Key::SECOND_RETURN;
        case SDLK_SEPARATOR: return core::Key::SEPARATOR;
        case SDLK_OUT: return core::Key::OUT;
        case SDLK_OPER: return core::Key::OPER;
        case SDLK_CLEARAGAIN: return core::Key::CLEAR_AGAIN;
        case SDLK_CRSEL: return core::Key::CRSEL;
        case SDLK_EXSEL: return core::Key::EXSEL;
        case SDLK_KP_00: return core::Key::KEYPAD_00;
        case SDLK_KP_000: return core::Key::KEYPAD_000;
        case SDLK_THOUSANDSSEPARATOR: return core::Key::THOUSANDSEPARATOR;
        case SDLK_DECIMALSEPARATOR: return core::Key::DECIMALSEPARATOR;
        case SDLK_CURRENCYUNIT: return core::Key::CURRENCY_UNIT;
        case SDLK_CURRENCYSUBUNIT: return core::Key::CURRENCY_SUBUNIT;
        case SDLK_KP_LEFTPAREN: return core::Key::KEYPAD_LEFTPAREN;
        case SDLK_KP_RIGHTPAREN: return core::Key::KEYPAD_RIGHTPAREN;
        case SDLK_KP_LEFTBRACE: return core::Key::KEYPAD_LEFTBRACE;
        case SDLK_KP_RIGHTBRACE: return core::Key::KEYPAD_RIGHTBRACE;
        case SDLK_KP_TAB: return core::Key::KEYPAD_TAB;
        case SDLK_KP_BACKSPACE: return core::Key::KEYPAD_BACKSPACE;
        case SDLK_KP_A: return core::Key::KEYPAD_A;
        case SDLK_KP_B: return core::Key::KEYPAD_B;
        case SDLK_KP_C: return core::Key::KEYPAD_C;
        case SDLK_KP_D: return core::Key::KEYPAD_D;
        case SDLK_KP_E: return core::Key::KEYPAD_E;
        case SDLK_KP_F: return core::Key::KEYPAD_F;
        case SDLK_KP_XOR: return core::Key::KEYPAD_XOR;
        case SDLK_KP_POWER: return core::Key::KEYPAD_POWER;
        case SDLK_KP_PERCENT: return core::Key::KEYPAD_PERCENT;
        case SDLK_KP_LESS: return core::Key::KEYPAD_LESS;
        case SDLK_KP_GREATER: return core::Key::KEYPAD_GREATER;
        case SDLK_KP_AMPERSAND: return core::Key::KEYPAD_AMPERSAND;
        case SDLK_KP_DBLAMPERSAND: return core::Key::KEYPAD_DOUBLE_AMPERSAND;
        case SDLK_KP_VERTICALBAR: return core::Key::KEYPAD_VERTICAL_BAR;
        case SDLK_KP_DBLVERTICALBAR:
            return core::Key::KEYPAD_DOUBLE_VERTICLE_BAR;
        case SDLK_KP_COLON: return core::Key::KEYPAD_COLON;
        case SDLK_KP_HASH: return core::Key::KEYPAD_HASH;
        case SDLK_KP_SPACE: return core::Key::KEYPAD_SPACE;
        case SDLK_KP_AT: return core::Key::KEYPAD_AT;
        case SDLK_KP_EXCLAM: return core::Key::KEYPAD_EXCLAM;
        case SDLK_KP_MEMSTORE: return core::Key::KEYPAD_MEM_STORE;
        case SDLK_KP_MEMRECALL: return core::Key::KEYPAD_MEM_RECALL;
        case SDLK_KP_MEMCLEAR: return core::Key::KEYPAD_MEM_CLEAR;
        case SDLK_KP_MEMADD: return core::Key::KEYPAD_MEM_ADD;
        case SDLK_KP_MEMSUBTRACT: return core::Key::KEYPAD_MEM_SUBTRACT;
        case SDLK_KP_MEMMULTIPLY: return core::Key::KEYPAD_MEM_MULTIPLY;
        case SDLK_KP_MEMDIVIDE: return core::Key::KEYPAD_MEM_DIVIDE;
        case SDLK_KP_PLUSMINUS: return core::Key::KEYPAD_PLUS_MINUS;
        case SDLK_KP_CLEAR: return core::Key::KEYPAD_CLEAR;
        case SDLK_KP_CLEARENTRY: return core::Key::KEYPAD_CLEAR_ENTRY;
        case SDLK_KP_BINARY: return core::Key::KEYPAD_BINARY;
        case SDLK_KP_OCTAL: return core::Key::KEYPAD_OCTAL;
        case SDLK_KP_DECIMAL: return core::Key::KEYPAD_DECIMAL;
        case SDLK_KP_HEXADECIMAL: return core::Key::KEYPAD_HEXADECIMAL;
        case SDLK_LCTRL: return core::Key::CTRL_LEFT;
        case SDLK_LSHIFT: return core::Key::SHIFT_LEFT;
        case SDLK_LALT: return core::Key::ALT_LEFT;
        case SDLK_LGUI: return core::Key::GUI_LEFT;
        case SDLK_RCTRL: return core::Key::CTRL_RIGHT;
        case SDLK_RSHIFT: return core::Key::SHIFT_RIGHT;
        case SDLK_RALT: return core::Key::ALT_RIGHT;
        case SDLK_RGUI: return core::Key::GUI_RIGHT;
        case SDLK_MODE: return core::Key::MODE;
        case SDLK_AUDIONEXT: return core::Key::AUDIO_NEXT;
        case SDLK_AUDIOPREV: return core::Key::AUDIO_PREV;
        case SDLK_AUDIOSTOP: return core::Key::AUDIO_STOP;
        case SDLK_AUDIOPLAY: return core::Key::AUDIO_PLAY;
        case SDLK_AUDIOMUTE: return core::Key::AUDIO_MUTE;
        case SDLK_MEDIASELECT: return core::Key::MEDIASELECT;
        case SDLK_WWW: return core::Key::WWW;
        case SDLK_MAIL: return core::Key::MAIL;
        case SDLK_CALCULATOR: return core::Key::CALCULATOR;
        case SDLK_COMPUTER: return core::Key::COMPUTER;
        case SDLK_AC_SEARCH: return core::Key::AC_SEARCH;
        case SDLK_AC_HOME: return core::Key::AC_HOME;
        case SDLK_AC_BACK: return core::Key::AC_BACK;
        case SDLK_AC_FORWARD: return core::Key::AC_FORWARD;
        case SDLK_AC_STOP: return core::Key::AC_STOP;
        case SDLK_AC_REFRESH: return core::Key::AC_REFRESH;
        case SDLK_AC_BOOKMARKS: return core::Key::AC_BOOKMARKS;
        case SDLK_BRIGHTNESSDOWN: return core::Key::BRIGHTNESS_DOWN;
        case SDLK_BRIGHTNESSUP: return core::Key::BRIGHTNESS_UP;
        case SDLK_DISPLAYSWITCH: return core::Key::DISPLAYSWITCH;
        case SDLK_KBDILLUMTOGGLE: return core::Key::KEYBOARD_ILLUM_TOGGLE;
        case SDLK_KBDILLUMDOWN: return core::Key::KEYBOARD_ILLUM_DOWN;
        case SDLK_KBDILLUMUP: return core::Key::KEYBOARD_ILLUM_UP;
        case SDLK_EJECT: return core::Key::EJECT;
        case SDLK_SLEEP: return core::Key::SLEEP;
        default:
            // todo(Gustav): die should have a x version too
            ASSERTX(false && "Invalid keyboard button", key.sym);
            return core::Key::INVALID;
        }
    }

    core::MouseButton
    ToKey(SDL_MouseButtonEvent mb)
    {
        switch(mb.button)
        {
        case SDL_BUTTON_LEFT: return core::MouseButton::LEFT;
        case SDL_BUTTON_MIDDLE: return core::MouseButton::MIDDLE;
        case SDL_BUTTON_RIGHT: return core::MouseButton::RIGHT;
        case SDL_BUTTON_X1: return core::MouseButton::X1;
        case SDL_BUTTON_X2: return core::MouseButton::X2;
        default:
            ASSERTX(false && "Invalid mouse button", mb.button);
            return core::MouseButton::INVALID;
        }
    }

}  // namespace euphoria::window
