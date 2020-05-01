#include "SDL.h"

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/ecs-systems.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_defaultshaders.h"
#include "core/proto.h"
#include "core/viewport.h"

#include "render/debuggl.h"
#include "render/fonts.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"
#include "render/shader.h"
#include "render/spriterender.h"
#include "render/viewporthandler.h"

#include "gui/root.h"
#include "gui/widget.h"

#include "window/key.h"
#include "window/imguilibrary.h"
#include "window/imgui_ext.h"
#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/engine.h"

#include "engine/loadworld.h"
#include "engine/systems.h"
#include "engine/dukintegration.h"
#include "engine/dukmathbindings.h"
#include "engine/dukprint.h"
#include "engine/input.h"
#include "engine/objectemplate.h"
#include "engine/components.h"
#include "engine/cameradata.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

LOG_SPECIFY_DEFAULT_LOGGER("samples-gui")


using namespace euphoria::core;
using namespace euphoria::gui;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::engine;


// todo(Gustav): move to window/imgui_ext
bool
ImWidget(const char* title, Sizef* s)
{
    return ImGui::DragFloat2(title, &s->width);
}

bool
ImWidget(const char* title, vec2f* v)
{
    return ImGui::DragFloat2(title, &v->x);
}

bool
ImWidget(const char* title, bool b)
{
    bool bb = b;
    ImGui::Checkbox(title, &bb);
    return false;
}

bool
ImWidget(const char* title, bool* b)
{
    return ImGui::Checkbox(title, b);
}


bool
ImWidget(const char* title, euphoria::gui::Lrtb* p)
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

    changed |= ImGui::DragFloat("T", &p->top);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    changed |= ImGui::DragFloat("B", &p->bottom);
    ImGui::PopItemWidth();

    ImGui::SameLine(0, spacing);

    ImGui::Text("%s", title);

    ImGui::EndGroup();
    ImGui::PopID();
    return changed;
}

bool
ImWidget(const char* title, Rectf* r)
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



bool
ImWidget(UiState* state)
{
    ImWidget("mouse", &state->mouse);
    ImWidget("down", &state->mouse_down);
    ImWidget("hot", state->hot != nullptr);
    ImWidget("active", state->active != nullptr);
    ImWidget("has active", &state->has_active);
    return false;
}

bool
ImWidget(Widget* w)
{
    ImWidget("margin", &w->margin);
    ImWidget("padding", &w->padding);
    ImWidget("rect", &w->rect_);

    return false;
}

bool
ImWidget(LayoutContainer* container)
{
    for(int i=0; i<container->widgets_.size(); i+= 1)
    {
        auto widget = container->widgets_[i];
        if(ImGui::TreeNode(widget->name.c_str()))
        {
            ImGui::PushID(i);
            ImWidget(widget.get());
            ImGui::PopID();

            ImGui::TreePop();
        }
    }

    return false;
}

void
DebugUi(Root* root)
{
    ImWidget("size", &root->size);
    ImWidget(&root->state);
    ImWidget(&root->container);
}

int
main(int argc, char* argv[])
{
    Engine engine;
    if(const auto ret = engine.Setup(argparse::Arguments::Extract(argc, argv)) != 0; ret != 0)
    {
        return ret;
    }

    engine.file_system->SetWrite
    (
        std::make_shared<vfs::FileSystemWriteFolder>(GetCurrentDirectory())
    );

    TextureCache cache {engine.file_system.get()};

    const auto clear_color = Color::Blue;

    ViewportHandler viewport_handler;

    int window_width  = 800;
    int window_height = 600;

    if
    (
        engine.CreateWindow
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

    viewport_handler.SetSize(window_width, window_height);

    Shader shader;
    attributes2d::PrebindShader(&shader);
    shader.Load(engine.file_system.get(), vfs::FilePath{"~/shaders/sprite"});
    SpriteRenderer renderer(&shader);
    FontCache      font_cache {engine.file_system.get(), &cache};

    auto root = Root{Sizef::FromWidthHeight(window_width, window_height)};
    const auto gui_loaded = root.Load
    (
        engine.file_system.get(),
        &font_cache,
        vfs::FilePath{"~/gui.json"},
        &cache
    );

    if(gui_loaded == false)
    {
        return -1;
    }

    Use(&shader);
    shader.SetUniform(shader.GetUniform("image"), 0);

    Uint64 now  = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    // SDL_StartTextInput();

    bool running = true;
    constexpr bool show_imgui = true;

    int window_mouse_x = 0;
    int window_mouse_y = 0;
    SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
    bool mouse_lmb_down = false;

    engine.init->Use2d();

    while(running)
    {
        last = now;
        now = SDL_GetPerformanceCounter();
        const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
        SDL_Event e;

        // imgui
        if(show_imgui)
        {
            engine.imgui->StartNewFrame();

            ImGui::Begin("Gui");
            DebugUi(&root);
            ImGui::End();
        }

        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }

            if(show_imgui)
            {
                engine.imgui->ProcessEvents(&e);
            }

            if(engine.HandleResize(e, &window_width, &window_height))
            {
                viewport_handler.SetSize(window_width, window_height);
                root.Resize(Sizef::FromWidthHeight(window_width, window_height));
            }

            if(e.type == SDL_MOUSEMOTION)
            {
                window_mouse_x = e.motion.x;
                window_mouse_y = e.motion.y;
            }
            else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
            {
                const bool down = e.type == SDL_KEYDOWN;
                const auto key = ToKey(e.key.keysym);
                if(down && key == Key::ESCAPE)
                {
                    running = false;
                }
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN
                    || e.type == SDL_MOUSEBUTTONUP)
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
        
        if(show_imgui && ImGui::GetIO().WantCaptureMouse)
        {
        }
        else
        {
            root.SetInputMouse
            (
                vec2f{window_mouse_x, window_mouse_y},
                mouse_lmb_down
            );
        }

        root.Step(dt);

        engine.init->ClearScreen(clear_color);
        root.Render(&renderer);
        if(show_imgui)
        {
            engine.imgui->Render();
        }
        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
