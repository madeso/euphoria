#pragma once

#include "core/palette.h"

// https://bids.github.io/colormap/


/*
New matplotlib colormaps by Nathaniel J. Smith, Stefan van der Walt,
and (in the case of viridis) Eric Firing.

This file and the colormaps in it are released under the CC0 license /
public domain dedication. We would appreciate credit if you use or
redistribute these colormaps, but do not impose any legal restrictions.

To the extent possible under law, the persons who associated CC0 with
mpl-colormaps have waived all copyright and related or neighboring rights
to mpl-colormaps.

You should have received a copy of the CC0 legalcode along with this
work. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


namespace eu::core::palettes::matplot
{
    constexpr Rgbi
    con(unsigned char r, unsigned char g, unsigned char b)
    {
        return {r, g, b};
    }

    constexpr auto magma = make_static_palette
    (
        "Magma",
        con(0, 0, 4), con(1, 0, 5), con(1, 1, 6),
        con(1, 1, 8), con(2, 1, 9), con(2, 2, 11),
        con(2, 2, 13), con(3, 3, 15), con(3, 3, 18),
        con(4, 4, 20), con(5, 4, 22), con(6, 5, 24),
        con(6, 5, 26), con(7, 6, 28), con(8, 7, 30),
        con(9, 7, 32), con(10, 8, 34), con(11, 9, 36),
        con(12, 9, 38), con(13, 10, 41), con(14, 11, 43),
        con(16, 11, 45), con(17, 12, 47), con(18, 13, 49),
        con(19, 13, 52), con(20, 14, 54), con(21, 14, 56),
        con(22, 15, 59), con(24, 15, 61), con(25, 16, 63),
        con(26, 16, 66), con(28, 16, 68), con(29, 17, 71),
        con(30, 17, 73), con(32, 17, 75), con(33, 17, 78),
        con(34, 17, 80), con(36, 18, 83), con(37, 18, 85),
        con(39, 18, 88), con(41, 17, 90), con(42, 17, 92),
        con(44, 17, 95), con(45, 17, 97), con(47, 17, 99),
        con(49, 17, 101), con(51, 16, 103), con(52, 16, 105),
        con(54, 16, 107), con(56, 16, 108), con(57, 15, 110),
        con(59, 15, 112), con(61, 15, 113), con(63, 15, 114),
        con(64, 15, 116), con(66, 15, 117), con(68, 15, 118),
        con(69, 16, 119), con(71, 16, 120), con(73, 16, 120),
        con(74, 16, 121), con(76, 17, 122), con(78, 17, 123),
        con(79, 18, 123), con(81, 18, 124), con(82, 19, 124),
        con(84, 19, 125), con(86, 20, 125), con(87, 21, 126),
        con(89, 21, 126), con(90, 22, 126), con(92, 22, 127),
        con(93, 23, 127), con(95, 24, 127), con(96, 24, 128),
        con(98, 25, 128), con(100, 26, 128), con(101, 26, 128),
        con(103, 27, 128), con(104, 28, 129), con(106, 28, 129),
        con(107, 29, 129), con(109, 29, 129), con(110, 30, 129),
        con(112, 31, 129), con(114, 31, 129), con(115, 32, 129),
        con(117, 33, 129), con(118, 33, 129), con(120, 34, 129),
        con(121, 34, 130), con(123, 35, 130), con(124, 35, 130),
        con(126, 36, 130), con(128, 37, 130), con(129, 37, 129),
        con(131, 38, 129), con(132, 38, 129), con(134, 39, 129),
        con(136, 39, 129), con(137, 40, 129), con(139, 41, 129),
        con(140, 41, 129), con(142, 42, 129), con(144, 42, 129),
        con(145, 43, 129), con(147, 43, 128), con(148, 44, 128),
        con(150, 44, 128), con(152, 45, 128), con(153, 45, 128),
        con(155, 46, 127), con(156, 46, 127), con(158, 47, 127),
        con(160, 47, 127), con(161, 48, 126), con(163, 48, 126),
        con(165, 49, 126), con(166, 49, 125), con(168, 50, 125),
        con(170, 51, 125), con(171, 51, 124), con(173, 52, 124),
        con(174, 52, 123), con(176, 53, 123), con(178, 53, 123),
        con(179, 54, 122), con(181, 54, 122), con(183, 55, 121),
        con(184, 55, 121), con(186, 56, 120), con(188, 57, 120),
        con(189, 57, 119), con(191, 58, 119), con(192, 58, 118),
        con(194, 59, 117), con(196, 60, 117), con(197, 60, 116),
        con(199, 61, 115), con(200, 62, 115), con(202, 62, 114),
        con(204, 63, 113), con(205, 64, 113), con(207, 64, 112),
        con(208, 65, 111), con(210, 66, 111), con(211, 67, 110),
        con(213, 68, 109), con(214, 69, 108), con(216, 69, 108),
        con(217, 70, 107), con(219, 71, 106), con(220, 72, 105),
        con(222, 73, 104), con(223, 74, 104), con(224, 76, 103),
        con(226, 77, 102), con(227, 78, 101), con(228, 79, 100),
        con(229, 80, 100), con(231, 82, 99), con(232, 83, 98),
        con(233, 84, 98), con(234, 86, 97), con(235, 87, 96),
        con(236, 88, 96), con(237, 90, 95), con(238, 91, 94),
        con(239, 93, 94), con(240, 95, 94), con(241, 96, 93),
        con(242, 98, 93), con(242, 100, 92), con(243, 101, 92),
        con(244, 103, 92), con(244, 105, 92), con(245, 107, 92),
        con(246, 108, 92), con(246, 110, 92), con(247, 112, 92),
        con(247, 114, 92), con(248, 116, 92), con(248, 118, 92),
        con(249, 120, 93), con(249, 121, 93), con(249, 123, 93),
        con(250, 125, 94), con(250, 127, 94), con(250, 129, 95),
        con(251, 131, 95), con(251, 133, 96), con(251, 135, 97),
        con(252, 137, 97), con(252, 138, 98), con(252, 140, 99),
        con(252, 142, 100), con(252, 144, 101), con(253, 146, 102),
        con(253, 148, 103), con(253, 150, 104), con(253, 152, 105),
        con(253, 154, 106), con(253, 155, 107), con(254, 157, 108),
        con(254, 159, 109), con(254, 161, 110), con(254, 163, 111),
        con(254, 165, 113), con(254, 167, 114), con(254, 169, 115),
        con(254, 170, 116), con(254, 172, 118), con(254, 174, 119),
        con(254, 176, 120), con(254, 178, 122), con(254, 180, 123),
        con(254, 182, 124), con(254, 183, 126), con(254, 185, 127),
        con(254, 187, 129), con(254, 189, 130), con(254, 191, 132),
        con(254, 193, 133), con(254, 194, 135), con(254, 196, 136),
        con(254, 198, 138), con(254, 200, 140), con(254, 202, 141),
        con(254, 204, 143), con(254, 205, 144), con(254, 207, 146),
        con(254, 209, 148), con(254, 211, 149), con(254, 213, 151),
        con(254, 215, 153), con(254, 216, 154), con(253, 218, 156),
        con(253, 220, 158), con(253, 222, 160), con(253, 224, 161),
        con(253, 226, 163), con(253, 227, 165), con(253, 229, 167),
        con(253, 231, 169), con(253, 233, 170), con(253, 235, 172),
        con(252, 236, 174), con(252, 238, 176), con(252, 240, 178),
        con(252, 242, 180), con(252, 244, 182), con(252, 246, 184),
        con(252, 247, 185), con(252, 249, 187), con(252, 251, 189),
        con(252, 253, 191)
    );

    constexpr auto inferno = make_static_palette
    (
        "Inferno",
        con(0, 0, 4), con(1, 0, 5), con(1, 1, 6),
        con(1, 1, 8), con(2, 1, 10), con(2, 2, 12),
        con(2, 2, 14), con(3, 2, 16), con(4, 3, 18),
        con(4, 3, 20), con(5, 4, 23), con(6, 4, 25),
        con(7, 5, 27), con(8, 5, 29), con(9, 6, 31),
        con(10, 7, 34), con(11, 7, 36), con(12, 8, 38),
        con(13, 8, 41), con(14, 9, 43), con(16, 9, 45),
        con(17, 10, 48), con(18, 10, 50), con(20, 11, 52),
        con(21, 11, 55), con(22, 11, 57), con(24, 12, 60),
        con(25, 12, 62), con(27, 12, 65), con(28, 12, 67),
        con(30, 12, 69), con(31, 12, 72), con(33, 12, 74),
        con(35, 12, 76), con(36, 12, 79), con(38, 12, 81),
        con(40, 11, 83), con(41, 11, 85), con(43, 11, 87),
        con(45, 11, 89), con(47, 10, 91), con(49, 10, 92),
        con(50, 10, 94), con(52, 10, 95), con(54, 9, 97),
        con(56, 9, 98), con(57, 9, 99), con(59, 9, 100),
        con(61, 9, 101), con(62, 9, 102), con(64, 10, 103),
        con(66, 10, 104), con(68, 10, 104), con(69, 10, 105),
        con(71, 11, 106), con(73, 11, 106), con(74, 12, 107),
        con(76, 12, 107), con(77, 13, 108), con(79, 13, 108),
        con(81, 14, 108), con(82, 14, 109), con(84, 15, 109),
        con(85, 15, 109), con(87, 16, 110), con(89, 16, 110),
        con(90, 17, 110), con(92, 18, 110), con(93, 18, 110),
        con(95, 19, 110), con(97, 19, 110), con(98, 20, 110),
        con(100, 21, 110), con(101, 21, 110), con(103, 22, 110),
        con(105, 22, 110), con(106, 23, 110), con(108, 24, 110),
        con(109, 24, 110), con(111, 25, 110), con(113, 25, 110),
        con(114, 26, 110), con(116, 26, 110), con(117, 27, 110),
        con(119, 28, 109), con(120, 28, 109), con(122, 29, 109),
        con(124, 29, 109), con(125, 30, 109), con(127, 30, 108),
        con(128, 31, 108), con(130, 32, 108), con(132, 32, 107),
        con(133, 33, 107), con(135, 33, 107), con(136, 34, 106),
        con(138, 34, 106), con(140, 35, 105), con(141, 35, 105),
        con(143, 36, 105), con(144, 37, 104), con(146, 37, 104),
        con(147, 38, 103), con(149, 38, 103), con(151, 39, 102),
        con(152, 39, 102), con(154, 40, 101), con(155, 41, 100),
        con(157, 41, 100), con(159, 42, 99), con(160, 42, 99),
        con(162, 43, 98), con(163, 44, 97), con(165, 44, 96),
        con(166, 45, 96), con(168, 46, 95), con(169, 46, 94),
        con(171, 47, 94), con(173, 48, 93), con(174, 48, 92),
        con(176, 49, 91), con(177, 50, 90), con(179, 50, 90),
        con(180, 51, 89), con(182, 52, 88), con(183, 53, 87),
        con(185, 53, 86), con(186, 54, 85), con(188, 55, 84),
        con(189, 56, 83), con(191, 57, 82), con(192, 58, 81),
        con(193, 58, 80), con(195, 59, 79), con(196, 60, 78),
        con(198, 61, 77), con(199, 62, 76), con(200, 63, 75),
        con(202, 64, 74), con(203, 65, 73), con(204, 66, 72),
        con(206, 67, 71), con(207, 68, 70), con(208, 69, 69),
        con(210, 70, 68), con(211, 71, 67), con(212, 72, 66),
        con(213, 74, 65), con(215, 75, 63), con(216, 76, 62),
        con(217, 77, 61), con(218, 78, 60), con(219, 80, 59),
        con(221, 81, 58), con(222, 82, 56), con(223, 83, 55),
        con(224, 85, 54), con(225, 86, 53), con(226, 87, 52),
        con(227, 89, 51), con(228, 90, 49), con(229, 92, 48),
        con(230, 93, 47), con(231, 94, 46), con(232, 96, 45),
        con(233, 97, 43), con(234, 99, 42), con(235, 100, 41),
        con(235, 102, 40), con(236, 103, 38), con(237, 105, 37),
        con(238, 106, 36), con(239, 108, 35), con(239, 110, 33),
        con(240, 111, 32), con(241, 113, 31), con(241, 115, 29),
        con(242, 116, 28), con(243, 118, 27), con(243, 120, 25),
        con(244, 121, 24), con(245, 123, 23), con(245, 125, 21),
        con(246, 126, 20), con(246, 128, 19), con(247, 130, 18),
        con(247, 132, 16), con(248, 133, 15), con(248, 135, 14),
        con(248, 137, 12), con(249, 139, 11), con(249, 140, 10),
        con(249, 142, 9), con(250, 144, 8), con(250, 146, 7),
        con(250, 148, 7), con(251, 150, 6), con(251, 151, 6),
        con(251, 153, 6), con(251, 155, 6), con(251, 157, 7),
        con(252, 159, 7), con(252, 161, 8), con(252, 163, 9),
        con(252, 165, 10), con(252, 166, 12), con(252, 168, 13),
        con(252, 170, 15), con(252, 172, 17), con(252, 174, 18),
        con(252, 176, 20), con(252, 178, 22), con(252, 180, 24),
        con(251, 182, 26), con(251, 184, 29), con(251, 186, 31),
        con(251, 188, 33), con(251, 190, 35), con(250, 192, 38),
        con(250, 194, 40), con(250, 196, 42), con(250, 198, 45),
        con(249, 199, 47), con(249, 201, 50), con(249, 203, 53),
        con(248, 205, 55), con(248, 207, 58), con(247, 209, 61),
        con(247, 211, 64), con(246, 213, 67), con(246, 215, 70),
        con(245, 217, 73), con(245, 219, 76), con(244, 221, 79),
        con(244, 223, 83), con(244, 225, 86), con(243, 227, 90),
        con(243, 229, 93), con(242, 230, 97), con(242, 232, 101),
        con(242, 234, 105), con(241, 236, 109), con(241, 237, 113),
        con(241, 239, 117), con(241, 241, 121), con(242, 242, 125),
        con(242, 244, 130), con(243, 245, 134), con(243, 246, 138),
        con(244, 248, 142), con(245, 249, 146), con(246, 250, 150),
        con(248, 251, 154), con(249, 252, 157), con(250, 253, 161),
        con(252, 255, 164)
    );

    constexpr auto plasma = make_static_palette
    (
        "Plasma",
        con(13, 8, 135), con(16, 7, 136), con(19, 7, 137),
        con(22, 7, 138), con(25, 6, 140), con(27, 6, 141),
        con(29, 6, 142), con(32, 6, 143), con(34, 6, 144),
        con(36, 6, 145), con(38, 5, 145), con(40, 5, 146),
        con(42, 5, 147), con(44, 5, 148), con(46, 5, 149),
        con(47, 5, 150), con(49, 5, 151), con(51, 5, 151),
        con(53, 4, 152), con(55, 4, 153), con(56, 4, 154),
        con(58, 4, 154), con(60, 4, 155), con(62, 4, 156),
        con(63, 4, 156), con(65, 4, 157), con(67, 3, 158),
        con(68, 3, 158), con(70, 3, 159), con(72, 3, 159),
        con(73, 3, 160), con(75, 3, 161), con(76, 2, 161),
        con(78, 2, 162), con(80, 2, 162), con(81, 2, 163),
        con(83, 2, 163), con(85, 2, 164), con(86, 1, 164),
        con(88, 1, 164), con(89, 1, 165), con(91, 1, 165),
        con(92, 1, 166), con(94, 1, 166), con(96, 1, 166),
        con(97, 0, 167), con(99, 0, 167), con(100, 0, 167),
        con(102, 0, 167), con(103, 0, 168), con(105, 0, 168),
        con(106, 0, 168), con(108, 0, 168), con(110, 0, 168),
        con(111, 0, 168), con(113, 0, 168), con(114, 1, 168),
        con(116, 1, 168), con(117, 1, 168), con(119, 1, 168),
        con(120, 1, 168), con(122, 2, 168), con(123, 2, 168),
        con(125, 3, 168), con(126, 3, 168), con(128, 4, 168),
        con(129, 4, 167), con(131, 5, 167), con(132, 5, 167),
        con(134, 6, 166), con(135, 7, 166), con(136, 8, 166),
        con(138, 9, 165), con(139, 10, 165), con(141, 11, 165),
        con(142, 12, 164), con(143, 13, 164), con(145, 14, 163),
        con(146, 15, 163), con(148, 16, 162), con(149, 17, 161),
        con(150, 19, 161), con(152, 20, 160), con(153, 21, 159),
        con(154, 22, 159), con(156, 23, 158), con(157, 24, 157),
        con(158, 25, 157), con(160, 26, 156), con(161, 27, 155),
        con(162, 29, 154), con(163, 30, 154), con(165, 31, 153),
        con(166, 32, 152), con(167, 33, 151), con(168, 34, 150),
        con(170, 35, 149), con(171, 36, 148), con(172, 38, 148),
        con(173, 39, 147), con(174, 40, 146), con(176, 41, 145),
        con(177, 42, 144), con(178, 43, 143), con(179, 44, 142),
        con(180, 46, 141), con(181, 47, 140), con(182, 48, 139),
        con(183, 49, 138), con(184, 50, 137), con(186, 51, 136),
        con(187, 52, 136), con(188, 53, 135), con(189, 55, 134),
        con(190, 56, 133), con(191, 57, 132), con(192, 58, 131),
        con(193, 59, 130), con(194, 60, 129), con(195, 61, 128),
        con(196, 62, 127), con(197, 64, 126), con(198, 65, 125),
        con(199, 66, 124), con(200, 67, 123), con(201, 68, 122),
        con(202, 69, 122), con(203, 70, 121), con(204, 71, 120),
        con(204, 73, 119), con(205, 74, 118), con(206, 75, 117),
        con(207, 76, 116), con(208, 77, 115), con(209, 78, 114),
        con(210, 79, 113), con(211, 81, 113), con(212, 82, 112),
        con(213, 83, 111), con(213, 84, 110), con(214, 85, 109),
        con(215, 86, 108), con(216, 87, 107), con(217, 88, 106),
        con(218, 90, 106), con(218, 91, 105), con(219, 92, 104),
        con(220, 93, 103), con(221, 94, 102), con(222, 95, 101),
        con(222, 97, 100), con(223, 98, 99), con(224, 99, 99),
        con(225, 100, 98), con(226, 101, 97), con(226, 102, 96),
        con(227, 104, 95), con(228, 105, 94), con(229, 106, 93),
        con(229, 107, 93), con(230, 108, 92), con(231, 110, 91),
        con(231, 111, 90), con(232, 112, 89), con(233, 113, 88),
        con(233, 114, 87), con(234, 116, 87), con(235, 117, 86),
        con(235, 118, 85), con(236, 119, 84), con(237, 121, 83),
        con(237, 122, 82), con(238, 123, 81), con(239, 124, 81),
        con(239, 126, 80), con(240, 127, 79), con(240, 128, 78),
        con(241, 129, 77), con(241, 131, 76), con(242, 132, 75),
        con(243, 133, 75), con(243, 135, 74), con(244, 136, 73),
        con(244, 137, 72), con(245, 139, 71), con(245, 140, 70),
        con(246, 141, 69), con(246, 143, 68), con(247, 144, 68),
        con(247, 145, 67), con(247, 147, 66), con(248, 148, 65),
        con(248, 149, 64), con(249, 151, 63), con(249, 152, 62),
        con(249, 154, 62), con(250, 155, 61), con(250, 156, 60),
        con(250, 158, 59), con(251, 159, 58), con(251, 161, 57),
        con(251, 162, 56), con(252, 163, 56), con(252, 165, 55),
        con(252, 166, 54), con(252, 168, 53), con(252, 169, 52),
        con(253, 171, 51), con(253, 172, 51), con(253, 174, 50),
        con(253, 175, 49), con(253, 177, 48), con(253, 178, 47),
        con(253, 180, 47), con(253, 181, 46), con(254, 183, 45),
        con(254, 184, 44), con(254, 186, 44), con(254, 187, 43),
        con(254, 189, 42), con(254, 190, 42), con(254, 192, 41),
        con(253, 194, 41), con(253, 195, 40), con(253, 197, 39),
        con(253, 198, 39), con(253, 200, 39), con(253, 202, 38),
        con(253, 203, 38), con(252, 205, 37), con(252, 206, 37),
        con(252, 208, 37), con(252, 210, 37), con(251, 211, 36),
        con(251, 213, 36), con(251, 215, 36), con(250, 216, 36),
        con(250, 218, 36), con(249, 220, 36), con(249, 221, 37),
        con(248, 223, 37), con(248, 225, 37), con(247, 226, 37),
        con(247, 228, 37), con(246, 230, 38), con(246, 232, 38),
        con(245, 233, 38), con(245, 235, 39), con(244, 237, 39),
        con(243, 238, 39), con(243, 240, 39), con(242, 242, 39),
        con(241, 244, 38), con(241, 245, 37), con(240, 247, 36),
        con(240, 249, 33)
    );


    constexpr auto viridis = make_static_palette
    (
        "Viridis",
        con(68, 1, 84), con(68, 2, 86), con(69, 4, 87),
        con(69, 5, 89), con(70, 7, 90), con(70, 8, 92),
        con(70, 10, 93), con(70, 11, 94), con(71, 13, 96),
        con(71, 14, 97), con(71, 16, 99), con(71, 17, 100),
        con(71, 19, 101), con(72, 20, 103), con(72, 22, 104),
        con(72, 23, 105), con(72, 24, 106), con(72, 26, 108),
        con(72, 27, 109), con(72, 28, 110), con(72, 29, 111),
        con(72, 31, 112), con(72, 32, 113), con(72, 33, 115),
        con(72, 35, 116), con(72, 36, 117), con(72, 37, 118),
        con(72, 38, 119), con(72, 40, 120), con(72, 41, 121),
        con(71, 42, 122), con(71, 44, 122), con(71, 45, 123),
        con(71, 46, 124), con(71, 47, 125), con(70, 48, 126),
        con(70, 50, 126), con(70, 51, 127), con(70, 52, 128),
        con(69, 53, 129), con(69, 55, 129), con(69, 56, 130),
        con(68, 57, 131), con(68, 58, 131), con(68, 59, 132),
        con(67, 61, 132), con(67, 62, 133), con(66, 63, 133),
        con(66, 64, 134), con(66, 65, 134), con(65, 66, 135),
        con(65, 68, 135), con(64, 69, 136), con(64, 70, 136),
        con(63, 71, 136), con(63, 72, 137), con(62, 73, 137),
        con(62, 74, 137), con(62, 76, 138), con(61, 77, 138),
        con(61, 78, 138), con(60, 79, 138), con(60, 80, 139),
        con(59, 81, 139), con(59, 82, 139), con(58, 83, 139),
        con(58, 84, 140), con(57, 85, 140), con(57, 86, 140),
        con(56, 88, 140), con(56, 89, 140), con(55, 90, 140),
        con(55, 91, 141), con(54, 92, 141), con(54, 93, 141),
        con(53, 94, 141), con(53, 95, 141), con(52, 96, 141),
        con(52, 97, 141), con(51, 98, 141), con(51, 99, 141),
        con(50, 100, 142), con(50, 101, 142), con(49, 102, 142),
        con(49, 103, 142), con(49, 104, 142), con(48, 105, 142),
        con(48, 106, 142), con(47, 107, 142), con(47, 108, 142),
        con(46, 109, 142), con(46, 110, 142), con(46, 111, 142),
        con(45, 112, 142), con(45, 113, 142), con(44, 113, 142),
        con(44, 114, 142), con(44, 115, 142), con(43, 116, 142),
        con(43, 117, 142), con(42, 118, 142), con(42, 119, 142),
        con(42, 120, 142), con(41, 121, 142), con(41, 122, 142),
        con(41, 123, 142), con(40, 124, 142), con(40, 125, 142),
        con(39, 126, 142), con(39, 127, 142), con(39, 128, 142),
        con(38, 129, 142), con(38, 130, 142), con(38, 130, 142),
        con(37, 131, 142), con(37, 132, 142), con(37, 133, 142),
        con(36, 134, 142), con(36, 135, 142), con(35, 136, 142),
        con(35, 137, 142), con(35, 138, 141), con(34, 139, 141),
        con(34, 140, 141), con(34, 141, 141), con(33, 142, 141),
        con(33, 143, 141), con(33, 144, 141), con(33, 145, 140),
        con(32, 146, 140), con(32, 146, 140), con(32, 147, 140),
        con(31, 148, 140), con(31, 149, 139), con(31, 150, 139),
        con(31, 151, 139), con(31, 152, 139), con(31, 153, 138),
        con(31, 154, 138), con(30, 155, 138), con(30, 156, 137),
        con(30, 157, 137), con(31, 158, 137), con(31, 159, 136),
        con(31, 160, 136), con(31, 161, 136), con(31, 161, 135),
        con(31, 162, 135), con(32, 163, 134), con(32, 164, 134),
        con(33, 165, 133), con(33, 166, 133), con(34, 167, 133),
        con(34, 168, 132), con(35, 169, 131), con(36, 170, 131),
        con(37, 171, 130), con(37, 172, 130), con(38, 173, 129),
        con(39, 173, 129), con(40, 174, 128), con(41, 175, 127),
        con(42, 176, 127), con(44, 177, 126), con(45, 178, 125),
        con(46, 179, 124), con(47, 180, 124), con(49, 181, 123),
        con(50, 182, 122), con(52, 182, 121), con(53, 183, 121),
        con(55, 184, 120), con(56, 185, 119), con(58, 186, 118),
        con(59, 187, 117), con(61, 188, 116), con(63, 188, 115),
        con(64, 189, 114), con(66, 190, 113), con(68, 191, 112),
        con(70, 192, 111), con(72, 193, 110), con(74, 193, 109),
        con(76, 194, 108), con(78, 195, 107), con(80, 196, 106),
        con(82, 197, 105), con(84, 197, 104), con(86, 198, 103),
        con(88, 199, 101), con(90, 200, 100), con(92, 200, 99),
        con(94, 201, 98), con(96, 202, 96), con(99, 203, 95),
        con(101, 203, 94), con(103, 204, 92), con(105, 205, 91),
        con(108, 205, 90), con(110, 206, 88), con(112, 207, 87),
        con(115, 208, 86), con(117, 208, 84), con(119, 209, 83),
        con(122, 209, 81), con(124, 210, 80), con(127, 211, 78),
        con(129, 211, 77), con(132, 212, 75), con(134, 213, 73),
        con(137, 213, 72), con(139, 214, 70), con(142, 214, 69),
        con(144, 215, 67), con(147, 215, 65), con(149, 216, 64),
        con(152, 216, 62), con(155, 217, 60), con(157, 217, 59),
        con(160, 218, 57), con(162, 218, 55), con(165, 219, 54),
        con(168, 219, 52), con(170, 220, 50), con(173, 220, 48),
        con(176, 221, 47), con(178, 221, 45), con(181, 222, 43),
        con(184, 222, 41), con(186, 222, 40), con(189, 223, 38),
        con(192, 223, 37), con(194, 223, 35), con(197, 224, 33),
        con(200, 224, 32), con(202, 225, 31), con(205, 225, 29),
        con(208, 225, 28), con(210, 226, 27), con(213, 226, 26),
        con(216, 226, 25), con(218, 227, 25), con(221, 227, 24),
        con(223, 227, 24), con(226, 228, 24), con(229, 228, 25),
        con(231, 228, 25), con(234, 229, 26), con(236, 229, 27),
        con(239, 229, 28), con(241, 229, 29), con(244, 230, 30),
        con(246, 230, 32), con(248, 230, 33), con(251, 231, 35),
        con(253, 231, 37)
    );
}
