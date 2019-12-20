#include "core/aabb.h"
#include "core/axisangle.h"
// #include "core/draw.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/mat4.h"
#include "core/os.h"
#include "core/path.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/texturetypes.h"
#include "core/str.h"
#include "core/mat4.h"
#include "core/camera.h"
#include "core/log.h"
#include "core/plane.h"
#include "core/intersection.h"
#include "core/lines.h"
#include "core/orbitcontroller.h"

// #include "render/camera.h"
#include "render/defaultfiles.h"
#include "render/materialshadercache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/viewport.h"
#include "render/world.h"
#include "render/compiledmesh.h"
#include "render/debuggl.h"
#include "render/init.h"
#include "render/materialshader.h"
#include "render/texturecache.h"
#include "render/viewporthandler.h"
#include "render/actor.h"
#include "render/compiledlines.h"
#include "render/positionedlines.h"

#include "window/filesystem.h"
#include "window/imgui_ext.h"
#include "window/imguilibrary.h"
#include "window/sdlglcontext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/timer.h"
#include "window/key.h"
#include "window/engine.h"
#include "window/imgui_icons.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>

#include "t3d/tilelibrary.h"

using namespace euphoria::core;
using namespace euphoria::core::vfs;
using namespace euphoria::t3d;
using namespace euphoria::render;
using namespace euphoria::window;

LOG_SPECIFY_DEFAULT_LOGGER("t3d")


struct Editor;

struct Tool
{
    virtual ~Tool() = default;

    virtual void
    MeshHasChanged(Editor* editor) = 0;


    virtual bool
    IsBusy(Editor* editor) = 0;


    virtual void
    Step(Editor* editor) = 0;


    virtual void
    OnMouse(Editor* editor, MouseButton key, bool down) = 0;
    
    
    virtual void
    OnKey(Editor* editor, Key key, bool down) = 0;
    
    
    virtual void
    OnScroll(Editor* editor, const vec2i& scroll) = 0;
    
    
    virtual void
    OnEditor(Editor* editor) = 0;
};

struct Tools;

struct ToolAction
{
    virtual ~ToolAction() = default;


    virtual void
    Act(Tools* tools) = 0;
};

struct PushToolAction : public ToolAction
{
    std::shared_ptr<Tool> new_tool;


    void
    Act(Tools* tools) override;


    explicit PushToolAction(std::shared_ptr<Tool> anew_tool)
        : new_tool(anew_tool)
    {}
};

struct PopToolAction : public ToolAction
{
    void
    Act(Tools* tools) override;
};

struct Tools
{
    Tool*
    GetCurrentTool()
    {
        ASSERT(!current_tool.empty());
        Tool* tool = current_tool.rbegin()->get();
        ASSERT(tool);
        return tool;
    }


    void
    PushTool(std::shared_ptr<Tool> new_tool)
    {
        pending_actions.push_back(std::make_shared<PushToolAction>(new_tool));
    }


    void
    PopTool()
    {
        pending_actions.push_back(std::make_shared<PopToolAction>());
    }


    void
    PerformTools()
    {
        for(auto t: pending_actions)
        {
            t->Act(this);
        }
        pending_actions.resize(0);
    }

    std::vector<std::shared_ptr<Tool>>       current_tool;
    std::vector<std::shared_ptr<ToolAction>> pending_actions;
};


void
PushToolAction::Act(Tools* tools)
{
    tools->current_tool.push_back(new_tool);
}

void
PopToolAction::Act(Tools* tools)
{
    tools->current_tool.pop_back();
}


struct Editor
{
    World*         world;
    TileLibrary*   tile_library;
    CompiledCamera camera;
    Viewport       viewport;


    Editor(World* aworld, TileLibrary* atile_library)
        : world(aworld)
        , tile_library(atile_library)
        , camera(mat4f::Identity(), mat4f::Identity())
        , viewport(Recti::FromWidthHeight(10, 10))
    {}


    vec2i mouse = vec2i {0, 0};
    Tools tools;


    // current tool callbacks
    void
    MeshHasChanged()
    {
        tools.GetCurrentTool()->MeshHasChanged(this);
    }


    bool
    IsBusy()
    {
        return tools.GetCurrentTool()->IsBusy(this);
    }


