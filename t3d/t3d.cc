#include "t3d/t3d.h"

#include "SDL.h"
#include "imgui/imgui.h"

#include "core/lines.h"
#include "core/colors.h"
#include "core/enum.h"
#include "core/texturetypes.h"

#include "render/positionedlines.h"
#include "render/world.h"
#include "render/actor.h"
#include "render/init.h"

#include "render/texturecache.h"
#include "render/materialshadercache.h"

#include "window/engine.h"
#include "window/sdlwindow.h"
#include "window/timer.h"
#include "window/imguilibrary.h"
#include "window/imgui_icons.h"
#include "window/key.h"
#include "window/imgui_ext.h"

#include "t3d/tilelibrary.h"
#include "t3d/editor.h"
#include "t3d/tool_notool.h"
#include "t3d/tool_placemeshonplane.h"


namespace euphoria::t3d
{
    T3d::T3d()
    {
        pending_files.extensions = std::vector<std::string>
        {
            "obj",
            "fbx"
        };
    }


    T3d::~T3d()
    {
    }


    [[nodiscard]] int
    T3d::Start(const core::argparse::Arguments& args)
    {
        engine = std::make_shared<window::Engine>();

        if(const auto r = engine->Setup(args); r != 0)
        {
            return r;
        }

        int width  = 1280;
        int height = 720;

        if(engine->CreateWindow("t3d", width, height) == false)
        {
            return -2;
        }

        viewport_handler.SetSize(width, height);

        material_shader_cache = std::make_shared<render::MaterialShaderCache>(engine->file_system.get());

        // SET_ENUM_VALUES(TextureType, SetupTextureNames);
        SET_ENUM_FROM_FILE
        (
            engine->file_system.get(),
            core::vfs::FilePath{"~/texture_types.json"},
            core::TextureType
        );

        texture_cache = std::make_shared<render::TextureCache>(engine->file_system.get());

        tile_library = std::make_shared<TileLibrary>(engine->file_system.get());

        world = std::make_shared<render::World>();

        editor = std::make_shared<Editor>(&grid_data, world.get(), tile_library.get());
        editor->tools.PushTool(std::make_shared<NoTool>());

        timer = std::make_shared<window::SdlTimer>();
        
        UpdateGrid();

        engine->window->EnableCharEvent(!immersive_mode);

        return 0;
    }


    void
    T3d::AddLibrary(const core::vfs::DirPath& path)
    {
        pending_files.AddDirectory(path, engine->file_system.get());
    }


    void
    T3d::AddSingleGridLine
    (
        core::Lines& def,
        float size,
        float x,
        const core::Rgb& color
    )
    {
        def.AddLine(core::vec3f {x, 0, -size},  core::vec3f {x, 0, size}, color);
        def.AddLine(core::vec3f {-size, 0, x},  core::vec3f {size, 0, x}, color);
        def.AddLine(core::vec3f {-x, 0, -size}, core::vec3f {-x, 0, size}, color);
        def.AddLine(core::vec3f {-size, 0, -x}, core::vec3f {size, 0, -x}, color);
    }


    void
    T3d::UpdateGrid()
    {
        constexpr auto smallest_step = 0.01f;
        constexpr auto small_color = core::Color::Gray;
        constexpr auto big_color   = core::Color::Black;
        constexpr auto x_color     = core::Color::PureBlue;
        constexpr auto z_color     = core::Color::PureRed;
        constexpr auto y_color     = core::Color::PureYellow;

        if(grid)
        {
            grid->remove_this = true;
            grid = nullptr;
        }

        if(grid_data.visible == false)
        {
            return;
        }

        const auto small_step = core::Max(smallest_step, grid_data.small_step);
        const auto size = core::Abs(grid_data.size * grid_data.small_step);
        const auto normal = grid_data.normal;

        auto def = core::Lines {};

        if(normal > 0)
        {
            def.AddLine(core::vec3f {0, 0, 0}, core::vec3f {0, normal, 0}, y_color);
        }

        for(int index = 0; index < grid_data.size; index += 1)
        {
            float x = small_step * (index+1);

            auto color = big_color;
            if(grid_data.big_step_interval > 1)
            {
                if(((index+1) % grid_data.big_step_interval) != 0 )
                {
                    color = small_color;
                }
            }
            AddSingleGridLine(def, size, x, color);
        }

        def.AddLine(core::vec3f {-size, 0, 0}, core::vec3f {size, 0, 0}, x_color);
        def.AddLine(core::vec3f {0, 0, -size}, core::vec3f {0, 0, size}, z_color);

        auto compiled = Compile(material_shader_cache.get(), def);
        grid     = std::make_shared<render::PositionedLines>(compiled);
        world->AddActor(grid);
    }


