#pragma once

#include "core/palette.h"


namespace euphoria::core::palettes::lospec
{
    constexpr rgbi
    con(unsigned int hex)
    {
        return rgbi::from_hex(hex);
    }

    // collected by https://github.com/peterhellberg/gfx/blob/master/palettes.go

    constexpr auto one_bit = make_static_palette
    (
        "OneBit",
        con(0x000000), con(0xFFFFFF)
    );

    // A grayscale palette calculated using 2-bits per color.
    // It was used by the original gameboy and a few other computer systems.
    // https://lospec.com/palette-list/2-bit-grayscale
    constexpr auto two_bit_grayscale = make_static_palette
    (
        "TwoBitGrayScale",
        con(0x000000), con(0x676767), con(0xB6B6B6), con(0xFFFFFF)
    );

    // A calculated palette using 1 bit for each RGB value.
    // It was used by a number of early computers.
    constexpr auto three_bit = make_static_palette
    (
        "ThreeBit",
        con(0x000000), con(0xFF0000), con(0x00FF00), con(0x0000FF),
        con(0x00FFFF), con(0xFF00FF), con(0xFFFF00), con(0xFFFFFF)
    );


    // Created by Endesga. #ARQ4
    // https://lospec.com/palette-list/arq4
    constexpr auto arq4 = make_static_palette
    (
        "ARQ4",
        con(0xFFFFFF), con(0x6772A9), con(0x3A3277), con(0x000000)
    );

    // Color Graphics Adapter Palette.
    // CGA was a graphics card released in 1981 for the IBM PC.
    // The standard mode uses one of two 4-color palettes
    // (each with a low-intensity and high-intensity mode),
    // but a hack allows use of all 16. #cga
    // https://lospec.com/palette-list/color-graphics-adapter
    constexpr auto cga = make_static_palette
    (
        "CGA",
        con(0x000000), con(0x555555), con(0xAAAAAA), con(0xFFFFFF),
        con(0x0000AA), con(0x5555FF), con(0x00AA00), con(0x55FF55),
        con(0x00AAAA), con(0x55FFFF), con(0xAA0000), con(0xFF5555),
        con(0xAA00AA), con(0xFF55FF), con(0xAA5500), con(0xFFFF55)
    );

    // Endesga 8 Palette.
    // Created by Endesga. #EDG8
    // https://lospec.com/palette-list/endesga-8
    constexpr auto endesga_8 = make_static_palette
    (
        "EDG8",
        con(0xFDFDF8), con(0xD32734), con(0xDA7D22), con(0xE6DA29),
        con(0x28C641), con(0x2D93DD), con(0x7B53AD), con(0x1B1C33)
    );

    // Endesga 16 Palette.
    // Created by Endesga. #EDG16
    // https://lospec.com/palette-list/endesga-16
    constexpr auto endesga_16 = make_static_palette
    (
        "EDG16",
        con(0xE4A672), con(0xB86F50), con(0x743F39), con(0x3F2832),
        con(0x9E2835), con(0xE53B44), con(0xFB922B), con(0xFFE762),
        con(0x63C64D), con(0x327345), con(0x193D3F), con(0x4F6781),
        con(0xAFBFD2), con(0xFFFFFF), con(0x2CE8F4), con(0x0484D1)
    );


    // Endesga 32 Palette.
    // Created by Endesga for his game NYKRA. #EDG32
    // https://lospec.com/palette-list/endesga-32
    constexpr auto endesga_32 = make_static_palette
    (
        "EDG32",
        con(0xBE4A2F), con(0xD77643), con(0xEAD4AA), con(0xE4A672),
        con(0xB86F50), con(0x733E39), con(0x3E2731), con(0xA22633),
        con(0xE43B44), con(0xF77622), con(0xFEAE34), con(0xFEE761),
        con(0x63C74D), con(0x3E8948), con(0x265C42), con(0x193C3E),
        con(0x124E89), con(0x0099DB), con(0x2CE8F5), con(0xFFFFFF),
        con(0xC0CBDC), con(0x8B9BB4), con(0x5A6988), con(0x3A4466),
        con(0x262B44), con(0x181425), con(0xFF0044), con(0x68386C),
        con(0xB55088), con(0xF6757A), con(0xE8B796), con(0xC28569)
    );


