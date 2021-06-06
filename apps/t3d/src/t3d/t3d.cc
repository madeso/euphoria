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
#include "window/imgui_extra.h"

#include "t3d/tilelibrary.h"
#include "t3d/editor.h"
#include "t3d/tool_notool.h"
#include "t3d/tool_placemeshonplane.h"


namespace euphoria::t3d
{
    application::application()
    {
        pending_files.extensions = std::vector<std::string>
        {
            "obj",
            "fbx"
        };
    }


    application::~application()
    {
    }


    [[nodiscard]] int
    application::start(const core::argparse::name_and_arguments& args)
    {
        engine = std::make_shared<window::engine>();

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

        material_shader_cache = std::make_shared<render::material_shader_cache>(engine->file_system.get());

        SET_ENUM_FROM_FILE
        (
            engine->file_system.get(),
            core::vfs::file_path{"~/texture_types.json"},
            core::texture_type
        );

        texture_cache = std::make_shared<render::texture_cache>(engine->file_system.get());

        tile_library = std::make_shared<t3d::tile_library>(engine->file_system.get());

        world = std::make_shared<render::world>();

        editor = std::make_shared<t3d::editor>(&grid_data, world.get(), tile_library.get());
        editor->tools.push_tool(std::make_shared<tool_no_tool>());

        timer = std::make_shared<window::sdl_timer>();

        update_grid();

        engine->window->enable_char_event(!immersive_mode);

        viewport_handler = std::make_shared<render::viewport_handler>
        (
            engine->init.get(),
            nullptr
        );
        viewport_handler->set_size(width, height);

        return 0;
    }


    void
    application::add_library(const core::vfs::dir_path& path)
    {
        pending_files.add_directory(path, engine->file_system.get());
    }


    void
    application::add_single_grid_line
    (
        core::lines& def,
        float size,
        float x,
        const core::rgb& color
    )
    {
        def.add_line(core::vec3f {x, 0, -size}, core::vec3f {x, 0, size}, color);
        def.add_line(core::vec3f {-size, 0, x}, core::vec3f {size, 0, x}, color);
        def.add_line(core::vec3f {-x, 0, -size}, core::vec3f {-x, 0, size}, color);
        def.add_line(core::vec3f {-size, 0, -x}, core::vec3f {size, 0, -x}, color);
    }


    void
    application::update_grid()
    {
        constexpr auto smallest_step = 0.01f;
        constexpr auto small_color = core::color::gray;
        constexpr auto big_color = core::color::black;
        constexpr auto x_color = core::color::pure_blue;
        constexpr auto z_color = core::color::pure_red;
        constexpr auto y_color = core::color::pure_yellow;

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
        const auto size = core::abs(grid_data.size * grid_data.small_step);
        const auto normal = grid_data.normal;

        auto def = core::lines {};

        if(normal > 0)
        {
            def.add_line(core::vec3f {0, 0, 0}, core::vec3f {0, normal, 0}, y_color);
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
            add_single_grid_line(def, size, x, color);
        }

        def.add_line(core::vec3f {-size, 0, 0}, core::vec3f {size, 0, 0}, x_color);
        def.add_line(core::vec3f {0, 0, -size}, core::vec3f {0, 0, size}, z_color);

        auto compiled = compile(material_shader_cache.get(), def);
        grid = std::make_shared<render::positioned_lines>(compiled);
        world->add_actor(grid);
    }


