#pragma once

#include <string>

namespace euphoria::core
{
    enum class mouse_button
    {
        invalid,
        unbound,
        left,
        middle,
        right,
        x1,
        x2
    };
    
    std::string
    to_string(mouse_button button);
    
    mouse_button
    to_mouse_button(const std::string& button);


    enum class key
    {
        invalid,
        unbound,
        return_key,
        escape,
        backspace,
        tab,
        space,
        exclaim,
        quotedbl,
        hash,
        percent,
        dollar,
        ampersand,
        quote,
        left_paren,
        right_paren,
        asterix,
        plus,
        comma,
        minus,
        period,
        slash,
        num_0,
        num_1,
        num_2,
        num_3,
        num_4,
        num_5,
        num_6,
        num_7,
        num_8,
        num_9,
        colon,
        semicolon,
        less,
        equals,
        greater,
        question,
        at,
        left_bracket,
        backslash,
        right_bracket,
        caret,
        underscore,
        backquote,
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,
        capslock,
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,
        print_screen,
        scroll_lock,
        pause,
        insert,
        home,
        page_up,
        delete_key,
        end,
        page_down,
        right,
        left,
        down,
        up,
        numlock_clear,
        keypad_divide,
        keypad_multiply,
        keypad_minus,
        keypad_plus,
        keypad_enter,
        keypad_1,
        keypad_2,
        keypad_3,
        keypad_4,
        keypad_5,
        keypad_6,
        keypad_7,
        keypad_8,
        keypad_9,
        keypad_0,
        keypad_period,
        application,
        power,
        keypad_equals,
        f13,
        f14,
        f15,
        f16,
        f17,
        f18,
        f19,
        f20,
        f21,
        f22,
        f23,
        f24,
        execute,
        help,
        menu,
        select,
        stop,
        again,
        undo,
        cut,
        copy,
        paste,
        find,
        mute,
        volume_up,
        volume_down,
        keypad_comma,
        keypad_equals_as_400,
        alt_erase,
        sysreq,
        cancel,
        clear,
        prior,
        second_return,
        separator,
        out,
        oper,
        clear_again,
        crsel,
        exsel,
        keypad_00,
        keypad_000,
        thousandseparator,
        decimalseparator,
        currency_unit,
        currency_subunit,
        keypad_leftparen,
        keypad_rightparen,
        keypad_leftbrace,
        keypad_rightbrace,
        keypad_tab,
        keypad_backspace,
        keypad_a,
        keypad_b,
        keypad_c,
        keypad_d,
        keypad_e,
        keypad_f,
        keypad_xor,
        keypad_power,
        keypad_percent,
        keypad_less,
        keypad_greater,
        keypad_ampersand,
        keypad_double_ampersand,
        keypad_vertical_bar,
        keypad_double_verticle_bar,
        keypad_colon,
        keypad_hash,
        keypad_space,
        keypad_at,
        keypad_exclam,
        keypad_mem_store,
        keypad_mem_recall,
        keypad_mem_clear,
        keypad_mem_add,
        keypad_mem_subtract,
        keypad_mem_multiply,
        keypad_mem_divide,
        keypad_plus_minus,
        keypad_clear,
        keypad_clear_entry,
        keypad_binary,
        keypad_octal,
        keypad_decimal,
        keypad_hexadecimal,
        ctrl_left,
        shift_left,
        alt_left,
        gui_left,
        ctrl_right,
        shift_right,
        alt_right,
        gui_right,
        mode,
        audio_next,
        audio_prev,
        audio_stop,
        audio_play,
        audio_mute,
        media_select,
        www,
        mail,
        calculator,
        computer,
        ac_search,
        ac_home,
        ac_back,
        ac_forward,
        ac_stop,
        ac_refresh,
        ac_bookmarks,
        brightness_down,
        brightness_up,
        displayswitch,
        keyboard_illum_toggle,
        keyboard_illum_down,
        keyboard_illum_up,
        eject,
        sleep
    };
    
    std::string
    to_string(key k);
    
    key
    to_key(const std::string& name);

}

