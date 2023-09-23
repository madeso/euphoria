#pragma once

#include "io/json.h"


namespace eu::files::gui
{
    enum class InterpolationType {
        linear,

        // also called ease in
        // start slow, end quick
        smooth_start2, smooth_start3, smooth_start4,

        // also called ease out
        // start fast, end slow
        smooth_stop2, smooth_stop3, smooth_stop4,

        // also called ease in and out
        // start slow, end slow
        smooth_step2, smooth_step3, smooth_step4
    };


    struct Button;
    struct Lrtb;
    struct Widget;
    struct TableLayout;
    struct SingleRowLayout;
    struct Layout;
    struct LayoutContainer;
    struct Panel;
    struct Rgb;
    struct ButtonState;
    struct Skin;
    struct File;


    struct Button
    {
        std::string skin = "default";
        std::string text;
        std::string command;
    };

    struct Lrtb
    {
        float left;
        float right;
        float top;
        float bottom;
    };

    struct Widget
    {
        // common data
        std::string name;

        jsonh::tint column;
        jsonh::tint row;
        float preferred_width;
        float preferred_height;
        Lrtb margin;
        Lrtb padding;


        // implementations, only one is valid
        std::optional<Button> button;
        std::unique_ptr<Panel> panel;
    };

    struct TableLayout
    {
        std::vector<bool> expanded_cols;
        std::vector<bool> expanded_rows;
        float padding;
    };

    struct SingleRowLayout
    {
        float padding;
    };

    struct Layout
    {
        std::optional<TableLayout> table;
        std::optional<SingleRowLayout> single_row;
    };

    struct LayoutContainer
    {
        Layout layout;
        std::vector<Widget> widgets;
    };

    struct Panel
    {
        LayoutContainer container;
    };

    struct Rgb
    {
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
    };

    struct ButtonState
    {
        // string image;
        float scale = 1.0f;
        Rgb image_color;
        Rgb text_color;
        float dx;
        float dy;

        InterpolationType interpolate_position = InterpolationType::linear;
        float interpolate_position_time = 0.1f;

        InterpolationType interpolate_color = InterpolationType::linear;
        float interpolate_color_time = 0.1f;

        InterpolationType interpolate_size = InterpolationType::linear;
        float interpolate_size_time = 0.1f;
    };

    struct Skin
    {
        std::string name = "default";
        std::string font;
        float text_size = 12.0f;
        std::string button_image;

        ButtonState button_idle;
        ButtonState button_hot;
        ButtonState button_active_hot;
    };

    struct File
    {
        // common data
        std::string cursor_image;
        std::string hover_image;

        std::vector<Skin> skins;
        LayoutContainer root;
    };

    JSON_PARSE_FUNC(File);
}
