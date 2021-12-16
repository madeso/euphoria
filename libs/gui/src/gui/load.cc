#include "gui/load.h"

#include "core/proto.h"
#include "core/log.h"

#include "render/texturecache.h"
#include "render/scalablesprite.h"
#include "render/fontcache.h"

#include "gui/layoutcontainer.h"
#include "gui/layout.h"
#include "gui/button.h"
#include "gui/panelwidget.h"
#include "gui/skin.h"
#include "gui/root.h"

#include "gaf_gui.h"
#include "gaf_pugixml_gui.h"

#include <iostream>
#include <map>

namespace euphoria::gui
{
    std::shared_ptr<Layout>
    create_layout(const ::gui::Layout& c)
    {
        if(c.table)
        {
            LOG_INFO("Creating a table layout");
            return create_table_layout
            (
                c.table->expanded_rows,
                c.table->expanded_cols,
                c.table->padding
            );
        }
        else if(c.single_row)
        {
            LOG_INFO("Creating a single row layout");
            return create_single_row_layout(c.single_row->padding);
        }
        else
        {
            LOG_ERROR("Missing a layout");
            return create_single_row_layout(0);
        }
    }


    struct CommandButton : public Button
    {
        explicit CommandButton(gui::State* state)
            : Button(state)
        {
        }

        void
        on_clicked() override
        {
            // todo(Gustav): set up command to actually do something
            LOG_INFO("Executing cmd: {0}", cmd);
        }

        std::string cmd;
    };


    void
    build_layout_container
    (
        core::vfs::FileSystem* fs,
        State* state,
        LayoutContainer* root,
        const ::gui::LayoutContainer& c,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    );


    void
    setup_layout(LayoutData* data, const ::gui::Widget& src)
    {
        data->column = src.column;
        data->row = src.row;
        data->preferred_width = src.preferred_width;
        data->preferred_height = src.preferred_height;
    }


    core::Lrud<float>
    lrtb_from_gaf(const ::gui::Lrtb& lrtd)
    {
        return core::Lrud<float>::from_lrud
        (
            lrtd.left,
            lrtd.right,
            lrtd.top,
            lrtd.bottom
        );
    }


    std::shared_ptr<Widget>
    create_widget
    (
        core::vfs::FileSystem* fs,
        State* state,
        const ::gui::Widget& w,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    )
    {
        std::shared_ptr<Widget> ret;

        if(w.button)
        {
            LOG_INFO("Creating a button widget");
            auto b = std::make_shared<CommandButton>(state);

            const std::string skin_name = w.button->skin;
            const auto skin_it = skins.find(skin_name);
            Skin* skin = nullptr;

            if(skin_it == skins.end())
            {
                LOG_ERROR("Failed to find skin '{0}'", skin_name);
            }
            else
            {
                b->set_skin(skin_it->second);

                skin = skin_it->second;

                if(skin_it->second->button_image.has_value())
                {
                    std::shared_ptr<render::ScalableSprite> sp
                    {
                        new render::ScalableSprite
                        {
                            fs,
                            skin->button_image.value(),
                            cache
                        }
                    };
                    b->sprite = sp;
                }
            }
            ret = b;
            b->cmd = w.button->command;
            b->text.update_string(w.button->text);

            if(skin != nullptr)
            {
                b->text.set_font(skin->font);
            }
        }
        else if(w.panel)
        {
            LOG_INFO("Creating a panel widget");
            auto l = std::make_shared<PanelWidget>(state);
            ret = l;
            build_layout_container
            (
                fs,
                state,
                &l->container,
                w.panel->container,
                cache,
                skins
            );
        }
        else
        {
            LOG_ERROR("Invalid widget");
        }

        ASSERT(ret);

        // load basic widget data
        ret->name = w.name;
        ret->padding = lrtb_from_gaf(w.padding);
        ret->margin = lrtb_from_gaf(w.margin);

        setup_layout(&ret->layout, w);

        return ret;
    }

    void
    build_layout_container
    (
        core::vfs::FileSystem* fs,
        State* state,
        LayoutContainer* root,
        const ::gui::LayoutContainer& c,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    )
    {
        root->layout = create_layout(c.layout);
        for(const auto& widget: c.widgets)
        {
            root->add(create_widget(fs, state, widget, cache, skins));
        }
    }

    core::Rgb
    load(const ::gui::Rgb& src)
    {
        return core::Rgb(src.r, src.g, src.b);
    }

