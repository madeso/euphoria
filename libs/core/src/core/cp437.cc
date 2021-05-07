#include "core/cp437.h"

#include <array>

#include "core/assert.h"
#include "core/random.h"
#include "core/range.h"

namespace
{
    constexpr std::array<std::string_view, 256> CODE_PAGE =
    {
  //        _0        _1       _2      _3     _4     _5      _6      _7     _8     _9     _A    _B      _C     _D     _E      _F
/*0_*/   u8"NULL", u8"☺", u8"☻",  u8"♥", u8"♦", u8"♣", u8"♠", u8"•", u8"◘", u8"○", u8"◙", u8"♂", u8"♀", u8"♪", u8"♫", u8"☼",
/*1_*/   u8"►",    u8"◄",  u8"↕",  u8"‼",  u8"¶", u8"§",  u8"▬", u8"↨", u8"↑", u8"↓", u8"→", u8"←", u8"∟", u8"↔", u8"▲", u8"▼",
/*2_*/   u8" ",    u8"!",  u8"\"", u8"#",  u8"$", u8"%",  u8"&", u8"'", u8"(", u8")", u8"*", u8"+", u8",",  u8"-", u8".", u8"/",
/*3_*/   u8"0",    u8"1",  u8"2",  u8"3",  u8"4", u8"5",  u8"6", u8"7", u8"8", u8"9", u8":", u8";", u8"<",  u8"=", u8">", u8"?",
/*4_*/   u8"@",    u8"A",  u8"B",  u8"C",  u8"D", u8"E",  u8"F", u8"G", u8"H", u8"I", u8"J", u8"K", u8"L",  u8"M", u8"N", u8"O",
/*5_*/   u8"P",    u8"Q",  u8"R",  u8"S",  u8"T", u8"U",  u8"V", u8"W", u8"X", u8"Y", u8"Z", u8"[", u8"\\", u8"]", u8"^", u8"_",
/*6_*/   u8"`",    u8"a",  u8"b",  u8"c",  u8"d", u8"e",  u8"f", u8"g", u8"h", u8"i", u8"j", u8"k", u8"l",  u8"m", u8"n", u8"o",
/*7_*/   u8"p",    u8"q",  u8"r",  u8"s",  u8"t", u8"u",  u8"v", u8"w", u8"x", u8"y", u8"z", u8"{", u8"|",  u8"}", u8"~", u8"⌂",
/*8_*/   u8"Ç",    u8"ü",  u8"é",  u8"â",  u8"ä", u8"à",  u8"å", u8"ç", u8"ê", u8"ë", u8"è", u8"ï", u8"î",  u8"ì", u8"Ä", u8"Å",
/*9_*/   u8"É",    u8"æ",  u8"Æ",  u8"ô",  u8"ö", u8"ò",  u8"û", u8"ù", u8"ÿ", u8"Ö", u8"Ü", u8"¢", u8"£",  u8"¥", u8"₧", u8"ƒ",
/*A_*/   u8"á",    u8"í",  u8"ó",  u8"ú",  u8"ñ", u8"Ñ",  u8"ª", u8"º", u8"¿", u8"⌐", u8"¬", u8"½", u8"¼",  u8"¡", u8"«", u8"»",
/*B_*/   u8"░",    u8"▒",  u8"▓",  u8"│",  u8"┤", u8"╡",  u8"╢", u8"╖", u8"╕", u8"╣", u8"║", u8"╗", u8"╝",  u8"╜", u8"╛", u8"┐",
/*C_*/   u8"└",    u8"┴",  u8"┬",  u8"├",  u8"─", u8"┼",  u8"╞", u8"╟", u8"╚", u8"╔", u8"╩", u8"╦", u8"╠",  u8"═", u8"╬", u8"╧",
/*D_*/   u8"╨",    u8"╤",  u8"╥",  u8"╙",  u8"╘", u8"╒",  u8"╓", u8"╫", u8"╪", u8"┘", u8"┌", u8"█", u8"▄",  u8"▌", u8"▐", u8"▀",
/*E_*/   u8"α",    u8"ß",  u8"Γ",  u8"π",  u8"Σ", u8"σ",  u8"µ", u8"τ", u8"Φ", u8"Θ", u8"Ω", u8"δ", u8"∞",  u8"φ", u8"ε", u8"∩",
/*F_*/   u8"≡",    u8"±",  u8"≥",  u8"≤",  u8"⌠", u8"⌡",  u8"÷", u8"≈", u8"°", u8"∙", u8"·", u8"√", u8"ⁿ",  u8"²", u8"■", u8"nbsp"
    };
}

namespace euphoria::core
{
    std::string_view
    get_cp437(int c)
    {
        ASSERTX(is_within_inclusive_as_int(0, c, 255), c);
        return CODE_PAGE[c];
    }

    std::string_view
    get_random_cp437_in_utf8(random* random)
    {
        while(true)
        {
            const auto c = get_next_range(random, 256);
            if(c == 0x00) { continue; } // not NULL
            if(c == 0x20) { continue; } // neither Space
            if(c == 0xFF) { continue; } // neither NBSP
            return get_cp437(c);
        }
    }
}
