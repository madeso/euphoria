#pragma once

#include "core/palette.h"


// original source: https://github.com/jiffyclub/palettable/blob/master/palettable/cubehelix/cubehelix.py

/*
Copyright (c) 2014, James R. A. Davenport and contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

namespace euphoria::core::palettes::cubehelix
{
    constexpr Rgbi
    con(unsigned char r, unsigned char g, unsigned char b)
    {
        return {r, g, b};
    }

    constexpr auto classic = make_static_palette
    (
        // dict(start=0.5, rotation=-1.5, gamma=1.0, sat=1.2,
        //      min_light=0., max_light=1., n=16)
        "Classic",
        con(0, 0, 0), con(22, 10, 34), con(24, 32, 68),
        con(16, 62, 83), con(14, 94, 74), con(35, 116, 51),
        con(80, 125, 35), con(138, 122, 45), con(190, 117, 85),
        con(218, 121, 145), con(219, 138, 203), con(204, 167, 240),
        con(191, 201, 251), con(195, 229, 244), con(220, 246, 239),
        con(255, 255, 255)
    );

    constexpr auto perceptual_rainbow = make_static_palette
    (
        // Similar to Matteo Niccoli's Perceptual Rainbow:
        // http://mycarta.wordpress.com/2013/02/21/perceptual-rainbow-palette-the-method/
        // https://github.com/jradavenport/cubehelix
        // dict(start_hue=240., end_hue=-300., min_sat=1., max_sat=2.5,
        //      min_light=0.3, max_light=0.8, gamma=.9, n=16)
        "Perceptual_rainbow",
        con(135, 59, 97), con(143, 64, 127), con(143, 72, 157),
        con(135, 85, 185), con(121, 102, 207), con(103, 123, 220),
        con(84, 146, 223), con(69, 170, 215), con(59, 192, 197),
        con(60, 210, 172), con(71, 223, 145), con(93, 229, 120),
        con(124, 231, 103), con(161, 227, 95), con(198, 220, 100),
        con(233, 213, 117)
    );

    constexpr auto purple = make_static_palette
    (
        // dict(start=0., rotation=0.0, n=16)
        "Purple",
        con(0, 0, 0), con(15, 14, 35), con(31, 28, 68),
        con(47, 43, 99), con(63, 59, 127), con(79, 75, 152),
        con(96, 91, 174), con(113, 107, 194), con(130, 124, 211),
        con(147, 142, 225), con(164, 160, 237), con(182, 178, 246),
        con(200, 196, 252), con(218, 215, 255), con(236, 235, 255),
        con(255, 255, 255)
    );

    constexpr auto jim_special = make_static_palette
    (
        // http://www.ifweassume.com/2014/04/cubehelix-colormap-for-python.html
        // dict(start=0.3, rotation=-0.5, n=16)
        "Jim_special",
        con(0, 0, 0), con(22, 10, 34), con(37, 25, 68),
        con(47, 43, 99), con(52, 65, 125), con(55, 88, 146),
        con(59, 112, 160), con(64, 137, 169), con(74, 160, 173),
        con(89, 181, 175), con(109, 199, 177), con(134, 214, 180),
        con(163, 227, 189), con(195, 237, 203), con(226, 246, 225),
        con(255, 255, 255)
    );

    constexpr auto red = make_static_palette
    (
        // http://www.ifweassume.com/2014/04/cubehelix-colormap-for-python.html
        // dict(start=0., rotation=0.5, n=16)
        "Red",
        con(0, 0, 0), con(19, 12, 35), con(44, 22, 65),
        con(73, 32, 90), con(104, 41, 107), con(134, 53, 118),
        con(162, 67, 124), con(185, 83, 126), con(204, 102, 128),
        con(216, 124, 130), con(225, 148, 136), con(229, 172, 147),
        con(232, 196, 164), con(236, 219, 189), con(242, 238, 219),
        con(255, 255, 255)
    );

    constexpr auto cubehelix_1 = make_static_palette
    (
        // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
        // dict(gamma=1.0, start=1.5, rotation=-1.0, sat=1.5, n=16)
        "Cubehelix1",
        con(0, 0, 0), con(27, 15, 0), con(65, 23, 4),
        con(104, 27, 32), con(133, 33, 75), con(147, 45, 126),
        con(144, 66, 175), con(129, 96, 210), con(111, 131, 227),
        con(99, 166, 226), con(101, 197, 211), con(120, 219, 194),
        con(153, 233, 185), con(193, 240, 191), con(230, 245, 216),
        con(255, 255, 255)
    );

    constexpr auto cubehelix_2 = make_static_palette
    (
        // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
        // dict(gamma=1.0, start=2.0, rotation=1.0, sat=1.5, n=16)
        "Cubehelix2",
        con(0, 0, 0), con(0, 28, 14), con(0, 51, 47),
        con(7, 65, 91), con(35, 71, 135), con(78, 72, 168),
        con(129, 72, 184), con(177, 77, 181), con(214, 90, 165),
        con(235, 113, 143), con(238, 142, 128), con(230, 175, 127),
        con(219, 206, 144), con(216, 231, 178), con(226, 247, 219),
        con(255, 255, 255)
    );

    constexpr auto cubehelix_3 = make_static_palette
    (
        // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
        // dict(gamma=1.0, start=2.0, rotation=1.0, sat=3, n=16)
        "Cubehelix3",
        con(0, 0, 0), con(0, 39, 12), con(0, 68, 60),
        con(0, 80, 131), con(3, 75, 202), con(72, 60, 252),
        con(156, 43, 255), con(235, 36, 244), con(255, 45, 194),
        con(255, 73, 134), con(255, 115, 86), con(255, 164, 67),
        con(235, 209, 85), con(211, 241, 135), con(215, 255, 200),
        con(255, 255, 255)
    );


}