    core::InterpolationType
    load(::gui::InterpolationType t)
    {
        switch(t)
        {
#define FUN(NAME, FUNC) \
    case ::gui::InterpolationType::NAME: return core::InterpolationType::NAME;
            // Linear interpolation (no easing)
            FUN(Linear, LinearInterpolation)

            // Quadratic easing; p^2
            FUN(QuadIn, QuadraticEaseIn)
            FUN(QuadOut, QuadraticEaseOut)
            FUN(Quad, QuadraticEaseInOut)

            // Cubic easing; p^3
            FUN(CubicIn, CubicEaseIn)
            FUN(CubicOut, CubicEaseOut)
            FUN(Cubic, CubicEaseInOut)

            // Quartic easing; p^4
            FUN(QuartIn, QuarticEaseIn)
            FUN(QuartOut, QuarticEaseOut)
            FUN(Quart, QuarticEaseInOut)

            // Quintic easing; p^5
            FUN(QuintIn, QuinticEaseIn)
            FUN(QuintOut, QuinticEaseOut)
            FUN(Quint, QuinticEaseInOut)

            // Sine wave easing; sin(p * PI/2)
            FUN(SineIn, SineEaseIn)
            FUN(SineOut, SineEaseOut)
            FUN(Sine, SineEaseInOut)

            // Circular easing; sqrt(1 - p^2)
            FUN(CircIn, CircularEaseIn)
            FUN(CircOut, CircularEaseOut)
            FUN(Circ, CircularEaseInOut)

            // Exponential easing, base 2
            FUN(ExpIn, ExponentialEaseIn)
            FUN(ExpOut, ExponentialEaseOut)
            FUN(Exp, ExponentialEaseInOut)

            // Exponentially-damped sine wave easing
            FUN(ElasticIn, ElasticEaseIn)
            FUN(ElasticOut, ElasticEaseOut)
            FUN(Elastic, ElasticEaseInOut)

            // Overshooting cubic easing;
            FUN(BackIn, BackEaseIn)
            FUN(BackOut, BackEaseOut)
            FUN(Back, BackEaseInOut)

            // Exponentially-decaying bounce easing
            FUN(BounceIn, BounceEaseIn)
            FUN(BounceOut, BounceEaseOut)
            FUN(Bounce, BounceEaseInOut)
#undef FUN
        default: return core::InterpolationType::INVALID;
        }
    }


    ButtonState
    load_button(const ::gui::ButtonState& src)
    {
        ButtonState ret;
        // ret.image = src.image();
        ret.scale = src.scale;
        ret.image_color = load(src.image_color);
        ret.text_color = load(src.text_color);
        ret.dx = src.dx;
        ret.dy = src.dy;
        ret.interpolation_color =
        {
                load(src.interpolate_color),
                src.interpolate_color_time
        };
        ret.interpolation_size =
        {
                load(src.interpolate_size),
                src.interpolate_size_time
        };
        ret.interpolation_position =
        {
                load(src.interpolate_position),
                src.interpolate_position_time
        };
        return ret;
    }


    std::shared_ptr<Skin>
    load_skin(const ::gui::Skin& src, render::FontCache* font)
    {
        std::shared_ptr<Skin> skin(new gui::Skin());
        skin->name = src.name;
        skin->font = font->get_font
        (
            core::vfs::FilePath::from_script(src.font).value_or
            (
                core::vfs::FilePath{"~/invalid_font_file"}
            )
        );
        skin->button_image = core::vfs::FilePath::from_script_or_empty
        (
            src.button_image
        );
        skin->text_size = src.text_size;
        skin->button_idle = load_button(src.button_idle);
        skin->button_hot = load_button(src.button_hot);
        skin->button_active_hot = load_button(src.button_active_hot);
        return skin;
    }


    bool
    load_gui
    (
        Root* root,
        core::vfs::FileSystem* fs,
        render::FontCache* font,
        const core::vfs::FilePath& path,
        render::TextureCache* cache
    )
    {
        const auto loaded = core::get_optional_and_log_errors
        (
            core::read_xml_file_to_gaf_struct<::gui::File>(fs, path, ::gui::ReadXmlElementFile)
        );
        
        if(loaded.has_value() == false)
        {
            return false;
        }

        const auto& f = *loaded;

        root->cursor_image = cache->get_texture
        (
            core::vfs::FilePath::from_script_or_empty(f.cursor_image)
        );
        root->hover_image = cache->get_texture
        (
            core::vfs::FilePath::from_script_or_empty(f.hover_image)
        );

        std::map<std::string, Skin*> skin_map;

        for(const auto& skin: f.skins)
        {
            std::shared_ptr<gui::Skin> skin_ptr = load_skin(skin, font);
            skin_map.insert(std::make_pair(skin.name, skin_ptr.get()));
            root->skins.push_back(skin_ptr);
        }

        build_layout_container
        (
            fs,
            &root->state,
            &root->container,
            f.root,
            cache,
            skin_map
        );

        return root->container.has_any_widgets();
    }
}
