#include "SDL.h"

#include "log/log.h"

#include "base/cint.h"
#include "base/os.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_defaultshaders.h"
#include "core/viewportdef.h"
#include "base/lrud.h"
#include "core/viewport.h"

#include "render/debuggl.h"
#include "render/font.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/shader.h"
#include "render/spriterender.h"
#include "render/viewporthandler.h"

#include "gui/root.h"
#include "gui/widget.h"
#include "gui/visitor.h"
#include "gui/button.h"
#include "gui/panelwidget.h"
#include "gui/skin.h"

#include "window/key.h"
#include "window/imguilibrary.h"
#include "window/imgui_extra.h"
#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/engine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui_stdlib.h"

using namespace eu;
using namespace eu::core;
using namespace eu::gui;
using namespace eu::render;
using namespace eu::window;


// todo(Gustav): move to window/imgui_ext
bool
imgui_widget(const char* title, size2f* s)
{
    return ImGui::DragFloat2(title, &s->width);
}

void
imgui_widget(const char* title, std::string* str)
{
    ImGui::InputText(title, str);
}

bool
imgui_widget(const char* title, vec2f* v)
{
    return ImGui::DragFloat2(title, &v->x);
}

bool
imgui_widget(const char* title, bool b)
{
    bool bb = b;
    ImGui::Checkbox(title, &bb);
    return false;
}

bool
imgui_widget(const char* title, bool* b)
{
    return ImGui::Checkbox(title, b);
}


bool
imgui_widget(const char* title, eu::Lrud<float>* p)
{
    const auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushID(title);

    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

    bool changed = ImGui::DragFloat("L", &p->left);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("R", &p->right);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("U", &p->up);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("D", &p->down);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    ImGui::Text("%s", title);

    ImGui::EndGroup();
    ImGui::PopID();
    return changed;
}

bool
imgui_widget(const char* title, Rectf* r)
{
    const auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushID(title);

    ImGui::BeginGroup();
    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

    bool changed = ImGui::DragFloat("L", &r->left);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("R", &r->right);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("T", &r->top);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("B", &r->bottom);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    ImGui::Text("%s", title);

    ImGui::EndGroup();
    ImGui::PopID();
    return changed;
}


void
imgui_widget(const char* title, eu::render::Texture2* tex)
{
    if(tex == nullptr) { return; }

    ImGui::Text("%s", title);
    eu::window::imgui::imgui_image(tex);
}

void
imgui_widget(eu::render::ScalableSprite* sprite)
{
    imgui_widget("texture", sprite->texture.get());
}


bool
imgui_widget(State* state)
{
    imgui_widget("mouse", &state->mouse);
    imgui_widget("down", &state->mouse_down);
    imgui_widget("hot", state->hot != nullptr);
    imgui_widget("active", state->active != nullptr);
    imgui_widget("has active", &state->has_active);
    return false;
}

void
imgui_widget(TextData* data)
{
    imgui_widget("string", &data->string);
    ImGui::DragFloat("size", &data->size);
}

bool
imgui_widget(LayoutContainer* container);

struct ImguiWidgetVisitor : public Visitor
{
    void
    visit(Button* w) override
    {
        imgui_widget(w->sprite.get());
        if(ImGui::Button("Click"))
        {
            w->on_clicked();
        }
        imgui_widget(&w->text);
    }

    void
    visit(PanelWidget* w) override
    {
        imgui_widget(&w->container);
    }
};


bool
imgui_widget(Widget* w)
{
    ImGui::InputText("name", &w->name);
    imgui_widget("margin", &w->margin);
    imgui_widget("padding", &w->padding);
    imgui_widget("rect", &w->rect);

    if(ImGui::Button("Size"))
    {
        w->on_size_changed();
    }

    auto vis = ImguiWidgetVisitor{};
    w->visit(&vis);

    return false;
}

bool
imgui_widget(LayoutContainer* container)
{
    for(int widget_index=0; widget_index < c_sizet_to_int(container->widgets.size()); widget_index+= 1)
    {
        auto widget = container->widgets[widget_index];
        if(ImGui::TreeNode(widget->name.c_str()))
        {
            ImGui::PushID(widget_index);
            imgui_widget(widget.get());
            ImGui::PopID();

            ImGui::TreePop();
        }
    }

    return false;
}

void
imgui_widget(int id, eu::render::Glyph* gl)
{
    const std::string s = fmt::format("glyph {}", id);
    if(ImGui::TreeNode(s.c_str()) == false) { return; }

    ImGui::PushID(id);

    imgui_widget("sprite", &gl->sprite_rect);
    imgui_widget("texture", &gl->texture_rect);
    ImGui::DragFloat("advance", &gl->advance);

    ImGui::PopID();

    ImGui::TreePop();
}