    void
    Step()
    {
        tools.GetCurrentTool()->Step(this);
    }


    void
    OnMouse(MouseButton button, bool down)
    {
        tools.GetCurrentTool()->OnMouse(this, button, down);
    }


    void
    OnKey(Key key, bool down)
    {
        tools.GetCurrentTool()->OnKey(this, key, down);
    }


    void
    OnScroll(const vec2i& scroll)
    {
        tools.GetCurrentTool()->OnScroll(this, scroll);
    }


    void
    OnEditor()
    {
        tools.GetCurrentTool()->OnEditor(this);
    }

    // todo(Gustav): move camera here so we can have camera movement
    // change so that fps control rotate focuspoint around current camera pos

    std::vector<std::shared_ptr<Actor>> actors;
    std::shared_ptr<CompiledMesh>       selected_mesh;
};

struct NoTool : public Tool
{
    void
    MeshHasChanged(Editor*) override
    {}


    bool
    IsBusy(Editor*) override
    {
        return false;
    }


    void
    Step(Editor*) override
    {}


    void
    OnMouse(Editor*, MouseButton, bool) override
    {}


    void
    OnKey(Editor*, Key, bool) override
    {}


    void
    OnScroll(Editor*, const vec2i&) override
    {}


    void
    OnEditor(Editor*) override
    {
        ImGui::Text("<No tool>");
    }
};

struct PlaceMeshOnPlane : public Tool
{
    std::shared_ptr<Actor> actor;
    Plane plane = Plane::FromNormalAndPoint(unit3f::Up(), vec3f::Zero());


    PlaceMeshOnPlane(std::shared_ptr<Actor> aactor) : actor(aactor) {}


    void
    MeshHasChanged(Editor* editor) override
    {
        // todo(Gustav): ugh... fix this ugly hack.. this currently crashes/asserts
        actor->mesh_ = editor->selected_mesh;
    }


    bool
    IsBusy(Editor*) override
    {
        return true;
    }


    void
    Step(Editor* editor) override
    {
        auto ray = editor->camera
                           .ClipToWorldRay(
                                   editor->viewport.ToClipCoord(editor->mouse))
                           .GetNormalized();

        const auto t = GetIntersection(ray, plane);

        // LOG_INFO("Step " << editor->mouse << ", " << t);

        if(t < 0)
        {
            return;
        }
        // do intersection with plane...
        const auto p = ray.GetPoint(t);
        actor->SetPosition(p);
    }


    void
    OnMouse(Editor* editor, MouseButton button, bool down) override
    {
        if(down)
            return;
        switch(button)
        {
        case MouseButton::LEFT: editor->tools.PopTool(); break;
        default: break;
        }
    }


    void
    OnKey(Editor* editor, Key key, bool down) override
    {
        if(down)
            return;
        switch(key)
        {
        case Key::RETURN: editor->tools.PopTool(); break;
        default: break;
        }
    }


    void
    OnScroll(Editor*, const vec2i&) override
    {}


    void
    OnEditor(Editor*) override
    {
        ImGui::Text("Placing object in world!");
    }
};


struct T3d
{
    std::shared_ptr<Engine> engine;
    ViewportHandler viewport_handler;
    std::shared_ptr<MaterialShaderCache> material_shader_cache;
    std::shared_ptr<TextureCache> texture_cache;
    std::shared_ptr<TileLibrary> tile_library;
    std::shared_ptr<World> world;
    Camera camera;
    std::shared_ptr<Editor> editor;
    bool running = true;
    std::shared_ptr<SdlTimer> timer;
    bool immersive_mode = false;
    std::shared_ptr<PositionedLines> grid;
    bool grid_visible = true;
    float grid_small_step = 0.5f;
    int grid_big_step_interval = 5;
    float grid_normal = 1.0f;
    int grid_size = 10;
    bool enviroment_window = false;
    bool camera_window     = false;
    bool tiles_window      = true;
    bool grid_window      = true;
    OrbitController orbit;
    bool mmb_down = false;
    bool shift_down = false;