    // Endesga 36 Palette.
    // Created by Endesga. #EDG36
    // https://lospec.com/palette-list/endesga-36
    constexpr auto endesga_36 = make_static_palette
    (
        "EDG36",
        con(0xDBE0E7), con(0xA3ACBE), con(0x67708B), con(0x4E5371),
        con(0x393A56), con(0x26243A), con(0x141020), con(0x7BCF5C),
        con(0x509B4B), con(0x2E6A42), con(0x1A453B), con(0x0F2738),
        con(0x0D2F6D), con(0x0F4DA3), con(0x0E82CE), con(0x13B2F2),
        con(0x41F3FC), con(0xF0D2AF), con(0xE5AE78), con(0xC58158),
        con(0x945542), con(0x623530), con(0x46211F), con(0x97432A),
        con(0xE57028), con(0xF7AC37), con(0xFBDF6B), con(0xFE979B),
        con(0xED5259), con(0xC42C36), con(0x781F2C), con(0x351428),
        con(0x4D2352), con(0x7F3B86), con(0xB45EB3), con(0xE38DD6)
    );


    // Endesga 64 Palette.
    // "Honed over years of palette creation, refined for materialistic pixelart
    // and design. High contrast, high saturation, shaped around painting the
    // organic and structured life of the heptaverse." Created by Endesga.
    // #EDG64
    // https://lospec.com/palette-list/endesga-64
    constexpr auto endesga_64 = make_static_palette
    (
        "EDG64",
        con(0xFF0040), con(0x131313), con(0x1B1B1B), con(0x272727),
        con(0x3D3D3D), con(0x5D5D5D), con(0x858585), con(0xB4B4B4),
        con(0xFFFFFF), con(0xC7CFDD), con(0x92A1B9), con(0x657392),
        con(0x424C6E), con(0x2A2F4E), con(0x1A1932), con(0x0E071B),
        con(0x1C121C), con(0x391F21), con(0x5D2C28), con(0x8A4836),
        con(0xBF6F4A), con(0xE69C69), con(0xF6CA9F), con(0xF9E6CF),
        con(0xEDAB50), con(0xE07438), con(0xC64524), con(0x8E251D),
        con(0xFF5000), con(0xED7614), con(0xFFA214), con(0xFFC825),
        con(0xFFEB57), con(0xD3FC7E), con(0x99E65F), con(0x5AC54F),
        con(0x33984B), con(0x1E6F50), con(0x134C4C), con(0x0C2E44),
        con(0x00396D), con(0x0069AA), con(0x0098DC), con(0x00CDF9),
        con(0x0CF1FF), con(0x94FDFF), con(0xFDD2ED), con(0xF389F5),
        con(0xDB3FFD), con(0x7A09FA), con(0x3003D9), con(0x0C0293),
        con(0x03193F), con(0x3B1443), con(0x622461), con(0x93388F),
        con(0xCA52C9), con(0xC85086), con(0xF68187), con(0xF5555D),
        con(0xEA323C), con(0xC42430), con(0x891E2B), con(0x571C27)
    );


    // Created by Endesga. #EN4
    // https://lospec.com/palette-list/en4
    constexpr auto endesga_4 = make_static_palette
    (
        "EN4",
        con(0xFBF7F3), con(0xE5B083), con(0x426E5D), con(0x20283D)
    );

    // Created by AprilSundae.
    // https://lospec.com/palette-list/ink
    constexpr auto ink = make_static_palette
    (
        "Ink",
        con(0x1F1F29), con(0x413A42), con(0x596070), con(0x96A2B3),
        con(0xEAF0D8)
    );

    // The palette used by PICO-8.
    // The PICO-8 is a virtual video game console created by Lexaloffle Games.
    // https://lospec.com/palette-list/pico-8
    constexpr auto pico8 = make_static_palette
    (
        "Pico8",
        con(0x000000), con(0x5F574F), con(0xC2C3C7), con(0xFFF1E8),
        con(0xFFEC27), con(0xFFA300), con(0xFFCCAA), con(0xAB5236),
        con(0xFF77A8), con(0xFF004D), con(0x83769C), con(0x7E2553),
        con(0x29ADFF), con(0x1D2B53), con(0x008751), con(0x00E436)
    );


    // Ammo-8 created by rsvp asap.
    // https://lospec.com/palette-list/ammo-8
    constexpr auto ammo8 = make_static_palette
    (
        "Ammo8",
        con(0x040C06), con(0x112318), con(0x1E3A29), con(0x305D42),
        con(0x4D8061), con(0x89A257), con(0xBEDC7F), con(0xEEFFCC)
    );


    // Created by Javier Guerrero.
    // https://lospec.com/palette-list/nyx8
    constexpr auto nyx8 = make_static_palette
    (
        "NYX8",
        con(0x08141E), con(0x0F2A3F), con(0x20394F), con(0xF6D6BD),
        con(0xC3A38A), con(0x997577), con(0x816271), con(0x4E495F)
    );


    // 15P DX palette by GrafxKid.
    // https://lospec.com/palette-list/15p-dx
    constexpr auto fifteen_p_dx = make_static_palette
    (
        "15p DX",
        con(0x6E3232), con(0xBB5735), con(0xDF9245),
        con(0xECD274), con(0x83A816), con(0x277224),
        con(0x173B47), con(0x046894), con(0x17A1A9),
        con(0x81DBCD), con(0xFDF9F1), con(0xC7B295),
        con(0x87715B), con(0x463F3C), con(0x201708)
    );