    void
    application::on_sdl_event(const SDL_Event& e)
    {
        int window_width = 0;
        int window_height = 0;
        if(engine->on_resize(e, &window_width, &window_height))
        {
            viewport_handler->set_size(window_width, window_height);
        }
        if(show_imgui)
        {
            engine->imgui->process_events(&e);
        }

        auto& io = ImGui::GetIO();
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
                viewport_handler->window_height - e.motion.y
            );
            const auto mouse_movement = core::vec2i
            (
                e.motion.xrel,
                e.motion.yrel
            );
            on_mouse_movement(mouse_position, mouse_movement, forward_mouse);
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
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
    application::on_mouse_movement(const core::vec2i& position, const core::vec2i& movement, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->mouse = position;

            if(mmb_down)
            {
                const auto mm = movement.StaticCast<float>();
                if(shift_down) { orbit.on_pan_input(mm.x, mm.y); }
                else { orbit.on_rotate_input(mm.x, mm.y); }
            }
        }
    }


    void
    application::on_key(core::key key, bool down, bool forward_keyboard)
    {
        if(forward_keyboard)
        {
            switch(key)
            {
            case core::key::escape:
                if(down)
                {
                    running = false;
                }
                break;
            case core::key::tab:
                if(!down)
                {
                    immersive_mode = !immersive_mode;
                    engine->window->keep_within(immersive_mode);
                    engine->window->enable_char_event(!immersive_mode);
                }
                break;
            default:
                if(forward_keyboard)
                {
                    editor->on_key(key, down);
                }
                break;
            }
        }

        switch(key)
        {
            case core::key::shift_left:
            case core::key::shift_right:
                shift_down = down;
                break;
            default:
                break;
        }
    }


    void
    application::on_mouse_button(core::MouseButton button, bool down, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->on_mouse(button, down);
        }

        switch(button)
        {
        case core::MouseButton::middle:
            mmb_down = down;
            break;
        default:
            break;
        }
    }


    void
    application::on_mouse_wheel(const SDL_Event& e, bool forward_mouse)
    {
        if(forward_mouse)
        {
            editor->on_scroll(core::vec2i(e.wheel.x, e.wheel.y));
            orbit.on_zoom_input(e.wheel.y);
        }
    }


    void
    application::handle_all_sdl_events()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            on_sdl_event(e);
        }
    }


    void
    application::process_imgui()
    {
        if(ImGui::BeginMainMenuBar())
        {
            on_main_menu();
        }
        ImGui::EndMainMenuBar();

        if(environment_window)
        {
            ImGui::Begin("Environment", &environment_window);
            on_environment_window();
            ImGui::End();
        }

        if(grid_window)
        {
            window::imgui::begin_fixed_overlay(window::corner::top_left, "grid overlay", 10.0f, 30.0f);
            on_grid_window();
            ImGui::End();
        }

        if(pending_files.has_more_files())
        {
            window::imgui::begin_fixed_overlay(window::corner::bottom_left, "pending files");
            const auto frac = pending_files.index / static_cast<float>(pending_files.files.size());
            window::imgui::label("Loading tiles...");
            ImGui::ProgressBar(frac, ImVec2{120, 0});
            ImGui::End();
        }

        if(camera_window)
        {
            ImGui::Begin("Camera", &camera_window);
            on_camera_window();
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


    void
    application::on_main_menu()
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
    }


    void
    application::on_lister_window()
    {
        auto actors = editor->actors;
        if(actors.empty())
        {
            return;
        }

        ImGui::BeginListBox("Items");
        for(auto actor: actors)
        {
            ASSERT(actor->tile);
            ASSERT(actor->tile);
            const auto p = actor->actor->position;
            std::string display = core::string_builder {}
                << actor->tile->name
                << " "
                << p;
            if(ImGui::Selectable(display.c_str(), actor->is_selected))
            {
                editor->set_all_selected(false);
                actor->is_selected = !actor->is_selected;
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
    application::on_environment_window()
    {
        ImGui::Combo
        (
            "Type",
            reinterpret_cast<int*>(&world->light.light_type),
            "Directional\0Point\0Spot\0\0"
        );
        window::imgui::color_edit("Ambient", &world->light.ambient);
        window::imgui::color_edit("Diffuse", &world->light.diffuse);
        window::imgui::color_edit("Specular", &world->light.specular);
    }


    void
    application::on_grid_window()
    {
        constexpr auto uistep = 0.01f;
        constexpr auto uimin = 0.0f;
        constexpr auto uimax = 100.0f;
        bool dirty = false;


        const std::string str = core::string_builder{} << grid_data.small_step;
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

            dirty = ImGui::DragInt("lines on grid", &grid_data.size) || dirty;

            ImGui::EndPopup();
        }

        if
        (
            ImGui::Button
            (
                grid_data.visible
                    ? ICON_MDI_GRID
                    : ICON_MDI_GRID_OFF
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
                    ? ICON_MDI_AXIS_ARROW_LOCK
                    : ICON_MDI_AXIS_ARROW
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
    application::on_camera_window()
    {
        window::imgui::angle_slider
        (
            "Horizontal",
            &orbit.horizontal_rotation,
            core::angle::zero(),
            core::angle::one_turn()
        );
        window::imgui::angle_slider
        (
            "Vertical",
            &orbit.vertical_rotation,
            -core::angle::quarter(),
            core::angle::quarter()
        );
        ImGui::InputFloat3("Position", orbit.center.get_data_ptr());
        ImGui::Spacing();
        ImGui::InputFloat("Distance", &orbit.distance);
        auto sens = [](const char* label, core::sensitivity* s)
        {
            ImGui::PushID(label);
            ImGui::Checkbox("##inverted", &s->inverted);
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::DragFloat(label, &s->multiplier, 0.1f, 0.0f);
        };
        sens("Pan dX", &orbit.pan_dx);
        sens("Pan dY", &orbit.pan_dy);
        sens("Rotate dY", &orbit.rotate_dx);
        sens("Rotate dX", &orbit.rotate_dy);
        sens("Zoom", &orbit.zoom);
    }


    void
    application::on_tile_window()
    {
        // if(!tile_library->tiles.empty())
        if(ImGui::Button(ICON_MDI_PLUS) && !editor->is_busy())
        {
            if(editor->selected_mesh == nullptr)
            {
                editor->selected_mesh = tile_library->get_first_tile();
            }

            // todo(Gustav): add message if tile library is empty...

            if(editor->selected_mesh)
            {
                editor->set_all_selected(false);
                auto placed = std::make_shared<placed_mesh>();
                placed->tile = editor->selected_mesh;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::actor>
                (
                        placed->tile->mesh
                );
                world->add_actor(placed->actor);
                editor->actors.emplace_back(placed);

                editor->tools.push_tool
                (
                    std::make_shared<tool_place_mesh_on_a_plane>(placed)
                );
            }
        }
        show_help("Place a new tile in the world");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MDI_PLUS_BOX_MULTIPLE))
        {
            auto selected = editor->get_first_selected_or_null();
            if(selected != nullptr && !editor->is_busy())
            {
                editor->set_all_selected(false);
                auto placed = std::make_shared<placed_mesh>();
                placed->tile = selected->tile;
                placed->is_selected = true;
                placed->actor = std::make_shared<render::actor>
                (
                        placed->tile->mesh
                );
                world->add_actor(placed->actor);
                editor->actors.emplace_back(placed);

                editor->tools.push_tool
                (
                    std::make_shared<tool_place_mesh_on_a_plane>(placed)
                );
            }
        }
        show_help("Duplicate the selected tile.");

        ImGui::SameLine();

        if(ImGui::Button(ICON_MDI_ARROW_COLLAPSE_DOWN))
        {
            auto placed = editor->get_first_selected_or_null();
            if(placed != nullptr && !editor->is_busy())
            {
                editor->selected_mesh = placed->tile;
                editor->tools.push_tool
                (
                    std::make_shared<tool_place_mesh_on_a_plane>(placed)
                );
            }
        }
        show_help("Place or replace selected tile on a new place.");

        editor->on_editor();
    }


    void
    application::on_preference_window()
    {
        constexpr const char* const help_styles_separated_by_zeros =
            "No help\0"
            "Help text\0"
            "Help marker\0"
            ;
        ImGui::Combo("Help style", &help_style, help_styles_separated_by_zeros);
    }


    void
    application::render()
    {
        auto viewport = viewport_handler->get_full_viewport();
        editor->camera = camera.compile(viewport.get_aspect_ratio());
        editor->viewport = viewport;
        world->render(viewport, camera);
    }


    void
    application::on_frame()
    {
        show_imgui = !immersive_mode;
        // const float delta =
        timer->update();

        world->step();
        editor->tools.perform_tools();

        handle_all_sdl_events();

        editor->step();

        if(pending_files.has_more_files())
        {
            const auto file = pending_files.get_next_file();
            tile_library->add_file(file, material_shader_cache.get(), texture_cache.get());
        }

        if(show_imgui)
        {
            engine->imgui->start_new_frame();
            process_imgui();
        }

        camera.position = orbit.get_camera_position();
        camera.rotation = orbit.get_rotation();

        engine->init->clear_screen(core::color::light_gray);

        render();

        if(show_imgui)
        {
            engine->imgui->render();
        }

        SDL_GL_SwapWindow(engine->window->window);
    }

    void
    application::show_help(const char* desc)
    {
        switch (help_style)
        {
        case 0:
            return;
        case 1:
            window::imgui::help_text(desc);
            break;
        case 2:
            window::imgui::help_marker(desc);
            break;
        default:
            DIE("Invalid style");
            break;
        }
    }
}
