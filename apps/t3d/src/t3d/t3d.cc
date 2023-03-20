#include "t3d/t3d.h"

#include "SDL.h"
#include "imgui/imgui_internal.h"

#include "log/log.h"

#include "core/stdutils.h"
#include "core/lines.h"
#include "core/colors.h"
#include "core/enum.h"
#include "core/texturetypes.h"
#include "core/viewport.h"
#include "core/plane.h"
#include "core/intersection.h"
#include "core/easing.h"

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
#include "window/imgui_extra.h"

#include "t3d/tilelibrary.h"
#include "t3d/editor.h"
#include "t3d/tool_notool.h"
#include "t3d/tool_placemeshonplane.h"
#include "t3d/keyboardstate.h"


namespace euphoria::t3d
{
    std::optional<core::vec3f>
    EditorCamera3::raycast
    (
        const core::UnitRay3f& ray
    )
    {
        ASSERT(parent != nullptr);
        
        const auto closest_point_on_mesh = parent->editor->raycast_closest_point(ray);
        if(closest_point_on_mesh) { return closest_point_on_mesh; }

        const auto ground = core::Plane::from_normal_and_point
        (
            core::unit3f::y_axis(), core::vec3f::zero()
        );

        const auto where = core::get_intersection(ray, ground);
        if(where > 0)
        {
            return ray.get_point(where);
        }
        
        return std::nullopt;
    }


    Application::Application()
    {
        editor_camera.parent = this;
        EUPH_INIT_LOGGING();

        editor_camera.fps.position.y = 5.0f;
        editor_camera.fps.position.z = 10.0f;
        for(int i=0; i<core::EditorCamera3::max_stored_index; i+=1)
        {
            editor_camera.save_camera(i);
        }

        pending_files.extensions = std::vector<std::string>
        {
            "obj",
            "fbx"
        };
    }


    Application::~Application() = default;


    [[nodiscard]] int
    Application::start(const core::argparse::NameAndArguments& args)
    {
        engine = std::make_shared<window::Engine>();

        if(const auto r = engine->setup(args); r != 0)
        {
            return r;
        }

        int width = 1280;
        int height = 720;

        if(engine->create_window("t3d", width, height) == false)
        {
            return -2;
        }

        material_shader_cache = std::make_shared<render::MaterialShaderCache>(engine->file_system.get());

        SET_ENUM_FROM_FILE
        (
            engine->file_system.get(),
            core::vfs::FilePath{"~/texture_types.json"},
            core::texture_type
        );

        texture_cache = std::make_shared<render::TextureCache>(engine->file_system.get());

        tile_library = std::make_shared<t3d::TileLibrary>(engine->file_system.get());

        world = std::make_shared<render::World>();

        editor = std::make_shared<t3d::Editor>(&grid_data, world.get(), tile_library.get());
        editor->tools.push_tool(std::make_shared<ToolNoTool>(this));

        timer = std::make_shared<window::SdlTimer>();

        update_grid();

        euphoria::window::enable_char_event(true);

        viewport_handler = std::make_shared<render::ViewportHandler>
        (
            engine->init.get(),
            nullptr
        );
        viewport_handler->set_size(width, height);

        return 0;
    }


    void
    Application::add_library(const core::vfs::DirPath& path)
    {
        pending_files.add_directory(path, engine->file_system.get());
    }


    void
    add_single_grid_line
    (
        core::Lines* def,
        float size,
        float x,
        const core::rgb& color
    )
    {
        def->add_line(core::vec3f {x, 0, -size}, core::vec3f {x, 0, size}, color);
        def->add_line(core::vec3f {-size, 0, x}, core::vec3f {size, 0, x}, color);
        def->add_line(core::vec3f {-x, 0, -size}, core::vec3f {-x, 0, size}, color);
        def->add_line(core::vec3f {-size, 0, -x}, core::vec3f {size, 0, -x}, color);
    }