    // 20P DX palette by GrafxKid.
    // https://lospec.com/palette-list/20p-dx
    constexpr auto twenty_p_dx = make_static_palette
    (
        "20p DX",
        con(0x170D20), con(0x474757), con(0x787876), con(0xB1B9A6),
        con(0xEBFFDA), con(0x68293E), con(0xA94400), con(0xD97E00),
        con(0xEBD000), con(0x523C14), con(0x816031), con(0xBC8B57),
        con(0xEBCD93), con(0x0E4C58), con(0x046E92), con(0x01A3C3),
        con(0x55DEB7), con(0x177947), con(0x5AB217), con(0xB1E329)
    );

    // Created by Arne.
    // https://lospec.com/palette-list/arne-16
    constexpr auto arne16 = make_static_palette
    (
        "Arne16",
        con(0x000000), con(0x493C2B), con(0xBE2633), con(0xE06F8B),
        con(0x9D9D9D), con(0xA46422), con(0xEB8931), con(0xF7E26B),
        con(0xFFFFFF), con(0x1B2632), con(0x2F484E), con(0x44891A),
        con(0xA3CE27), con(0x005784), con(0x31A2F2), con(0xB2DCEF)
    );

    // 3rd place winner of the PixelJoint 16 color palette competition (2015).
    // Created by Night.
    // https://lospec.com/palette-list/night-16
    constexpr auto night16 = make_static_palette
    (
        "Night16",
        con(0x0F0F1E), con(0xFFF8BC), con(0x0C2133), con(0x48586D),
        con(0x79A0B0), con(0xB0CE9D), con(0x657F49), con(0x3F4536),
        con(0xB99D6A), con(0xFFDD91), con(0xDD945B), con(0x9A5142),
        con(0x644B48), con(0x333033), con(0x767088), con(0xC5A3B3)
    );

    // AAP-16 created by Adigun Polack, meant for beginners.
    // https://lospec.com/palette-list/aap-16
    constexpr auto aap16 = make_static_palette
    (
        "AAP16",
        con(0x070708), con(0x332222), con(0x774433), con(0xCC8855),
        con(0x993311), con(0xDD7711), con(0xFFDD55), con(0xFFFF33),
        con(0x55AA44), con(0x115522), con(0x44EEBB), con(0x3388DD),
        con(0x5544AA), con(0x555577), con(0xAABBBB), con(0xFFFFFF)
    );

    // AAP-64 palette created by Adigun Polack.
    // https://lospec.com/palette-list/aap-16
    constexpr auto aap64 = make_static_palette
    (
        "AAP64",
        con(0x060608), con(0x141013), con(0x3B1725), con(0x73172D),
        con(0xB4202A), con(0xDF3E23), con(0xFA6A0A), con(0xF9A31B),
        con(0xFFD541), con(0xFFFC40), con(0xD6F264), con(0x9CDB43),
        con(0x59C135), con(0x14A02E), con(0x1A7A3E), con(0x24523B),
        con(0x122020), con(0x143464), con(0x285CC4), con(0x249FDE),
        con(0x20D6C7), con(0xA6FCDB), con(0xFFFFFF), con(0xFEF3C0),
        con(0xFAD6B8), con(0xF5A097), con(0xE86A73), con(0xBC4A9B),
        con(0x793A80), con(0x403353), con(0x242234), con(0x221C1A),
        con(0x322B28), con(0x71413B), con(0xBB7547), con(0xDBA463),
        con(0xF4D29C), con(0xDAE0EA), con(0xB3B9D1), con(0x8B93AF),
        con(0x6D758D), con(0x4A5462), con(0x333941), con(0x422433),
        con(0x5B3138), con(0x8E5252), con(0xBA756A), con(0xE9B5A3),
        con(0xE3E6FF), con(0xB9BFFB), con(0x849BE4), con(0x588DBE),
        con(0x477D85), con(0x23674E), con(0x328464), con(0x5DAF8D),
        con(0x92DCBA), con(0xCDF7E2), con(0xE4D2AA), con(0xC7B08B),
        con(0xA08662), con(0x796755), con(0x5A4E44), con(0x423934)
    );

