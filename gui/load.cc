#include "gui/load.h"

#include <iostream>
#include <map>

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

namespace euphoria::gui
{
    LOG_SPECIFY_DEFAULT_LOGGER("gui.load")


    std::shared_ptr<Layout>
    GetLayout(const ::gui::Layout& c)
    {
        if(c.table)
        {
            LOG_INFO("Creating a table layout");
            return CreateTableLayout
            (
                c.table->expanded_rows,
                c.table->expanded_cols,
                c.table->padding
            );
        }
        else if(c.single_row)
        {
            LOG_INFO("Creating a single row layout");
            return CreateSingleRowLayout(c.single_row->padding);
        }
        else
        {
            LOG_ERROR("Missing a layout");
            return CreateSingleRowLayout(0);
        }
    }


    struct CmdButton : public Button
    {
        explicit CmdButton(UiState* state)
            : Button(state)
        {
        }

        void
        OnClicked() override
        {
            LOG_INFO("Executing cmd: {0}", cmd);
        }

        std::string cmd;
    };


    void
    BuildLayoutContainer
    (
        core::vfs::FileSystem* fs,
        UiState* state,
        LayoutContainer* root,
        const ::gui::LayoutContainer& c,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    );
    

    void
    SetupLayout(LayoutData* data, const ::gui::Widget& src)
    {
        data->SetColumn(src.column);
        data->SetRow(src.row);
        data->SetPreferredWidth(src.preferred_width);
        data->SetPreferredHeight(src.preferred_height);
    }


    Lrtb
    LrtbFromProt(const ::gui::Lrtb& lrtd)
    {
        Lrtb r;
        r.left = lrtd.left;
        r.right = lrtd.right;
        r.top = lrtd.top;
        r.bottom = lrtd.bottom;
        return r;
    }


    std::shared_ptr<Widget>
    CreateWidget
    (
        core::vfs::FileSystem* fs,
        UiState* state,
        const ::gui::Widget& w,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    )
    {
        std::shared_ptr<Widget> ret;

        if(w.button)
        {
            LOG_INFO("Creating a button widget");
            auto* b = new CmdButton(state);

            const std::string skin_name = w.button->skin;
            const auto skin_it = skins.find(skin_name);
            Skin* skin = nullptr;

            if(skin_it == skins.end())
            {
                LOG_ERROR("Failed to find skin '{0}'", skin_name);
            }
            else
            {
                b->SetSkin(skin_it->second);
            
                skin = skin_it->second;

                if(!skin_it->second->button_image.has_value())
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
                    b->SetSprite(sp);
                }
            }
            ret.reset(b);
            b->cmd = w.button->command;
            b->Text().SetString(w.button->text);
            
            if(skin)
            {
                b->Text().SetFont(skin->font);
            }
        }
        else if(w.panel)
        {
            LOG_INFO("Creating a panel widget");
            PanelWidget* l = new PanelWidget(state);
            ret.reset(l);
            BuildLayoutContainer
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
        ret->padding = LrtbFromProt(w.padding);
        ret->margin = LrtbFromProt(w.margin);

        SetupLayout(&ret->layout, w);

        return ret;
    }

    void
    BuildLayoutContainer
    (
        core::vfs::FileSystem* fs,
        UiState* state,
        LayoutContainer* root,
        const ::gui::LayoutContainer& c,
        render::TextureCache* cache,
        const std::map<std::string, Skin*>& skins
    )
    {
        root->SetLayout(GetLayout(c.layout));
        for(const auto& widget: c.widgets)
        {
            root->Add(CreateWidget(fs, state, widget, cache, skins));
        }
    }

    core::Rgb
    Load(const ::gui::Rgb& src)
    {
        return core::Rgb(src.r, src.g, src.b);
    }

    core::InterpolationType
    Load(::gui::InterpolationType t)
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
    LoadButton(const ::gui::ButtonState& src)
    {
        ButtonState ret;
        // ret.image = src.image();
        ret.scale = src.scale;
        ret.image_color = Load(src.image_color);
        ret.text_color = Load(src.text_color);
        ret.dx = src.dx;
        ret.dy = src.dy;
        ret.interpolation_color =
        {
            Load(src.interpolate_color),
            src.interpolate_color_time
        };
        ret.interpolation_size =
        {
            Load(src.interpolate_size),
            src.interpolate_size_time
        };
        ret.interpolation_position =
        {
            Load(src.interpolate_position),
            src.interpolate_position_time
        };
        return ret;
    }


    std::shared_ptr<Skin>
    LoadSkin(const ::gui::Skin& src, render::FontCache* font)
    {
        std::shared_ptr<Skin> skin(new Skin());
        skin->name = src.name;
        skin->font = font->GetFont
        (
            core::vfs::FilePath::FromScript(src.font).value_or
            (
                core::vfs::FilePath{"~/invalid_font_file"}
            )
        );
        skin->button_image = core::vfs::FilePath::FromScriptOrEmpty
        (
            src.button_image
        );
        skin->text_size = src.text_size;
        skin->button_idle = LoadButton(src.button_idle);
        skin->button_hot = LoadButton(src.button_hot);
        skin->button_active_hot = LoadButton(src.button_active_hot);
        return skin;
    }


    bool
    Load
    (
        Root* root,
        core::vfs::FileSystem* fs,
        render::FontCache* font,
        const core::vfs::FilePath& path,
        render::TextureCache* cache
    )
    {
        ::gui::File f;
        const std::string load_result = core::LoadProtoJson(fs, &f, path);
        if(false == load_result.empty())
        {
            LOG_ERROR("Failed to load gui from {0}: {1}", path, load_result);
            return false;
        }

        root->cursor_image = cache->GetTexture
        (
            core::vfs::FilePath::FromScriptOrEmpty(f.cursor_image)
        );
        root->hover_image = cache->GetTexture
        (
            core::vfs::FilePath::FromScriptOrEmpty(f.hover_image)
        );

        std::map<std::string, Skin*> skin_map;

        for(const auto& skin: f.skins)
        {
            std::shared_ptr<Skin> skin_ptr = LoadSkin(skin, font);
            skin_map.insert(std::make_pair(skin.name, skin_ptr.get()));
            root->skins_.push_back(skin_ptr);
        }

        BuildLayoutContainer
        (
            fs,
            &root->state_,
            &root->container_,
            f.root,
            cache,
            skin_map
        );

        return root->container_.HasWidgets();
    }
}

