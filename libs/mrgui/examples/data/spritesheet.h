#pragma once

namespace spritesheet
{
    constexpr eu::Rect img(float x, float y, float w, float h)
    {
        constexpr float iw = 478;
        constexpr float ih = 382;
        return eu::Rect::from_left_right_bottom_top(x/iw, (x + w)/iw, (ih - y - h)/ih, (ih-y)/ih);
    }

constexpr eu::Rect arrow_basic_e = img(1, 1, 32, 32);
constexpr eu::Rect arrow_basic_e_small = img(35, 1, 24, 24);
constexpr eu::Rect arrow_basic_n = img(61, 1, 32, 32);
constexpr eu::Rect arrow_basic_n_small = img(95, 1, 24, 24);
constexpr eu::Rect arrow_basic_s = img(121, 1, 32, 32);
constexpr eu::Rect arrow_basic_s_small = img(155, 1, 24, 24);
constexpr eu::Rect arrow_basic_w = img(181, 1, 32, 32);
constexpr eu::Rect arrow_basic_w_small = img(215, 1, 24, 24);
constexpr eu::Rect arrow_decorative_e = img(241, 1, 32, 32);
constexpr eu::Rect arrow_decorative_e_small = img(275, 1, 24, 24);
constexpr eu::Rect arrow_decorative_n = img(301, 1, 32, 32);
constexpr eu::Rect arrow_decorative_n_small = img(335, 1, 24, 24);
constexpr eu::Rect arrow_decorative_s = img(335, 27, 32, 32);
constexpr eu::Rect arrow_decorative_s_small = img(35, 27, 24, 24);
constexpr eu::Rect arrow_decorative_w = img(1, 61, 32, 32);
constexpr eu::Rect arrow_decorative_w_small = img(95, 27, 24, 24);
constexpr eu::Rect button_round_border = img(35, 61, 64, 64);
constexpr eu::Rect button_round_depth_border = img(101, 61, 64, 64);
constexpr eu::Rect button_round_depth_flat = img(167, 35, 64, 64);
constexpr eu::Rect button_round_depth_gloss = img(233, 61, 64, 64);
constexpr eu::Rect button_round_depth_gradient = img(299, 61, 64, 64);
constexpr eu::Rect button_round_depth_line = img(1, 127, 64, 64);
constexpr eu::Rect button_round_flat = img(67, 127, 64, 64);
constexpr eu::Rect button_round_gloss = img(133, 127, 64, 64);
constexpr eu::Rect button_round_gradient = img(199, 127, 64, 64);
constexpr eu::Rect button_round_line = img(265, 127, 64, 64);
constexpr eu::Rect button_square_border = img(1, 193, 64, 64);
constexpr eu::Rect button_square_depth_border = img(67, 193, 64, 64);
constexpr eu::Rect button_square_depth_flat = img(133, 193, 64, 64);
constexpr eu::Rect button_square_depth_gloss = img(199, 193, 64, 64);
constexpr eu::Rect button_square_depth_gradient = img(265, 193, 64, 64);
constexpr eu::Rect button_square_depth_line = img(1, 259, 64, 64);
constexpr eu::Rect button_square_flat = img(67, 259, 64, 64);
constexpr eu::Rect button_square_gloss = img(133, 259, 64, 64);
constexpr eu::Rect button_square_gradient = img(199, 259, 64, 64);
constexpr eu::Rect button_square_line = img(265, 259, 64, 64);
constexpr eu::Rect check_round_color = img(331, 127, 32, 32);
constexpr eu::Rect check_round_grey = img(331, 161, 32, 32);
constexpr eu::Rect check_round_grey_circle = img(331, 195, 32, 32);
constexpr eu::Rect check_round_round_circle = img(331, 229, 32, 32);
constexpr eu::Rect check_square_color = img(331, 263, 32, 32);
constexpr eu::Rect check_square_color_checkmark = img(331, 297, 32, 32);
constexpr eu::Rect check_square_color_cross = img(1, 331, 32, 32);
constexpr eu::Rect check_square_color_square = img(35, 331, 32, 32);
constexpr eu::Rect check_square_grey = img(69, 331, 32, 32);
constexpr eu::Rect check_square_grey_checkmark = img(103, 331, 32, 32);
constexpr eu::Rect check_square_grey_cross = img(137, 331, 32, 32);
constexpr eu::Rect check_square_grey_square = img(171, 331, 32, 32);
constexpr eu::Rect icon_checkmark = img(275, 27, 20, 18);
constexpr eu::Rect icon_circle = img(361, 1, 16, 16);
constexpr eu::Rect icon_cross = img(205, 331, 18, 18);
constexpr eu::Rect icon_outline_checkmark = img(225, 331, 20, 18);
constexpr eu::Rect icon_outline_circle = img(247, 331, 16, 16);
constexpr eu::Rect icon_outline_cross = img(265, 331, 18, 18);
constexpr eu::Rect icon_outline_square = img(285, 331, 16, 16);
constexpr eu::Rect icon_square = img(303, 331, 16, 16);
constexpr eu::Rect slide_hangle = img(321, 331, 24, 32);
constexpr eu::Rect slide_horizontal_color = img(205, 351, 96, 16);
constexpr eu::Rect slide_horizontal_color_section = img(347, 331, 16, 16);
constexpr eu::Rect slide_horizontal_color_section_wide = img(379, 1, 32, 16);
constexpr eu::Rect slide_horizontal_grey = img(1, 365, 96, 16);
constexpr eu::Rect slide_horizontal_grey_section = img(369, 19, 16, 16);
constexpr eu::Rect slide_horizontal_grey_section_wide = img(1, 37, 32, 16);
constexpr eu::Rect slide_vertical_color = img(387, 19, 16, 96);
constexpr eu::Rect slide_vertical_color_section = img(365, 117, 16, 16);
constexpr eu::Rect slide_vertical_color_section_wide = img(383, 117, 16, 32);
constexpr eu::Rect slide_vertical_grey = img(365, 151, 16, 96);
constexpr eu::Rect slide_vertical_grey_section = img(383, 151, 16, 16);
constexpr eu::Rect slide_vertical_grey_section_wide = img(383, 169, 16, 32);
constexpr eu::Rect star = img(413, 1, 64, 60);
constexpr eu::Rect star_outline = img(405, 63, 64, 60);
constexpr eu::Rect star_outline_depth = img(401, 125, 64, 60);
}