    void
    Application::update_grid()
    {
        constexpr auto smallest_step = 0.01f;
        constexpr auto small_color = core::NamedColor::gray;
        constexpr auto big_color = core::NamedColor::black;
        constexpr auto x_color = core::NamedColor::pure_blue;
        constexpr auto z_color = core::NamedColor::pure_red;
        constexpr auto y_color = core::NamedColor::pure_yellow;

        if(grid)
        {
            grid->remove_this = true;
            grid = nullptr;
        }

        if(grid_data.visible == false)
        {
            return;
        }

        const auto small_step = core::max(smallest_step, grid_data.small_step);
        const auto size = core::abs(core::c_int_to_float(grid_data.size) * grid_data.small_step);
        const auto normal = grid_data.normal;

        auto def = core::Lines{};

        if(normal > 0)
        {
            def.add_line(core::vec3f {0, 0, 0}, core::vec3f {0, normal, 0}, y_color);
        }

        for(int index = 0; index < grid_data.size; index += 1)
        {
            float x = small_step * core::c_int_to_float(index+1);

            auto color = big_color;
            if(grid_data.big_step_interval > 1)
            {
                if(((index+1) % grid_data.big_step_interval) != 0 )
                {
                    color = small_color;
                }
            }
            add_single_grid_line(&def, size, x, color);
        }

        def.add_line(core::vec3f{-size, 0, 0}, core::vec3f{size, 0, 0}, x_color);
        def.add_line(core::vec3f{0, 0, -size}, core::vec3f{0, 0, size}, z_color);

        auto compiled = compile(material_shader_cache.get(), def);
        grid = std::make_shared<render::PositionedLines>(compiled);
        world->add_actor(grid);
    }


    KeyboardState
    Application::calculate_keyboard_state() const
    {
        return
        {
            state_ctrl_left  || state_ctrl_right,
            state_shift_left || state_shift_right,
            state_alt_left   || state_alt_right,
        };
    }


    void
    Application::on_sdl_event(const SDL_Event& e)
    {
        int window_width = 0;
        int window_height = 0;
        if(engine->on_resize(e, &window_width, &window_height))
        {
            viewport_handler->set_size(window_width, window_height);
        }
        
        window::imgui::send_events_to_imgui(&e);

        auto& io = ImGui::GetIO();
        const auto forward_keyboard = io.WantCaptureKeyboard == false;
        const auto forward_mouse = io.WantCaptureMouse == false;

        switch(e.type)
        {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_MOUSEMOTION:
            {
                const auto mouse_position = core::vec2i
                (
                    e.motion.x,
                    viewport_handler->window_height - e.motion.y
                );
                const auto mouse_movement = core::vec2i
                (
                    e.motion.xrel,
                    e.motion.yrel
                );
                on_mouse_movement(mouse_position, mouse_movement, forward_mouse);
            }
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            {
                const auto key = window::to_key(e.key.keysym);
                const bool down = e.type == SDL_KEYDOWN;
                on_key(key, down, forward_keyboard);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                const auto button = window::to_key(e.button);
                const bool down = e.type == SDL_MOUSEBUTTONDOWN;
                on_mouse_button(button, down, forward_mouse);
            }
            break;

        case SDL_MOUSEWHEEL:
            on_mouse_wheel(e, forward_mouse);
            break;

        default:
            // ignore other events
            break;
        }
    }


    void
    Application::focus_selected()
    {
        const auto sphere = editor->calculate_selected_bounding_sphere();
        if(sphere)
        {
            editor_camera.focus(*sphere, camera);
        }
    }


    void
    Application::on_mouse_movement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->mouse = position;