    // Splendor 128 palette created by Adigun Polack as a successor to
    // his AAP-64 palette. #Splendor128
    // https://lospec.com/palette-list/aap-splendor128
    constexpr auto splendor128 = make_static_palette
    (
        "Splendor128",
        con(0x050403), con(0x0E0C0C), con(0x2D1B1E), con(0x612721),
        con(0xB9451D), con(0xF1641F), con(0xFCA570), con(0xFFE0B7),
        con(0xFFFFFF), con(0xFFF089), con(0xF8C53A), con(0xE88A36),
        con(0xB05B2C), con(0x673931), con(0x271F1B), con(0x4C3D2E),
        con(0x855F39), con(0xD39741), con(0xF8F644), con(0xD5DC1D),
        con(0xADB834), con(0x7F8E44), con(0x586335), con(0x333C24),
        con(0x181C19), con(0x293F21), con(0x477238), con(0x61A53F),
        con(0x8FD032), con(0xC4F129), con(0xD0FFEA), con(0x97EDCA),
        con(0x59CF93), con(0x42A459), con(0x3D6F43), con(0x27412D),
        con(0x14121D), con(0x1B2447), con(0x2B4E95), con(0x2789CD),
        con(0x42BFE8), con(0x73EFE8), con(0xF1F2FF), con(0xC9D4FD),
        con(0x8AA1F6), con(0x4572E3), con(0x494182), con(0x7864C6),
        con(0x9C8BDB), con(0xCEAAED), con(0xFAD6FF), con(0xEEB59C),
        con(0xD480BB), con(0x9052BC), con(0x171516), con(0x373334),
        con(0x695B59), con(0xB28B78), con(0xE2B27E), con(0xF6D896),
        con(0xFCF7BE), con(0xECEBE7), con(0xCBC6C1), con(0xA69E9A),
        con(0x807B7A), con(0x595757), con(0x323232), con(0x4F342F),
        con(0x8C5B3E), con(0xC68556), con(0xD6A851), con(0xB47538),
        con(0x724B2C), con(0x452A1B), con(0x61683A), con(0x939446),
        con(0xC6B858), con(0xEFDD91), con(0xB5E7CB), con(0x86C69A),
        con(0x5D9B79), con(0x486859), con(0x2C3B39), con(0x171819),
        con(0x2C3438), con(0x465456), con(0x64878C), con(0x8AC4C3),
        con(0xAFE9DF), con(0xDCEAEE), con(0xB8CCD8), con(0x88A3BC),
        con(0x5E718E), con(0x485262), con(0x282C3C), con(0x464762),
        con(0x696682), con(0x9A97B9), con(0xC5C7DD), con(0xE6E7F0),
        con(0xEEE6EA), con(0xE3CDDF), con(0xBFA5C9), con(0x87738F),
        con(0x564F5B), con(0x322F35), con(0x36282B), con(0x654956),
        con(0x966888), con(0xC090A9), con(0xD4B8B8), con(0xEAE0DD),
        con(0xF1EBDB), con(0xDDCEBF), con(0xBDA499), con(0x886E6A),
        con(0x594D4D), con(0x33272A), con(0xB29476), con(0xE1BF89),
        con(0xF8E398), con(0xFFE9E3), con(0xFDC9C9), con(0xF6A2A8),
        con(0xE27285), con(0xB25266), con(0x64364B), con(0x2A1E23)
    );

    // Famicube palette created by Arne as part of his Famicube Project.
    // https://lospec.com/palette-list/famicube
    constexpr auto famicube = make_static_palette
    (
        "Famicube",
        con(0x000000), con(0x00177D), con(0x024ACA), con(0x0084FF),
        con(0x5BA8FF), con(0x98DCFF), con(0x9BA0EF), con(0x6264DC),
        con(0x3D34A5), con(0x211640), con(0x5A1991), con(0x6A31CA),
        con(0xA675FE), con(0xE2C9FF), con(0xFEC9ED), con(0xD59CFC),
        con(0xCC69E4), con(0xA328B3), con(0x871646), con(0xCF3C71),
        con(0xFF82CE), con(0xFFE9C5), con(0xF5B784), con(0xE18289),
        con(0xDA655E), con(0x823C3D), con(0x4F1507), con(0xE03C28),
        con(0xE2D7B5), con(0xC59782), con(0xAE6C37), con(0x5C3C0D),
        con(0x231712), con(0xAD4E1A), con(0xF68F37), con(0xFFE737),
        con(0xFFBB31), con(0xCC8F15), con(0x939717), con(0xB6C121),
        con(0xEEFFA9), con(0xBEEB71), con(0x8CD612), con(0x6AB417),
        con(0x376D03), con(0x172808), con(0x004E00), con(0x139D08),
        con(0x58D332), con(0x20B562), con(0x00604B), con(0x005280),
        con(0x0A98AC), con(0x25E2CD), con(0xBDFFCA), con(0x71A6A1),
        con(0x415D66), con(0x0D2030), con(0x151515), con(0x343434),
        con(0x7B7B7B), con(0xA8A8A8), con(0xD7D7D7), con(0xFFFFFF)
    );
}
