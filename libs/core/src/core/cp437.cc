#include "core/cp437.h"

#include <array>

#include "assert/assert.h"
#include "core/random.h"
#include "core/range.h"

namespace
{
    constexpr std::array<std::string_view, 256> code_page =
    {
  //        _0        _1       _2      _3     _4     _5      _6      _7     _8     _9     _A    _B      _C     _D     _E      _F
/*0_*/     "NULL",   "☺",   "☻",    "♥",   "♦",   "♣",   "♠",   "•",   "◘",   "○",   "◙",   "♂",   "♀",   "♪",   "♫",   "☼",
/*1_*/     "►",      "◄",    "↕",    "‼",    "¶",   "§",    "▬",   "↨",   "↑",   "↓",   "→",   "←",   "∟",   "↔",   "▲",   "▼",
/*2_*/     " ",      "!",    "\"",   "#",    "$",   "%",    "&",   "'",   "(",   ")",   "*",   "+",   ",",    "-",   ".",   "/",
/*3_*/     "0",      "1",    "2",    "3",    "4",   "5",    "6",   "7",   "8",   "9",   ":",   ";",   "<",    "=",   ">",   "?",
/*4_*/     "@",      "A",    "B",    "C",    "D",   "E",    "F",   "G",   "H",   "I",   "J",   "K",   "L",    "M",   "N",   "O",
/*5_*/     "P",      "Q",    "R",    "S",    "T",   "U",    "V",   "W",   "X",   "Y",   "Z",   "[",   "\\",   "]",   "^",   "_",
/*6_*/     "`",      "a",    "b",    "c",    "d",   "e",    "f",   "g",   "h",   "i",   "j",   "k",   "l",    "m",   "n",   "o",
/*7_*/     "p",      "q",    "r",    "s",    "t",   "u",    "v",   "w",   "x",   "y",   "z",   "{",   "|",    "}",   "~",   "⌂",
/*8_*/     "Ç",      "ü",    "é",    "â",    "ä",   "à",    "å",   "ç",   "ê",   "ë",   "è",   "ï",   "î",    "ì",   "Ä",   "Å",
/*9_*/     "É",      "æ",    "Æ",    "ô",    "ö",   "ò",    "û",   "ù",   "ÿ",   "Ö",   "Ü",   "¢",   "£",    "¥",   "₧",   "ƒ",
/*A_*/     "á",      "í",    "ó",    "ú",    "ñ",   "Ñ",    "ª",   "º",   "¿",   "⌐",   "¬",   "½",   "¼",    "¡",   "«",   "»",
/*B_*/     "░",      "▒",    "▓",    "│",    "┤",   "╡",    "╢",   "╖",   "╕",   "╣",   "║",   "╗",   "╝",    "╜",   "╛",   "┐",
/*C_*/     "└",      "┴",    "┬",    "├",    "─",   "┼",    "╞",   "╟",   "╚",   "╔",   "╩",   "╦",   "╠",    "═",   "╬",   "╧",
/*D_*/     "╨",      "╤",    "╥",    "╙",    "╘",   "╒",    "╓",   "╫",   "╪",   "┘",   "┌",   "█",   "▄",    "▌",   "▐",   "▀",
/*E_*/     "α",      "ß",    "Γ",    "π",    "Σ",   "σ",    "µ",   "τ",   "Φ",   "Θ",   "Ω",   "δ",   "∞",    "φ",   "ε",   "∩",
/*F_*/     "≡",      "±",    "≥",    "≤",    "⌠",   "⌡",    "÷",   "≈",   "°",   "∙",   "·",   "√",   "ⁿ",    "²",   "■",   "nbsp"
    };
}

namespace eu::core
{
    std::string_view
    get_cp437(int c)
    {
        ASSERTX(is_within_inclusive_as_int(0, c, 255), c);
        return code_page[c];
    }

    std::string_view
    get_random_cp437_in_utf8(Random* random)
    {
        while(true)
        {
            const auto c = get_random_in_range(random, 256);
            if(c == 0x00) { continue; } // not NULL
            if(c == 0x20) { continue; } // neither Space
            if(c == 0xFF) { continue; } // neither NBSP
            return get_cp437(c);
        }
    }
}
