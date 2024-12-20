#include "files/gui.h"

#include "io/json.h"


namespace eu::files::gui
{
    JSON_PARSE_FUNC(Button);
    JSON_PARSE_FUNC(Lrtb);
    JSON_PARSE_FUNC(Widget);
    JSON_PARSE_FUNC(TableLayout);
    JSON_PARSE_FUNC(SingleRowLayout);
    JSON_PARSE_FUNC(Layout);
    JSON_PARSE_FUNC(LayoutContainer);
    JSON_PARSE_FUNC(Panel);
    JSON_PARSE_FUNC(Rgb);
    JSON_PARSE_FUNC(ButtonState);
    JSON_PARSE_FUNC(Skin);
    JSON_PARSE_FUNC(File);


    JSON_PARSE_FUNC(InterpolationType)
    {
        JSON_BEGIN_ENUM(InterpolationType);

        JSON_ENUM_VAL(linear);
        JSON_ENUM_VAL(smooth_start2); JSON_ENUM_VAL(smooth_start3); JSON_ENUM_VAL(smooth_start4);
        JSON_ENUM_VAL(smooth_stop2); JSON_ENUM_VAL(smooth_stop3); JSON_ENUM_VAL(smooth_stop4);
        JSON_ENUM_VAL(smooth_step2); JSON_ENUM_VAL(smooth_step3); JSON_ENUM_VAL(smooth_step4);

        JSON_END_ENUM();
    }

    
    JSON_PARSE_FUNC(Button)
    {
        JSON_BEGIN_OBJECT();

        JSON_OPT_VAL(String, skin);
        JSON_VAL(String, text);
        JSON_VAL(String, command);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(Lrtb)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(Number, left);
        JSON_VAL(Number, right);
        JSON_VAL(Number, top);
        JSON_VAL(Number, bottom);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(TableLayout)
    {
        JSON_BEGIN_OBJECT();

        JSON_ARRAY_PROP(Bool, expanded_cols);
        JSON_ARRAY_PROP(Bool, expanded_rows);
        JSON_VAL(Number, padding);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(SingleRowLayout)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(Number, padding);

        JSON_END_OBJECT();
    }


    JSON_PARSE_FUNC(Layout)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL_OPT_OBJ(TableLayout, table);
        JSON_VAL_OPT_OBJ(SingleRowLayout, single_row);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(LayoutContainer)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL_OBJ(Layout, layout);
        JSON_ARRAY(Widget, widgets);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(Widget)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, name);
        JSON_VAL(Int, column);
        JSON_VAL(Int, row);
        JSON_VAL(Number, preferred_width);
        JSON_VAL(Number, preferred_height);
        JSON_VAL_OBJ(Lrtb, margin);
        JSON_VAL_OBJ(Lrtb, padding);

        JSON_VAL_OPT_OBJ(Button, button);
        JSON_VAL_OPT_OBJ_CON(Panel, panel, std::make_unique<Panel>);

        JSON_END_OBJECT();
    }
    
    
    JSON_PARSE_FUNC(Panel)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL_OBJ(LayoutContainer, container);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(Rgb)
    {
        JSON_BEGIN_OBJECT();

        JSON_OPT_VAL(Number, r);
        JSON_OPT_VAL(Number, g);
        JSON_OPT_VAL(Number, b);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(ButtonState)
    {
        JSON_BEGIN_OBJECT();
        
        JSON_OPT_VAL(Number, scale);
        JSON_VAL_OBJ(Rgb, image_color);
        JSON_VAL_OBJ(Rgb, text_color);
        JSON_VAL(Number, dx);
        JSON_VAL(Number, dy);

        JSON_VAL_OPT_OBJ(InterpolationType, interpolate_position);
        JSON_OPT_VAL(Number, interpolate_position_time);

        JSON_VAL_OPT_OBJ(InterpolationType, interpolate_color);
        JSON_OPT_VAL(Number, interpolate_color_time);

        JSON_VAL_OPT_OBJ(InterpolationType, interpolate_size);
        JSON_OPT_VAL(Number, interpolate_size_time);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(Skin)
    {
        JSON_BEGIN_OBJECT();

        JSON_OPT_VAL(String, name);
        JSON_VAL(String, font);
        JSON_OPT_VAL(Number, text_size);
        JSON_VAL(String, button_image);

        JSON_VAL_OBJ(ButtonState, button_idle);
        JSON_VAL_OBJ(ButtonState, button_hot);
        JSON_VAL_OBJ(ButtonState, button_active_hot);

        JSON_END_OBJECT();
    }

    
    JSON_PARSE_FUNC(File)
    {
        JSON_BEGIN_OBJECT();

        JSON_VAL(String, cursor_image);
        JSON_VAL(String, hover_image);
        JSON_ARRAY(Skin, skins);
        JSON_VAL_OBJ(LayoutContainer, root);

        JSON_END_OBJECT();
    }
}