    [[nodiscard]] bool
    Start(const argparse::Args& args)
    {
        engine = std::make_shared<Engine>();

        if(engine->Setup(args) == false)
        {
            return false;
        }

        int width  = 1280;
        int height = 720;

        if(engine->CreateWindow("t3d", width, height) == false)
        {
            return false;
        }

        viewport_handler = ViewportHandler {};
        viewport_handler.SetSize(width, height);

        material_shader_cache = std::make_shared<MaterialShaderCache>(engine->file_system.get());

        // SET_ENUM_VALUES(TextureType, SetupTextureNames);
        SET_ENUM_FROM_FILE(
                engine->file_system.get(), "texture_types.json", TextureType);

        texture_cache = std::make_shared<TextureCache>(engine->file_system.get());

        tile_library = std::make_shared<TileLibrary>(engine->file_system.get());

        world = std::make_shared<World>();

        editor = std::make_shared<Editor>(world.get(), tile_library.get());
        editor->tools.PushTool(std::make_shared<NoTool>());

        timer = std::make_shared<SdlTimer>();
        
        UpdateGrid();

        engine->window->EnableCharEvent(!immersive_mode);

        return true;
    }


    void
    AddLibrary(const std::string& path)
    {
        tile_library->AddDirectory(path, material_shader_cache.get(), texture_cache.get());
    }


    void
    AddSingleGridLine
    (
        Lines& def,
        float size,
        float x,
        const Rgb& color
    )
    {
        def.AddLine(vec3f {x, 0, -size}, vec3f {x, 0, size}, color);
        def.AddLine(vec3f {-size, 0, x}, vec3f {size, 0, x}, color);
        def.AddLine(vec3f {-x, 0, -size}, vec3f {-x, 0, size}, color);
        def.AddLine(vec3f {-size, 0, -x}, vec3f {size, 0, -x}, color);
    }


    void
    UpdateGrid()
    {
        constexpr auto smallest_step = 0.01f;
        constexpr auto small_color = Color::Gray;
        constexpr auto big_color   = Color::Black;
        constexpr auto x_color     = Color::PureBlue;
        constexpr auto z_color     = Color::PureRed;
        constexpr auto y_color     = Color::PureYellow;

        if(grid)
        {
            grid->remove_this = true;
            grid = nullptr;
        }

        if(grid_visible == false)
        {
            return;
        }

        const auto small_step = Max(smallest_step, grid_small_step);
        const auto size = Abs(grid_size * grid_small_step);
        const auto normal = grid_normal;

        auto def = Lines {};

        if(normal > 0)
        {
            def.AddLine(vec3f {0, 0, 0}, vec3f {0, normal, 0}, y_color);
        }

        for(int index = 0; index < grid_size; index += 1)
        {
            float x = small_step * (index+1);

            auto color = big_color;
            if(grid_big_step_interval > 1)
            {
                if(((index+1) % grid_big_step_interval) != 0 )
                {
                    color = small_color;
                }
            }
            AddSingleGridLine(def, size, x, color);
        }

        def.AddLine(vec3f {-size, 0, 0}, vec3f {size, 0, 0}, x_color);
        def.AddLine(vec3f {0, 0, -size}, vec3f {0, 0, size}, z_color);

        auto compiled = Compile(material_shader_cache.get(), def);
        grid     = std::make_shared<PositionedLines>(compiled);
        world->AddActor(grid);
    }


    void
    OnSingleEvent(const SDL_Event& e)
    {
        int window_width  = 0;
        int window_height = 0;
        if(engine->HandleResize(e, &window_width, &window_height))
        {
            viewport_handler.SetSize(window_width, window_height);
        }
        if(show_imgui)
        {
            engine->imgui->ProcessEvents(&e);
        }

        auto&      io = ImGui::GetIO();
        const auto forward_keyboard
                = !(show_imgui && io.WantCaptureKeyboard);
        const auto forward_mouse = !(show_imgui && io.WantCaptureMouse);

        switch(e.type)
        {
        case SDL_QUIT: running = false; break;
        case SDL_MOUSEMOTION:
        {
            const auto mouse_position = vec2i
            (
                e.motion.x,
                viewport_handler.height - e.motion.y
            );
            const auto mouse_movement = vec2i
            (
                e.motion.xrel,
                e.motion.yrel
            );
            OnMouseMovement(mouse_position, mouse_movement, forward_mouse);
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            const auto key = ToKey(e.key.keysym);
            const bool down = e.type == SDL_KEYDOWN;
            OnKey(key, down, forward_keyboard);
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                const auto button = ToKey(e.button);
                const bool down = e.type == SDL_MOUSEBUTTONDOWN;
                OnMouseButton(button, down, forward_mouse);
            }
            break;

        case SDL_MOUSEWHEEL:
            OnMouseWheel(e, forward_mouse);
            break;


        default:
            // ignore other events
            break;
        }
    }


