#include "core/palette_mycarta.h"

#include "core/palette.h"

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
        Cube1()
        {
            static const auto p = Palette {
                    "Cube1",          C(120, 0, 133),   C(121, 0, 136),
                    C(122, 0, 139),   C(123, 1, 142),   C(124, 1, 145),
                    C(125, 2, 148),   C(126, 2, 151),   C(127, 3, 153),
                    C(128, 4, 156),   C(128, 5, 159),   C(129, 5, 162),
                    C(129, 6, 165),   C(130, 8, 167),   C(130, 9, 170),
                    C(131, 10, 173),  C(131, 11, 176),  C(131, 12, 178),
                    C(131, 13, 181),  C(131, 15, 184),  C(131, 17, 186),
                    C(131, 19, 189),  C(131, 22, 192),  C(130, 25, 194),
                    C(130, 28, 197),  C(130, 31, 200),  C(130, 34, 202),
                    C(129, 38, 205),  C(129, 42, 208),  C(129, 45, 210),
                    C(128, 49, 213),  C(128, 52, 215),  C(127, 55, 217),
                    C(127, 59, 220),  C(126, 61, 222),  C(126, 64, 224),
                    C(126, 66, 226),  C(125, 69, 228),  C(124, 71, 230),
                    C(123, 73, 233),  C(122, 75, 235),  C(121, 77, 237),
                    C(120, 79, 240),  C(119, 81, 242),  C(118, 83, 244),
                    C(117, 85, 246),  C(116, 87, 247),  C(115, 89, 249),
                    C(114, 91, 250),  C(113, 93, 252),  C(112, 94, 252),
                    C(111, 96, 253),  C(110, 98, 253),  C(109, 100, 253),
                    C(108, 102, 253), C(107, 104, 253), C(106, 107, 252),
                    C(105, 109, 252), C(104, 111, 251), C(103, 113, 251),
                    C(102, 115, 250), C(102, 117, 249), C(101, 119, 248),
                    C(100, 121, 247), C(99, 123, 247),  C(98, 125, 246),
                    C(97, 126, 245),  C(96, 128, 244),  C(95, 130, 243),
                    C(94, 132, 242),  C(93, 134, 241),  C(92, 135, 240),
                    C(91, 137, 239),  C(90, 138, 238),  C(89, 140, 236),
                    C(88, 142, 235),  C(87, 143, 234),  C(86, 145, 232),
                    C(85, 146, 230),  C(83, 148, 229),  C(82, 149, 227),
                    C(81, 151, 226),  C(80, 152, 224),  C(79, 153, 222),
                    C(78, 155, 221),  C(77, 156, 219),  C(76, 158, 217),
                    C(74, 159, 215),  C(73, 161, 214),  C(72, 162, 212),
                    C(70, 164, 210),  C(69, 165, 208),  C(67, 167, 206),
                    C(66, 168, 204),  C(64, 170, 202),  C(63, 171, 201),
                    C(61, 173, 199),  C(60, 174, 197),  C(59, 175, 194),
                    C(58, 177, 192),  C(57, 178, 190),  C(57, 179, 188),
                    C(56, 181, 186),  C(56, 182, 184),  C(56, 183, 182),
                    C(56, 184, 180),  C(57, 185, 178),  C(57, 187, 176),
                    C(58, 188, 174),  C(58, 189, 171),  C(59, 190, 169),
                    C(60, 191, 167),  C(61, 192, 165),  C(62, 193, 163),
                    C(62, 194, 160),  C(63, 195, 158),  C(64, 196, 156),
                    C(65, 197, 154),  C(66, 198, 151),  C(67, 199, 149),
                    C(67, 200, 147),  C(68, 201, 145),  C(69, 202, 142),
                    C(69, 203, 140),  C(70, 203, 138),  C(71, 204, 135),
                    C(71, 205, 133),  C(72, 206, 131),  C(73, 207, 129),
                    C(73, 208, 126),  C(74, 208, 124),  C(75, 209, 122),
                    C(75, 210, 119),  C(76, 211, 117),  C(77, 211, 115),
                    C(77, 212, 113),  C(78, 213, 111),  C(79, 214, 108),
                    C(80, 215, 106),  C(80, 216, 104),  C(81, 216, 101),
                    C(82, 217, 98),   C(82, 218, 96),   C(83, 219, 93),
                    C(84, 220, 90),   C(84, 221, 87),   C(85, 222, 85),
                    C(86, 222, 82),   C(87, 223, 80),   C(87, 224, 78),
                    C(88, 225, 76),   C(89, 225, 75),   C(90, 226, 74),
                    C(91, 227, 73),   C(92, 227, 73),   C(94, 228, 73),
                    C(95, 229, 73),   C(97, 229, 73),   C(99, 230, 74),
                    C(101, 230, 74),  C(104, 231, 74),  C(106, 231, 75),
                    C(109, 232, 75),  C(111, 232, 76),  C(114, 233, 76),
                    C(117, 233, 77),  C(120, 234, 78),  C(122, 234, 78),
                    C(125, 234, 79),  C(128, 235, 79),  C(130, 235, 80),
                    C(133, 235, 80),  C(135, 235, 80),  C(137, 235, 81),
                    C(140, 235, 81),  C(142, 235, 82),  C(145, 235, 82),
                    C(147, 235, 82),  C(150, 236, 83),  C(152, 236, 83),
                    C(155, 236, 84),  C(157, 236, 84),  C(160, 236, 84),
                    C(162, 236, 85),  C(165, 236, 85),  C(167, 236, 85),
                    C(169, 236, 86),  C(171, 236, 86),  C(173, 236, 86),
                    C(175, 236, 87),  C(177, 236, 87),  C(180, 236, 87),
                    C(182, 236, 87),  C(184, 236, 88),  C(185, 236, 88),
                    C(187, 236, 88),  C(189, 236, 88),  C(191, 236, 89),
                    C(193, 236, 89),  C(195, 236, 89),  C(196, 236, 89),
                    C(198, 236, 89),  C(200, 236, 89),  C(201, 236, 90),
                    C(203, 236, 90),  C(204, 236, 90),  C(205, 236, 90),
                    C(207, 236, 90),  C(208, 235, 90),  C(209, 234, 91),
                    C(210, 234, 91),  C(211, 233, 91),  C(212, 232, 91),
                    C(213, 230, 91),  C(214, 229, 91),  C(215, 228, 91),
                    C(216, 226, 91),  C(217, 225, 91),  C(218, 224, 92),
                    C(219, 222, 92),  C(220, 221, 92),  C(221, 219, 92),
                    C(222, 218, 92),  C(223, 217, 92),  C(224, 215, 92),
                    C(226, 214, 92),  C(227, 213, 93),  C(229, 211, 93),
                    C(230, 210, 93),  C(231, 208, 93),  C(233, 206, 93),
                    C(234, 205, 93),  C(236, 203, 93),  C(237, 201, 94),
                    C(238, 200, 94),  C(239, 198, 94),  C(240, 196, 94),
                    C(241, 194, 94),  C(242, 192, 94),  C(243, 190, 94),
                    C(243, 188, 94),  C(244, 186, 94),  C(244, 184, 94),
                    C(245, 182, 94),  C(245, 180, 94),  C(246, 178, 94),
                    C(246, 176, 93),  C(247, 173, 93),  C(247, 171, 93),
                    C(248, 168, 93),  C(248, 166, 93),  C(248, 163, 92),
                    C(249, 161, 92),  C(249, 158, 92),  C(249, 156, 92),
                    C(249, 153, 91),  C(249, 150, 91)};
            return p;
        }

        const Palette&
        CubeYF()
        {
            static const auto p = Palette {
                    "CybeYF",         C(123, 2, 144),   C(123, 3, 146),
                    C(124, 4, 149),   C(125, 5, 151),   C(126, 6, 153),
                    C(127, 7, 156),   C(128, 8, 158),   C(129, 9, 160),
                    C(129, 10, 163),  C(130, 11, 165),  C(131, 12, 168),
                    C(131, 12, 171),  C(132, 12, 174),  C(133, 13, 177),
                    C(133, 15, 179),  C(134, 17, 181),  C(134, 19, 184),
                    C(134, 21, 186),  C(134, 24, 188),  C(134, 26, 190),
                    C(134, 29, 192),  C(134, 31, 194),  C(134, 34, 196),
                    C(134, 36, 198),  C(133, 39, 200),  C(133, 41, 203),
                    C(133, 43, 204),  C(133, 45, 206),  C(132, 47, 208),
                    C(132, 49, 210),  C(132, 51, 212),  C(131, 53, 214),
                    C(131, 55, 216),  C(130, 56, 218),  C(130, 58, 219),
                    C(129, 60, 221),  C(129, 62, 223),  C(128, 64, 224),
                    C(128, 66, 226),  C(127, 68, 228),  C(127, 69, 230),
                    C(126, 71, 232),  C(125, 72, 234),  C(125, 74, 235),
                    C(122, 77, 238),  C(121, 78, 240),  C(120, 80, 241),
                    C(119, 81, 242),  C(118, 83, 244),  C(117, 84, 245),
                    C(117, 86, 247),  C(115, 87, 248),  C(115, 89, 250),
                    C(114, 90, 251),  C(113, 91, 251),  C(113, 92, 252),
                    C(112, 94, 252),  C(111, 95, 252),  C(110, 97, 253),
                    C(110, 98, 253),  C(109, 100, 253), C(108, 101, 254),
                    C(108, 103, 254), C(106, 104, 254), C(106, 106, 254),
                    C(105, 107, 255), C(105, 108, 255), C(104, 110, 255),
                    C(104, 112, 254), C(104, 113, 254), C(103, 115, 253),
                    C(103, 116, 253), C(102, 118, 252), C(101, 119, 251),
                    C(100, 120, 250), C(99, 122, 249),  C(98, 124, 248),
                    C(97, 126, 246),  C(97, 128, 245),  C(96, 130, 244),
                    C(95, 131, 243),  C(94, 132, 242),  C(93, 134, 241),
                    C(92, 135, 240),  C(91, 136, 239),  C(90, 137, 238),
                    C(90, 138, 237),  C(89, 139, 236),  C(88, 141, 234),
                    C(87, 142, 233),  C(86, 143, 232),  C(85, 144, 231),
                    C(84, 145, 230),  C(84, 147, 228),  C(82, 148, 227),
                    C(82, 150, 225),  C(81, 151, 224),  C(80, 152, 223),
                    C(79, 153, 222),  C(78, 154, 221),  C(77, 156, 219),
                    C(76, 157, 218),  C(76, 158, 217),  C(75, 159, 216),
                    C(74, 160, 215),  C(73, 161, 214),  C(72, 163, 212),
                    C(71, 164, 211),  C(70, 165, 210),  C(69, 167, 208),
                    C(67, 169, 205),  C(66, 170, 203),  C(65, 171, 202),
                    C(64, 172, 200),  C(64, 173, 199),  C(63, 174, 197),
                    C(62, 175, 196),  C(61, 176, 195),  C(60, 177, 193),
                    C(60, 178, 191),  C(59, 179, 190),  C(58, 180, 189),
                    C(57, 181, 187),  C(56, 182, 185),  C(56, 182, 184),
                    C(55, 184, 182),  C(55, 184, 181),  C(56, 185, 179),
                    C(57, 186, 177),  C(57, 187, 176),  C(58, 188, 175),
                    C(58, 188, 173),  C(59, 189, 171),  C(59, 190, 170),
                    C(60, 191, 168),  C(61, 192, 166),  C(61, 192, 164),
                    C(62, 193, 163),  C(62, 193, 161),  C(63, 194, 159),
                    C(64, 195, 158),  C(64, 196, 156),  C(65, 197, 153),
                    C(66, 198, 150),  C(67, 199, 148),  C(67, 200, 147),
                    C(68, 201, 145),  C(69, 202, 143),  C(69, 202, 141),
                    C(70, 203, 140),  C(70, 203, 138),  C(70, 204, 136),
                    C(71, 205, 134),  C(71, 205, 133),  C(72, 206, 131),
                    C(72, 207, 129),  C(73, 208, 128),  C(73, 208, 126),
                    C(74, 209, 124),  C(75, 210, 122),  C(75, 210, 120),
                    C(76, 211, 118),  C(76, 211, 116),  C(77, 212, 114),
                    C(77, 212, 113),  C(78, 213, 111),  C(79, 214, 109),
                    C(79, 214, 107),  C(80, 215, 106),  C(80, 215, 104),
                    C(81, 216, 102),  C(82, 217, 101),  C(82, 217, 100),
                    C(83, 218, 98),   C(84, 219, 95),   C(85, 220, 92),
                    C(86, 220, 90),   C(87, 221, 88),   C(88, 222, 87),
                    C(88, 222, 85),   C(89, 223, 84),   C(89, 223, 82),
                    C(89, 224, 81),   C(90, 225, 80),   C(90, 225, 79),
                    C(91, 226, 78),   C(91, 226, 76),   C(92, 227, 74),
                    C(92, 227, 73),   C(93, 228, 71),   C(95, 229, 71),
                    C(96, 229, 71),   C(98, 229, 72),   C(99, 229, 72),
                    C(101, 230, 72),  C(103, 230, 73),  C(104, 231, 73),
                    C(106, 231, 74),  C(108, 231, 74),  C(110, 232, 75),
                    C(111, 232, 75),  C(113, 232, 76),  C(115, 233, 77),
                    C(116, 233, 77),  C(118, 233, 77),  C(120, 233, 77),
                    C(123, 233, 78),  C(126, 233, 79),  C(130, 234, 80),
                    C(132, 235, 80),  C(134, 235, 80),  C(136, 235, 81),
                    C(138, 235, 81),  C(140, 235, 82),  C(142, 235, 82),
                    C(145, 235, 82),  C(147, 236, 83),  C(149, 236, 83),
                    C(152, 236, 84),  C(154, 236, 84),  C(156, 236, 84),
                    C(159, 236, 85),  C(161, 236, 85),  C(163, 236, 85),
                    C(164, 236, 85),  C(165, 236, 85),  C(166, 236, 86),
                    C(168, 236, 86),  C(171, 236, 86),  C(172, 236, 86),
                    C(173, 236, 86),  C(174, 236, 86),  C(176, 236, 87),
                    C(178, 236, 87),  C(180, 236, 87),  C(180, 236, 87),
                    C(182, 236, 87),  C(183, 236, 88),  C(184, 236, 88),
                    C(184, 236, 88),  C(187, 236, 88),  C(189, 236, 88),
                    C(191, 236, 89),  C(192, 236, 89),  C(192, 236, 89),
                    C(194, 236, 89),  C(197, 236, 89),  C(199, 236, 90),
                    C(200, 236, 90),  C(201, 236, 90),  C(202, 236, 90),
                    C(204, 236, 90),  C(204, 236, 90),  C(205, 236, 90),
                    C(207, 236, 91),  C(209, 235, 91),
            };
            return p;
        }


        const Palette&
        LinearL()
        {
            static const auto p = Palette {
                    "LinearL",        C(4, 4, 4),       C(10, 3, 8),
                    C(13, 4, 11),     C(16, 5, 14),     C(18, 5, 16),
                    C(21, 6, 18),     C(22, 7, 19),     C(24, 8, 21),
                    C(26, 8, 22),     C(27, 9, 24),     C(28, 10, 25),
                    C(30, 11, 26),    C(31, 12, 27),    C(32, 12, 28),
                    C(33, 13, 29),    C(35, 14, 31),    C(36, 14, 32),
                    C(37, 15, 32),    C(38, 15, 33),    C(39, 16, 34),
                    C(40, 17, 35),    C(41, 17, 36),    C(42, 18, 38),
                    C(43, 19, 38),    C(44, 19, 39),    C(46, 20, 41),
                    C(46, 20, 45),    C(46, 21, 50),    C(45, 21, 55),
                    C(45, 21, 60),    C(45, 22, 64),    C(45, 23, 67),
                    C(45, 23, 71),    C(45, 24, 75),    C(45, 24, 77),
                    C(45, 25, 81),    C(45, 25, 84),    C(44, 26, 87),
                    C(44, 27, 90),    C(45, 27, 92),    C(45, 28, 95),
                    C(44, 29, 98),    C(44, 29, 100),   C(44, 30, 103),
                    C(44, 31, 106),   C(44, 31, 109),   C(44, 32, 110),
                    C(44, 33, 113),   C(44, 34, 116),   C(43, 34, 118),
                    C(42, 35, 121),   C(40, 38, 120),   C(38, 40, 119),
                    C(36, 42, 120),   C(34, 44, 120),   C(33, 46, 120),
                    C(32, 47, 120),   C(31, 49, 121),   C(30, 50, 122),
                    C(30, 51, 123),   C(29, 52, 123),   C(29, 53, 125),
                    C(28, 55, 125),   C(28, 56, 126),   C(27, 57, 127),
                    C(28, 58, 128),   C(28, 59, 129),   C(27, 60, 129),
                    C(27, 61, 131),   C(27, 62, 132),   C(27, 63, 133),
                    C(28, 64, 134),   C(27, 65, 135),   C(27, 66, 136),
                    C(27, 68, 137),   C(27, 69, 138),   C(25, 71, 136),
                    C(22, 73, 134),   C(21, 74, 133),   C(20, 76, 131),
                    C(17, 78, 129),   C(16, 79, 128),   C(15, 81, 126),
                    C(14, 82, 125),   C(10, 84, 123),   C(10, 85, 122),
                    C(9, 87, 120),    C(8, 88, 119),    C(7, 89, 118),
                    C(6, 91, 117),    C(4, 92, 115),    C(4, 94, 114),
                    C(4, 95, 114),    C(3, 96, 112),    C(1, 98, 111),
                    C(1, 99, 110),    C(0, 100, 109),   C(0, 101, 108),
                    C(0, 103, 107),   C(0, 104, 106),   C(0, 105, 105),
                    C(0, 107, 104),   C(0, 108, 101),   C(0, 110, 100),
                    C(0, 111, 99),    C(0, 112, 98),    C(0, 114, 96),
                    C(0, 115, 95),    C(0, 116, 93),    C(0, 118, 92),
                    C(0, 119, 90),    C(0, 120, 89),    C(0, 121, 88),
                    C(0, 123, 86),    C(0, 124, 85),    C(0, 125, 83),
                    C(0, 127, 82),    C(0, 128, 80),    C(0, 129, 79),
                    C(0, 131, 77),    C(0, 132, 75),    C(0, 133, 73),
                    C(0, 134, 72),    C(0, 136, 70),    C(0, 137, 68),
                    C(0, 138, 66),    C(0, 139, 65),    C(0, 141, 64),
                    C(0, 142, 63),    C(0, 143, 61),    C(0, 145, 60),
                    C(0, 146, 60),    C(0, 147, 58),    C(0, 149, 57),
                    C(0, 150, 56),    C(0, 151, 55),    C(0, 153, 53),
                    C(0, 154, 52),    C(0, 155, 51),    C(0, 157, 50),
                    C(0, 158, 48),    C(0, 159, 47),    C(0, 160, 45),
                    C(0, 162, 44),    C(0, 163, 42),    C(0, 164, 41),
                    C(0, 165, 39),    C(0, 167, 36),    C(0, 168, 34),
                    C(0, 169, 31),    C(0, 170, 23),    C(0, 169, 8),
                    C(9, 170, 0),     C(20, 171, 0),    C(29, 172, 0),
                    C(35, 173, 0),    C(40, 174, 0),    C(45, 175, 0),
                    C(48, 176, 0),    C(52, 177, 0),    C(55, 178, 0),
                    C(59, 179, 0),    C(61, 180, 0),    C(64, 181, 0),
                    C(66, 182, 0),    C(68, 183, 0),    C(71, 184, 0),
                    C(73, 185, 0),    C(76, 186, 0),    C(78, 187, 0),
                    C(79, 188, 0),    C(81, 189, 0),    C(83, 190, 0),
                    C(85, 191, 0),    C(87, 192, 0),    C(92, 193, 0),
                    C(99, 193, 0),    C(106, 193, 0),   C(114, 193, 0),
                    C(119, 194, 0),   C(125, 194, 0),   C(130, 194, 0),
                    C(135, 195, 0),   C(140, 195, 0),   C(145, 195, 0),
                    C(149, 196, 0),   C(153, 196, 0),   C(157, 197, 0),
                    C(161, 197, 0),   C(165, 197, 0),   C(169, 198, 0),
                    C(172, 198, 0),   C(176, 199, 0),   C(180, 199, 0),
                    C(184, 199, 0),   C(186, 200, 0),   C(190, 201, 0),
                    C(193, 201, 0),   C(197, 201, 0),   C(200, 202, 0),
                    C(201, 201, 24),  C(203, 202, 51),  C(206, 202, 65),
                    C(207, 203, 77),  C(209, 203, 87),  C(212, 203, 95),
                    C(213, 204, 103), C(215, 205, 109), C(218, 205, 116),
                    C(219, 206, 121), C(221, 207, 127), C(223, 207, 132),
                    C(226, 207, 138), C(227, 208, 143), C(229, 209, 147),
                    C(231, 209, 151), C(232, 210, 155), C(235, 211, 159),
                    C(237, 211, 164), C(238, 212, 168), C(240, 212, 172),
                    C(243, 213, 175), C(243, 214, 179), C(245, 214, 183),
                    C(248, 215, 186), C(248, 216, 189), C(248, 218, 193),
                    C(247, 219, 195), C(247, 220, 198), C(247, 222, 201),
                    C(248, 223, 204), C(247, 224, 206), C(247, 226, 209),
                    C(247, 227, 211), C(247, 229, 214), C(247, 230, 216),
                    C(247, 231, 218), C(247, 232, 220), C(248, 234, 224),
                    C(247, 235, 225), C(247, 236, 229), C(247, 238, 231),
                    C(247, 239, 232), C(248, 240, 235), C(248, 242, 237),
                    C(247, 243, 239), C(248, 244, 241), C(248, 246, 244),
                    C(248, 247, 246), C(248, 248, 248), C(249, 249, 249),
                    C(251, 251, 251), C(252, 252, 252), C(253, 253, 253),
                    C(254, 254, 254), C(255, 255, 255),
            };
            return p;
        }
    }  // namespace palette

}  // namespace euphoria::core