            if(mmb_down)
            {
                editor_camera.on_mouse_move(movement.x, movement.y);
            }
        }
    }


    void
    Application::on_key(core::Key key, bool down, bool forward_keyboard)
    {
        if(forward_keyboard)
        {
            // update keyvoard state
            switch (key)
            {
            case core::Key::shift_left: state_shift_left = down; break;
            case core::Key::shift_right: state_shift_right = down; break;
            case core::Key::ctrl_left: state_ctrl_left = down; break;
            case core::Key::ctrl_right: state_ctrl_right = down; break;
            case core::Key::alt_left: state_alt_left = down; break;
            case core::Key::alt_right: state_alt_right = down; break;
            default: break;
            }

            switch(key)
            {
            case core::Key::escape:
                if(down)
                {
                    running = false;
                }
                break;

            case core::Key::tab:
                if(!down)
                {
                    immersive_mode = !immersive_mode;
                }
                break;

            case core::Key::f:
                focus_selected();
                break;

            case core::Key::num_0: case core::Key::num_1:
            case core::Key::num_2: case core::Key::num_3:
            case core::Key::num_4: case core::Key::num_5:
            case core::Key::num_6: case core::Key::num_7:
            case core::Key::num_8: case core::Key::num_9:
            {
                const auto index = core::base_cast(key) - core::base_cast(core::Key::num_0);
                if(calculate_keyboard_state().ctrl)
                {
                    editor_camera.save_camera(index);
                }
                else
                {
                    editor_camera.load_camera(index);
                }
            }
            break;
                
            default:
                editor->on_key(key, down);
                break;
            }
        }

        if(mmb_down)
        {
            editor_camera.on_key(key, down);
        }

        switch(key)
        {
            case core::Key::shift_left:
            case core::Key::shift_right:
                shift_down = down;
                break;
                
            default:
                break;
        }
    }


    void
    Application::on_mouse_button(core::MouseButton button, bool down, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->on_mouse
            (
                button,
                calculate_keyboard_state(),
                down
            );
        }

        switch(button)
        {
        case core::MouseButton::middle:
            mmb_down = down;
            if(mmb_down)
            {
                editor_camera.on_camera_start();
            }
            else
            {
                editor_camera.on_camera_stop();
            }
            break;
        default:
            break;
        }

        update_mouse();
    }


    void
    Application::update_mouse()
    {
        const auto suggested_mouse = editor_camera.get_mouse();
        if(current_mouse && *current_mouse == suggested_mouse)
        {
            return;
        }

        current_mouse = suggested_mouse;
        engine->window->set_mouse_behaviour(suggested_mouse);
    }


    void
    Application::on_mouse_wheel(const SDL_Event& e, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->on_scroll(core::vec2i(e.wheel.x, e.wheel.y));
            editor_camera.on_scroll(e.wheel.x, e.wheel.y);
        }
    }


    void
    Application::handle_all_sdl_events()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            on_sdl_event(e);
        }
    }

    void Application::guizmo
    (
        bool is_translate, bool global_space,
        bool translate_x, bool translate_y, bool translate_z,
        bool rotate_x, bool rotate_y, bool rotate_z
    )
    {
        if (show_gizmo == false)
        {
            return;
        }

        if (compiled_camera.has_value())
        {
            editor->run_tools
            (
                is_translate, global_space, *compiled_camera,
                translate_x, translate_y, translate_z,
                rotate_x, rotate_y, rotate_z
            );
        }
    }

    void
    Application::process_imgui(bool display_full)
    {
        if(ImGui::BeginMainMenuBar())
        {
            on_main_menu();
        }
        ImGui::EndMainMenuBar();

        if(grid_window)
        {
            window::imgui::begin_fixed_overlay(window::Corner::top_left, "grid overlay", 10.0f, 30.0f);
            on_grid_window();
            ImGui::End();
        }

        if(camera_window)
        {
            window::imgui::begin_fixed_overlay(window::Corner::top_right, "camera overlay", 10.0f, 30.0f);

            constexpr auto popup_camera = "popup_camera";
            if(ImGui::Button(ICON_MD_VIDEOCAM))
            {
                ImGui::OpenPopup(popup_camera);
            }

            if(ImGui::BeginPopup(popup_camera))
            {
                on_camera_window();
                ImGui::EndPopup();
            }

            ImGui::SameLine();

            constexpr auto camera_orbit = ICON_MD_3D_ROTATION;
            constexpr auto camera_fps = ICON_MD_VIDEOCAM;

            if(ImGui::Button( editor_camera.is_camera_orbit() ? camera_orbit : camera_fps))
            {
                editor_camera.toggle_camera_orbit();
            }

            ImGui::End();
        }

        if(pending_files.has_more_files())
        {
            window::imgui::begin_fixed_overlay(window::Corner::bottom_left, "pending files");
            const auto frac = core::c_sizet_to_float(pending_files.index) / core::c_sizet_to_float(pending_files.files.size());
            window::imgui::label("Loading tiles...");
            ImGui::ProgressBar(frac, ImVec2{120, 0});
            ImGui::End();
        }

        if(display_full)
        {
            if(environment_window)
            {
                ImGui::Begin("Environment", &environment_window);
                on_environment_window();
                ImGui::End();
            }

            if(tiles_window)
            {
                ImGui::Begin("Tiles", &tiles_window);
                on_tile_window();
                ImGui::End();
            }

            if(lister_window)
            {
                ImGui::Begin("Lister", &lister_window);
                on_lister_window();
                ImGui::End();
            }

            if(preference_window)
            {
                ImGui::Begin("Preferences", &preference_window);
                on_preference_window();
                ImGui::End();
            }
        }
    }


    void
    Application::on_main_menu()
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
            ImGui::MenuItem("Environment", nullptr, &environment_window);
            ImGui::MenuItem("Camera", nullptr, &camera_window);
            ImGui::MenuItem("Tiles", nullptr, &tiles_window);
            ImGui::MenuItem("Grid", nullptr, &grid_window);
            ImGui::MenuItem("Lister", nullptr, &lister_window);
            ImGui::MenuItem("Preferences", nullptr, &preference_window);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Camera"))
        {
            if(ImGui::BeginMenu("Save"))
            {
                for(int i=0; i<core::EditorCamera3::max_stored_index; i+=1)
                {
                    const auto label = "Save {}"_format(i);
                    const auto shortcut = "Ctrl+{}"_format(i);
                    if(ImGui::MenuItem(label.c_str(), shortcut.c_str()))
                    {
                        editor_camera.save_camera(i);
                    }
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Load"))
            {
                for(int i=0; i<core::EditorCamera3::max_stored_index; i+=1)
                {
                    const auto label = "Load "_format(i);
                    const auto shortcut =  "{}"_format(i);
                    if(ImGui::MenuItem(label.c_str(), shortcut.c_str()))
                    {
                        editor_camera.load_camera(i);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }


    void
    Application::on_lister_window()
    {
        auto placed_meshes = editor->placed_meshes;
        if(placed_meshes.empty())
        {
            return;
        }

        ImGui::BeginListBox("Items");
        for(auto mesh: placed_meshes)
        {
            ASSERT(mesh->tile);
            ASSERT(mesh->tile);
            const auto p = mesh->actor->position;
            std::string display = "{} {}"_format(mesh->tile->name, p);
            if (ImGui::Selectable(display.c_str(), mesh->is_selected))
            {
                if (calculate_keyboard_state().ctrl == false)
                {
                    editor->set_all_selected(false);
                }
                mesh->is_selected = !mesh->is_selected;
            }
        }
        ImGui::EndListBox();

        if(ImGui::Button("Select none"))
        {
            editor->set_all_selected(false);
        }
        ImGui::SameLine();
        if(ImGui::Button("Select all"))
        {
            editor->set_all_selected(true);
        }
    }


    void
    Application::on_environment_window()
    {
        ImGui::Combo
        (
            "Type",
            reinterpret_cast<int*>(&world->light.light_type),
            "Directional\0Point\0Spot\0\0"
        );
        window::imgui::imgui_color_edit("Ambient", &world->light.ambient);
        window::imgui::imgui_color_edit("Diffuse", &world->light.diffuse);
        window::imgui::imgui_color_edit("Specular", &world->light.specular);
    }


    void
    Application::on_grid_window()
    {
        constexpr auto uistep = 0.01f;
        constexpr auto uimin = 0.0f;
        constexpr auto uimax = 100.0f;
        bool dirty = false;


        const std::string str = "{}"_format(grid_data.small_step);
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

            dirty = window::imgui::imgui_angle_slider
            (
                "angle snap",
                &grid_data.angle_snap,
                core::no_rotation,
                core::one_turn
            ) || dirty;

            dirty = ImGui::DragInt("lines on grid", &grid_data.size) || dirty;

            ImGui::EndPopup();
        }

        if
        (
            ImGui::Button
            (
                grid_data.visible
                    ? ICON_MD_GRID_ON
                    : ICON_MD_GRID_OFF
            )
        )
        {
            grid_data.visible = !grid_data.visible;
            dirty = true;
        }
        show_help("Toggle the grid visibility.");

        if
        (
            ImGui::Button
            (
                grid_data.snap_enabled
                    ? ICON_MD_NEAR_ME
                    : ICON_MD_NEAR_ME_DISABLED
            )
        )
        {
            grid_data.snap_enabled = !grid_data.snap_enabled;
            dirty = true;
        }
        show_help("Toggle the grid snapping");

        if(dirty)
        {
            update_grid();
        }
    }


    void
    Application::on_camera_window()
    {
        window::imgui::imgui_angle_slider
        (
            "Rotation (left/right)",
            &editor_camera.fps.rotation_angle
        );
        window::imgui::imgui_angle_slider
        (
            "Look (up/down)",
            &editor_camera.fps.look_angle,
            -core::quarter_turn,
            core::quarter_turn
        );
        ImGui::DragFloat3("Position", editor_camera.fps.position.get_data_ptr());
        ImGui::Spacing();
        auto sens = [](const char* label, core::Sensitivity* s)
        {
            ImGui::PushID(label);
            ImGui::Checkbox("##inverted", &s->inverted);
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::DragFloat(label, &s->multiplier, 0.1f, 0.0f);
        };
        sens("Look Sensitivity", &editor_camera.fps.look_sensitivity);
        ImGui::DragFloat("Move speed", &editor_camera.fps.move_speed, 0.1f, 0.0f);

        ImGui::DragFloat("Zoom percent", &editor_camera.zoom_percent, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Max zoom change", &editor_camera.max_zoom_change);

        ImGui::Checkbox("Animate camera", &editor_camera.animate_camera);
        ImGui::DragFloat("Camera lerp time", &editor_camera.camera_lerp_time, 0.001f, 0.0f, 1.0f);

        window::imgui::combo("Lerp function", &editor_camera.lerp_function, core::easing::get_all_values());

        ImGui::Checkbox("Scroll in orbit", &editor_camera.scroll_in_orbit);
    }


    void
    Application::on_tile_window()
    {
        // if(!tile_library->tiles.empty())
        if(ImGui::Button(ICON_MD_ADD) && !editor->is_busy())
        {
            if(editor->selected_tile == nullptr)
            {
                editor->selected_tile = tile_library->get_first_tile();
            }

            // todo(Gustav): add message if tile library is empty...

            if(editor->selected_tile)
            {
                editor->set_all_selected(false);
                auto placed = std::make_shared<PlacedMesh>();
                placed->tile = editor->selected_tile;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::Actor>
                (
                        placed->tile->mesh
                );
                world->add_actor(placed->actor);
                editor->placed_meshes.emplace_back(placed);

                editor->tools.push_tool
                (
                    std::make_shared<ToolPlaceMeshOnAPlane>(placed)
                );
            }
        }
        show_help("Place a new tile in the world");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MD_CONTENT_COPY))
        {
            auto selected = editor->get_first_selected_or_null();
            if(selected != nullptr && !editor->is_busy())
            {
                editor->set_all_selected(false);
                auto placed = std::make_shared<PlacedMesh>();
                placed->tile = selected->tile;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::Actor>
                (
                    placed->tile->mesh
                );
                world->add_actor(placed->actor);
                editor->placed_meshes.emplace_back(placed);

                editor->tools.push_tool
                (
                    std::make_shared<ToolPlaceMeshOnAPlane>(placed)
                );
            }
        }
        show_help("Duplicate the selected tile.");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MD_PIN_DROP))
        {
            auto placed = editor->get_first_selected_or_null();
            if(placed != nullptr && !editor->is_busy())
            {
                editor->selected_tile = placed->tile;
                editor->tools.push_tool
                (
                    std::make_shared<ToolPlaceMeshOnAPlane>(placed)
                );
            }
        }
        show_help("Place or replace selected tile on a new place.");

        editor->on_editor();
    }


    void
    Application::on_preference_window()
    {
        constexpr const char* const help_styles_separated_by_zeros =
            "No help\0"
            "Help text\0"
            "Help marker\0"
            "Help text after time\0"
            ;
        ImGui::Combo("Help style", &help_style, help_styles_separated_by_zeros);
    }


    void
    Application::render()
    {
        auto viewport = viewport_handler->get_full_viewport();
        editor->camera = camera.compile(viewport.get_aspect_ratio());
        editor->viewport = viewport;
        compiled_camera = world->render(viewport, camera);
    }


    void
    Application::on_frame()
    {
        const float delta = timer->update();

        // todo(Gustav): improve when to display hover text...
        help_hover.step(ImGui::GetHoveredID() != 0, delta);
        

        if(compiled_camera.has_value())
        {
            editor_camera.step
            (
                shift_down,
                editor->mouse,
                *compiled_camera,
                editor->viewport,
                delta
            );
        }

        world->step();

        editor->tools.perform_tools();
        
        handle_all_sdl_events();

        editor->step();

        if(pending_files.has_more_files())
        {
            const auto file = pending_files.get_next_file();
            tile_library->add_file(file, material_shader_cache.get(), texture_cache.get());
        }

        window::imgui::begin_new_frame();
        process_imgui(!immersive_mode);

        camera.position = editor_camera.fps.position;
        camera.rotation = editor_camera.fps.get_rotation();

        engine->init->clear_screen(core::NamedColor::light_gray);

        render();

        window::imgui::imgui_render();

        SDL_GL_SwapWindow(engine->window->window);
    }

    void
    Application::show_help(const char* desc) const
    {
        switch (help_style)
        {
        case 0:
            return;
        case 1:
            window::imgui::add_help_text_for_previous_widget(desc);
            break;
        case 2:
            window::imgui::add_help_marker_for_previous_widget(desc);
            break;
        case 3:
            if(help_hover.display_hover)
            {
                window::imgui::add_help_text_for_previous_widget(desc);
            }
            break;
        default:
            DIE("Invalid style");
            break;
        }
    }
}