    void
    T3d::OnSingleEvent(const SDL_Event& e)
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
            const auto mouse_position = core::vec2i
            (
                e.motion.x,
                viewport_handler.height - e.motion.y
            );
            const auto mouse_movement = core::vec2i
            (
                e.motion.xrel,
                e.motion.yrel
            );
            OnMouseMovement(mouse_position, mouse_movement, forward_mouse);
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            const auto key = window::ToKey(e.key.keysym);
            const bool down = e.type == SDL_KEYDOWN;
            OnKey(key, down, forward_keyboard);
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                const auto button = window::ToKey(e.button);
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
    T3d::OnMouseMovement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse)
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
    T3d::OnKey(core::Key key, bool down, bool forward_keyboard)
    {
        if(forward_keyboard)
        {
            switch(key)
            {
            case core::Key::ESCAPE:
                if(down)
                {
                    running = false;
                }
                break;
            case core::Key::TAB:
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
            case core::Key::SHIFT_LEFT:
            case core::Key::SHIFT_RIGHT:
                shift_down = down;
                break;
            default:
                break;
        }
    }


    void
    T3d::OnMouseButton(core::MouseButton button, bool down, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->OnMouse(button, down);
        }
        
        switch(button)
        {
        case core::MouseButton::MIDDLE:
            mmb_down = down;
            break;
        default:
            break;
        }
    }


    void
    T3d::OnMouseWheel(const SDL_Event& e, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->OnScroll(core::vec2i(e.wheel.x, e.wheel.y));
            orbit.Zoom(e.wheel.y);
        }
    }


    void
    T3d::HandleEvents()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            OnSingleEvent(e);
        }
    }


    void
    T3d::ProcessImgui()
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
            window::BeginFixedOverlay(window::ImguiCorner::TopLeft, "grid overlay", 10.0f, 30.0f);
            OnGridWindow();
            ImGui::End();
        }

        if(pending_files.HasMoreFiles())
        {
            window::BeginFixedOverlay(window::ImguiCorner::BottomLeft, "pending files");
            const auto frac = pending_files.index / static_cast<float>(pending_files.files.size());
            window::ImguiLabel("Loading tiles...");
            ImGui::ProgressBar(frac, ImVec2{120, 0});
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

        if(lister_window)
        {
            ImGui::Begin("Lister", &lister_window);
            OnListerWindow();
            ImGui::End();
        }

        if(preference_window)
        {
            ImGui::Begin("Preferences", &preference_window);
            OnPreferenceWindow();
            ImGui::End();
        }
    }


    void
    T3d::OnMainMenu()
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
            ImGui::MenuItem("Lister", nullptr, &lister_window);
            ImGui::MenuItem("Preferences", nullptr, &preference_window);
            ImGui::EndMenu();
        }
    }


    void
    T3d::OnListerWindow()
    {
        auto actors = editor->actors;
        if(actors.empty())
        {
            return;
        }

        ImGui::ListBoxHeader("Items");
        for(auto actor: actors)
        {
            ASSERT(actor->tile);
            ASSERT(actor->tile);
            const auto p = actor->actor->GetPosition();
            std::string display = core::Str {}
                << actor->tile->name
                << " "
                << p;
            if(ImGui::Selectable(
                        display.c_str(),
                        actor->is_selected))
            {
                editor->SetAllSelected(false);
                actor->is_selected = !actor->is_selected;
            }
        }
        ImGui::ListBoxFooter();

        if(ImGui::Button("Select none"))
        {
            editor->SetAllSelected(false);
        }
        ImGui::SameLine();
        if(ImGui::Button("Select all"))
        {
            editor->SetAllSelected(true);
        }
    }


    void
    T3d::OnEnviromentWindow()
    {
        ImGui::Combo
        (
            "Type",
            reinterpret_cast<int*>(&world->light.type),
            "Directional\0Point\0Spot\0\0"
        );
        window::ImGuiColorEdit3("Ambient", &world->light.ambient);
        window::ImGuiColorEdit3("Diffuse", &world->light.diffuse);
        window::ImGuiColorEdit3("Specular", &world->light.specular);
    }


    void
    T3d::OnGridWindow()
    {
        constexpr auto uistep = 0.01f;
        constexpr auto uimin = 0.0f;
        constexpr auto uimax = 100.0f;
        bool dirty = false;

        
        const std::string str = core::Str{} << grid_data.small_step;
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
                &grid_data.small_step,
                uistep,
                uimin,
                uimax
            ) || dirty;
            dirty = ImGui::DragInt
            (
                "Major line increment",
                &grid_data.big_step_interval
            ) || dirty;
            dirty = ImGui::DragFloat
            (
                "Normal size",
                &grid_data.normal,
                uistep,
                uimin,
                uimax
            ) || dirty;
            dirty = ImGui::DragInt("Lines on grid", &grid_data.size)
                || dirty;

            ImGui::EndPopup();
        }

        if(ImGui::Button(grid_data.visible
            ? ICON_MDI_GRID
            : ICON_MDI_GRID_OFF
        ))
        {
            grid_data.visible = !grid_data.visible;
            dirty = true;
        }
        Help("Toggle the grid visibility.");

        if(ImGui::Button(grid_data.snap_enabled
            ? ICON_MDI_AXIS_ARROW_LOCK
            : ICON_MDI_AXIS_ARROW
        ))
        {
            grid_data.snap_enabled = !grid_data.snap_enabled;
            dirty = true;
        }
        Help("Toggle the grid snapping");

        if(dirty)
        {
            UpdateGrid();
        }
    }


    void
    T3d::OnCameraWindow()
    {
        window::ImguiAngleSlider
        (
            "Horizontal",
            &orbit.horizontal_rotation,
            core::Angle::Zero(),
            core::Angle::OneTurn()
        );
        window::ImguiAngleSlider
        (
            "Vertical",
            &orbit.vertical_rotation,
            -core::Angle::Quarter(),
            core::Angle::Quarter()
        );
        ImGui::InputFloat3("Position", orbit.center.GetDataPtr());
        ImGui::Spacing();
        ImGui::InputFloat("Distance", &orbit.distance);
        auto sens = [](const char* label, core::Sensitivity* s)
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
    T3d::OnTileWindow()
    {
        // if(!tile_library->tiles.empty())
        if(ImGui::Button(ICON_MDI_PLUS) && !editor->IsBusy())
        {
            if(editor->selected_mesh == nullptr)
            {
                editor->selected_mesh = tile_library->FirstTile();
            }

            // todo(Gustav): add message if tile library is empty...

            if(editor->selected_mesh)
            {
                editor->SetAllSelected(false);
                auto placed = std::make_shared<PlacedMesh>();
                placed->tile = editor->selected_mesh;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::Actor>
                (
                        placed->tile->mesh
                );
                world->AddActor(placed->actor);
                editor->actors.emplace_back(placed);

                editor->tools.PushTool
                (
                    std::make_shared<PlaceMeshOnPlane>(placed)
                );
            }
        }
        Help("Place a new tile in the world");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MDI_PLUS_BOX_MULTIPLE))
        {
            auto selected = editor->GetFirstSelectedOrNull();
            if(selected != nullptr && !editor->IsBusy())
            {
                editor->SetAllSelected(false);
                auto placed = std::make_shared<PlacedMesh>();
                placed->tile = selected->tile;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::Actor>
                (
                        placed->tile->mesh
                );
                world->AddActor(placed->actor);
                editor->actors.emplace_back(placed);

                editor->tools.PushTool
                (
                    std::make_shared<PlaceMeshOnPlane>(placed)
                );
            }
        }
        Help("Duplicate the selected tile.");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MDI_ARROW_COLLAPSE_DOWN))
        {
            auto placed = editor->GetFirstSelectedOrNull();
            if(placed != nullptr && !editor->IsBusy())
            {
                editor->selected_mesh = placed->tile;
                editor->tools.PushTool
                (
                    std::make_shared<PlaceMeshOnPlane>(placed)
                );
            }
        }
        Help("Place or replace selected tile on a new place.");

        editor->OnEditor();
    }


    void
    T3d::OnPreferenceWindow()
    {
        constexpr const char* const help_styles_separated_by_zeros =
            "No help\0"
            "Help text\0"
            "Help marker\0"
            ;
        ImGui::Combo("Help style", &help_style, help_styles_separated_by_zeros);
    }


    void
    T3d::Render()
    {
        auto viewport = viewport_handler.GetFullViewport();
        editor->camera = camera.Compile(viewport.GetAspectRatio());
        editor->viewport = viewport;
        world->Render(viewport, camera);
    }


    void
    T3d::Frame()
    {
        show_imgui = !immersive_mode;
        //  const float delta =
            timer->Update();

        world->Step();
        editor->tools.PerformTools();

        HandleEvents();

        editor->Step();

        if(pending_files.HasMoreFiles())
        {
            const auto file = pending_files.NextFile();
            tile_library->AddFile(file, material_shader_cache.get(), texture_cache.get());
        }

        if(show_imgui)
        {
            engine->imgui->StartNewFrame();
            ProcessImgui();
        }

        camera.position = orbit.GetCameraPosition();
        camera.rotation = orbit.GetRotation();

        engine->init->ClearScreen(core::Color::LightGray);

        Render();

        if(show_imgui)
        {
            engine->imgui->Render();
        }

        SDL_GL_SwapWindow(engine->window->window);
    }

    void
    T3d::Help(const char* desc)
    {
        switch (help_style)
        {
        case 0:
            return;
        case 1:
            window::HelpText(desc);
            break;
        case 2:
            window::HelpMarker(desc); 
            break;
        default:
            DIE("Invalid style");
            break;
        }
    }
}