    void
    OnMouseMovement(const vec2i& position, const vec2i& movement, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->mouse = position;

            if(mmb_down)
            {
                const auto mm = movement.StaticCast<float>();
                if(shift_down) { orbit.Pan(mm.x, mm.y); }
                else { orbit.Rotate(mm.x, mm.y); }
            }
        }
    }


    void
    OnKey(Key key, bool down, bool forward_keyboard)
    {
        if(forward_keyboard)
        {
            switch(key)
            {
            case Key::ESCAPE:
                if(down)
                {
                    running = false;
                }
                break;
            case Key::TAB:
                if(!down)
                {
                    immersive_mode = !immersive_mode;
                    engine->window->KeepWithin(immersive_mode);
                    engine->window->EnableCharEvent(!immersive_mode);
                }
                break;
            default:
                if(forward_keyboard)
                {
                    editor->OnKey(key, down);
                }
                break;
            }
        }
        
        switch(key)
        {
            case Key::SHIFT_LEFT:
            case Key::SHIFT_RIGHT:
                shift_down = down;
                break;
            default:
                break;
        }
    }


    void
    OnMouseButton(MouseButton button, bool down, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->OnMouse(button, down);
        }
        
        switch(button)
        {
        case MouseButton::MIDDLE:
            mmb_down = down;
            break;
        default:
            break;
        }
    }


    void
    OnMouseWheel(const SDL_Event& e, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->OnScroll(vec2i(e.wheel.x, e.wheel.y));
            orbit.Zoom(e.wheel.y);
        }
    }


    void
    HandleEvents()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            OnSingleEvent(e);
        }
    }


    bool show_imgui = false;


    void
    ProcessImgui()
    {
        if(ImGui::BeginMainMenuBar())
        {
            OnMainMenu();
        }
        ImGui::EndMainMenuBar();
        
        if(enviroment_window)
        {
            ImGui::Begin("Enviroment", &enviroment_window);
            OnEnviromentWindow();
            ImGui::End();
        }

        if(grid_window)
        {
            BeginFixedOverlay(ImguiCorner::TopLeft, "grid overlay", 10.0f, 30.0f);
            OnGridWindow();
            ImGui::End();
        }

        if(camera_window)
        {
            ImGui::Begin("Camera", &camera_window);
            OnCameraWindow();
            ImGui::End();
        }

        if(tiles_window)
        {
            ImGui::Begin("Tiles", &tiles_window);
            OnTileWindow();
            ImGui::End();
        }
    }


    void
    OnMainMenu()
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Exit", "Ctrl+Q"))
            {
                running = false;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Enviroment", nullptr, &enviroment_window);
            ImGui::MenuItem("Camera", nullptr, &camera_window);
            ImGui::MenuItem("Tiles", nullptr, &tiles_window);
            ImGui::MenuItem("Grid", nullptr, &grid_window);
            ImGui::EndMenu();
        }
    }


    void
    OnEnviromentWindow()
    {
        ImGui::Combo
        (
            "Type",
            reinterpret_cast<int*>(&world->light.type),
            "Directional\0Point\0Spot\0\0"
        );
        ImGuiColorEdit3("Ambient", &world->light.ambient);
        ImGuiColorEdit3("Diffuse", &world->light.diffuse);
        ImGuiColorEdit3("Specular", &world->light.specular);
    }


    void
    OnGridWindow()
    {
        constexpr auto uistep = 0.01f;
        constexpr auto uimin = 0.0f;
        constexpr auto uimax = 100.0f;
        bool dirty = false;

        
        const std::string str = Str{} << grid_small_step;
        constexpr auto popup_grid = "popup_grid";
        if(ImGui::Button(str.c_str()))
        {
            ImGui::OpenPopup(popup_grid);
        }
        if(ImGui::BeginPopup(popup_grid))
        {
            dirty = ImGui::DragFloat
            (
                "Snap interval",
                &grid_small_step,
                uistep,
                uimin,
                uimax
            ) || dirty;
            dirty = ImGui::DragInt
            (
                "Major line increment",
                &grid_big_step_interval
            ) || dirty;
            dirty = ImGui::DragFloat
            (
                "Normal size",
                &grid_normal,
                uistep,
                uimin,
                uimax
            ) || dirty;
            dirty = ImGui::DragInt("Lines on grid", &grid_size)
                || dirty;

            ImGui::EndPopup();
        }

        if(ImGui::Button(grid_visible
            ? ICON_MDI_GRID
            : ICON_MDI_GRID_OFF
        ))
        {
            grid_visible = !grid_visible;
            dirty = true;
        }

        if(dirty)
        {
            UpdateGrid();
        }
    }


    void
    OnCameraWindow()
    {
        ImguiAngleSlider
        (
            "Horizontal",
            &orbit.horizontal_rotation,
            Angle::Zero(),
            Angle::OneTurn()
        );
        ImguiAngleSlider
        (
            "Vertical",
            &orbit.vertical_rotation,
            -Angle::Quarter(),
            Angle::Quarter()
        );
        ImGui::InputFloat3("Position", orbit.center.GetDataPtr());
        ImGui::Spacing();
        ImGui::InputFloat("Distance", &orbit.distance);
        auto sens = [](const char* label, Sensitivity* s)
        {
            ImGui::PushID(label);
            ImGui::Checkbox("##inverted", &s->inverted);
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::DragFloat(label, &s->value, 0.1f, 0.0f);
        };
        sens("Pan dX", &orbit.pan_dx);
        sens("Pan dY", &orbit.pan_dy);
        sens("Rotate dY", &orbit.rotate_dx);
        sens("Rotate dX", &orbit.rotate_dy);
        sens("Zoom", &orbit.zoom);
    }


    void
    OnTileWindow()
    {
        if(!tile_library->tiles.empty())
        {
            ImGui::ListBoxHeader("Tiles");
            for(auto tile: tile_library->tiles)
            {
                std::string display = Str {}
                    << tile->name << ": "
                    << tile->aabb.GetSize();
                if(ImGui::Selectable(
                            display.c_str(),
                            editor->selected_mesh == tile->mesh))
                {
                    editor->selected_mesh = tile->mesh;
                    editor->MeshHasChanged();
                }
            }
            ImGui::ListBoxFooter();

            if(ImGui::Button("Add"))
            {
                if(editor->selected_mesh && !editor->IsBusy())
                {
                    auto actor = std::make_shared<Actor>(
                            editor->selected_mesh);
                    world->AddActor(actor);
                    editor->actors.emplace_back(actor);

                    editor->tools.PushTool
                    (
                        std::make_shared<PlaceMeshOnPlane>(actor)
                    );
                }
            }
        }
        editor->OnEditor();
    }


    void
    Render()
    {
        auto viewport = viewport_handler.GetFullViewport();
        editor->camera = camera.Compile(viewport.GetAspectRatio());
        editor->viewport = viewport;
        world->Render(viewport, camera);
    }


    void
    Frame()
    {
        show_imgui = !immersive_mode;
        const float delta      = timer->Update();

        world->Step();
        editor->tools.PerformTools();

        HandleEvents();

        editor->Step();

        if(show_imgui)
        {
            engine->imgui->StartNewFrame();
            ProcessImgui();
        }

        camera.position = orbit.GetCameraPosition();
        camera.rotation = orbit.GetRotation();

        engine->init->ClearScreen(Color::LightGray);

        Render();

        if(show_imgui)
        {
            engine->imgui->Render();
        }

        SDL_GL_SwapWindow(engine->window->window);
    }
};


int
main(int argc, char** argv)
{
    T3d t3d;

    if(t3d.Start(argparse::Args::Extract(argc, argv)) == false)
    {
        return -1;
    }

    t3d.AddLibrary("world");

    if(t3d.tile_library->tiles.empty())
    {
        LOG_ERROR("No tile loaded!");
        return -2;
    }

    while(t3d.running)
    {
        t3d.Frame();
    }

    return 0;
}