void
imgui_widget(const char* title, eu::render::DrawableFont* font)
{
    if(ImGui::TreeNode(title) == false) {return;}

    imgui_widget("texture", font->texture.get());
    imgui_widget("background", font->background.get());

    for(auto& g: font->char_to_glyph)
    {
        imgui_widget(g.first, g.second.get());
    }

    ImGui::TreePop();
}

void
imgui_widget(Skin* skin)
{
    if(ImGui::TreeNode(skin->name.c_str()) == false) { return; }

    ImGui::InputText("name", &skin->name);
    imgui_widget("font", skin->font.get());
    ImGui::DragFloat("text size", &skin->text_size);

    ImGui::TreePop();
}


void
imgui_widget(Root* root)
{
    for(auto& skin: root->skins)
    {
        imgui_widget(skin.get());
    }
    imgui_widget("size", &root->size);
    imgui_widget(&root->state);
    imgui_widget(&root->container);

    imgui_widget("cursor", root->cursor_image.get());
    imgui_widget("hover", root->hover_image.get());
}


int
main(int argc, char* argv[])
{
    Engine engine;
    if(const auto ret = engine.setup(argparse::NameAndArguments::extract(argc, argv)); ret != 0)
    {
        return ret;
    }

    engine.file_system->set_write_root
    (
        std::make_shared<io::WriteRootPhysicalFolder>(get_current_directory())
    );

    TextureCache cache {engine.file_system.get()};

    const auto clear_color = NamedColor::blue;

    int window_width = 800;
    int window_height = 600;

    if
    (
        engine.create_window
        (
            "euphoria gui demo",
            window_width,
            window_height,
            true
        ) == false
    )
    {
        return -1;
    }


    ShaderProgram shader;
    attributes2d::add_attributes_to_shader(&shader);
    shader.load(engine.file_system.get(), io::FilePath{"~/shaders/sprite"});
    SpriteRenderer renderer(&shader);
    FontCache font_cache {engine.file_system.get(), &cache};

    use(&shader);
    shader.set_uniform(shader.get_uniform("image"), 0);

    auto root = eu::gui::Root
    {
        size2f::create_from_width_height
        (
            c_int_to_float(window_width),
            c_int_to_float(window_height)
        )
    };
    const auto gui_loaded = root.load
    (
        engine.file_system.get(),
        &font_cache,
        io::FilePath{"~/gui.json"},
        &cache
    );

    if(gui_loaded == false)
    {
        return -1;
    }

    auto viewport_handler = eu::render::ViewportHandler
    {
        engine.init.get(),
        nullptr
    };
    viewport_handler.add(&shader);
    viewport_handler.set_size(window_width, window_height);

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    // SDL_StartTextInput();

    bool running = true;

    int window_mouse_x = 0;
    int window_mouse_y = 0;
    SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
    bool mouse_lmb_down = false;

    engine.init->use_2d();

    auto handle_events = [&](bool show_imgui)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }

            if(show_imgui)
            {
                imgui::send_events_to_imgui(&e);
            }

            if(engine.on_resize(e, &window_width, &window_height))
            {
                viewport_handler.set_size(window_width, window_height);
                root.set_size
                (
                    size2f::create_from_width_height
                    (
                        c_int_to_float(window_width),
                        c_int_to_float(window_height)
                    )
                );
            }

            if(e.type == SDL_MOUSEMOTION)
            {
                window_mouse_x = e.motion.x;
                window_mouse_y = e.motion.y;
            }
            else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
            {
                const bool down = e.type == SDL_KEYDOWN;
                const auto key = to_key(e.key.keysym);
                if(down && key == Key::escape)
                {
                    running = false;
                }
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
            {
                const bool down = e.type == SDL_MOUSEBUTTONDOWN;
                window_mouse_x = e.button.x;
                window_mouse_y = e.button.y;
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_lmb_down = down;
                }
            }
            else if(e.type == SDL_TEXTINPUT)
            {
                // const std::string& input = e.text.text;
            }
        }
    };

    auto run_update = [&](bool show_imgui, float dt)
    {
        if(show_imgui && ImGui::GetIO().WantCaptureMouse)
        {
        }
        else
        {
            root.set_input_mouse
            (
                vec2f
                {
                    static_cast<float>(window_mouse_x),
                    static_cast<float>(window_height - window_mouse_y)
                },
                mouse_lmb_down
            );
        }

        root.update(dt);
    };

    auto render_example = [&](bool show_imgui)
    {
        engine.init->clear_screen(clear_color);
        root.render(&renderer);
        if(show_imgui)
        {
            imgui::imgui_render();
        }
        SDL_GL_SwapWindow(engine.window->window);
    };

    constexpr bool show_imgui = true;
    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float dt = static_cast<float>(now - last) / static_cast<float>(SDL_GetPerformanceFrequency());

        // imgui
        if(show_imgui)
        {
            imgui::begin_new_frame();

            ImGui::Begin("Gui");
            imgui_widget(&root);
            ImGui::End();
        }

        run_update(show_imgui, dt);
        render_example(show_imgui);
        handle_events(show_imgui);
    }

    return 0;
}
