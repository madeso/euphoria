#include "core/palette_cubehelix.h"

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

namespace euphoria::core
{
    namespace
    {
        rgbi
        C(unsigned char r, unsigned char g, unsigned char b)
        {
            return {r, g, b};
        }
    }  // namespace

    namespace palette
    {
        const Palette&
        Classic()
        {
            // dict(start=0.5, rotation=-1.5, gamma=1.0, sat=1.2,
            //      min_light=0., max_light=1., n=16)
            static const auto p = Palette {"Classic",
                                           C(0, 0, 0),
                                           C(22, 10, 34),
                                           C(24, 32, 68),
                                           C(16, 62, 83),
                                           C(14, 94, 74),
                                           C(35, 116, 51),
                                           C(80, 125, 35),
                                           C(138, 122, 45),
                                           C(190, 117, 85),
                                           C(218, 121, 145),
                                           C(219, 138, 203),
                                           C(204, 167, 240),
                                           C(191, 201, 251),
                                           C(195, 229, 244),
                                           C(220, 246, 239),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Perceptual_rainbow()
        {
            // Similar to Matteo Niccoli's Perceptual Rainbow:
            // http://mycarta.wordpress.com/2013/02/21/perceptual-rainbow-palette-the-method/
            // https://github.com/jradavenport/cubehelix
            // dict(start_hue=240., end_hue=-300., min_sat=1., max_sat=2.5,
            //      min_light=0.3, max_light=0.8, gamma=.9, n=16)
            static const auto p = Palette {"Perceptual_rainbow",
                                           C(135, 59, 97),
                                           C(143, 64, 127),
                                           C(143, 72, 157),
                                           C(135, 85, 185),
                                           C(121, 102, 207),
                                           C(103, 123, 220),
                                           C(84, 146, 223),
                                           C(69, 170, 215),
                                           C(59, 192, 197),
                                           C(60, 210, 172),
                                           C(71, 223, 145),
                                           C(93, 229, 120),
                                           C(124, 231, 103),
                                           C(161, 227, 95),
                                           C(198, 220, 100),
                                           C(233, 213, 117)};
            return p;
        }

        const Palette&
        Purple()
        {
            // dict(start=0., rotation=0.0, n=16)
            static const auto p = Palette {"Purple",
                                           C(0, 0, 0),
                                           C(15, 14, 35),
                                           C(31, 28, 68),
                                           C(47, 43, 99),
                                           C(63, 59, 127),
                                           C(79, 75, 152),
                                           C(96, 91, 174),
                                           C(113, 107, 194),
                                           C(130, 124, 211),
                                           C(147, 142, 225),
                                           C(164, 160, 237),
                                           C(182, 178, 246),
                                           C(200, 196, 252),
                                           C(218, 215, 255),
                                           C(236, 235, 255),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Jim_special()
        {
            // http://www.ifweassume.com/2014/04/cubehelix-colormap-for-python.html
            // dict(start=0.3, rotation=-0.5, n=16)
            static const auto p = Palette {"Jim_special",
                                           C(0, 0, 0),
                                           C(22, 10, 34),
                                           C(37, 25, 68),
                                           C(47, 43, 99),
                                           C(52, 65, 125),
                                           C(55, 88, 146),
                                           C(59, 112, 160),
                                           C(64, 137, 169),
                                           C(74, 160, 173),
                                           C(89, 181, 175),
                                           C(109, 199, 177),
                                           C(134, 214, 180),
                                           C(163, 227, 189),
                                           C(195, 237, 203),
                                           C(226, 246, 225),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Red()
        {
            // http://www.ifweassume.com/2014/04/cubehelix-colormap-for-python.html
            // dict(start=0., rotation=0.5, n=16)
            static const auto p = Palette {"Red",
                                           C(0, 0, 0),
                                           C(19, 12, 35),
                                           C(44, 22, 65),
                                           C(73, 32, 90),
                                           C(104, 41, 107),
                                           C(134, 53, 118),
                                           C(162, 67, 124),
                                           C(185, 83, 126),
                                           C(204, 102, 128),
                                           C(216, 124, 130),
                                           C(225, 148, 136),
                                           C(229, 172, 147),
                                           C(232, 196, 164),
                                           C(236, 219, 189),
                                           C(242, 238, 219),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Cubehelix1()
        {
            // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
            // dict(gamma=1.0, start=1.5, rotation=-1.0, sat=1.5, n=16)
            static const auto p = Palette {"Cubehelix1",
                                           C(0, 0, 0),
                                           C(27, 15, 0),
                                           C(65, 23, 4),
                                           C(104, 27, 32),
                                           C(133, 33, 75),
                                           C(147, 45, 126),
                                           C(144, 66, 175),
                                           C(129, 96, 210),
                                           C(111, 131, 227),
                                           C(99, 166, 226),
                                           C(101, 197, 211),
                                           C(120, 219, 194),
                                           C(153, 233, 185),
                                           C(193, 240, 191),
                                           C(230, 245, 216),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Cubehelix2()
        {
            // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
            // dict(gamma=1.0, start=2.0, rotation=1.0, sat=1.5, n=16)
            static const auto p = Palette {"Cubehelix2",
                                           C(0, 0, 0),
                                           C(0, 28, 14),
                                           C(0, 51, 47),
                                           C(7, 65, 91),
                                           C(35, 71, 135),
                                           C(78, 72, 168),
                                           C(129, 72, 184),
                                           C(177, 77, 181),
                                           C(214, 90, 165),
                                           C(235, 113, 143),
                                           C(238, 142, 128),
                                           C(230, 175, 127),
                                           C(219, 206, 144),
                                           C(216, 231, 178),
                                           C(226, 247, 219),
                                           C(255, 255, 255)};
            return p;
        }

        const Palette&
        Cubehelix3()
        {
            // http://nbviewer.ipython.org/gist/anonymous/a4fa0adb08f9e9ea4f94
            // dict(gamma=1.0, start=2.0, rotation=1.0, sat=3, n=16)
            static const auto p = Palette {"Cubehelix3",
                                           C(0, 0, 0),
                                           C(0, 39, 12),
                                           C(0, 68, 60),
                                           C(0, 80, 131),
                                           C(3, 75, 202),
                                           C(72, 60, 252),
                                           C(156, 43, 255),
                                           C(235, 36, 244),
                                           C(255, 45, 194),
                                           C(255, 73, 134),
                                           C(255, 115, 86),
                                           C(255, 164, 67),
                                           C(235, 209, 85),
                                           C(211, 241, 135),
                                           C(215, 255, 200),
                                           C(255, 255, 255)};
            return p;
        }

    }  // namespace palette

}  // namespace euphoria::core
