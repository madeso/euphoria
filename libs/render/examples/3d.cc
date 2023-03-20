#include "core/mat4.h"
#include "core/image_draw.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/vfs.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_path.h"
#include "core/os.h"
#include "core/range.h"
#include "core/camera3.h"
#include "core/palette.h"
#include "core/fpscontroller.h"
#include "core/viewport.h"

#include "render/init.h"
#include "render/debuggl.h"
#include "render/materialshader.h"
#include "render/compiledmesh.h"
#include "render/texturecache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/materialshadercache.h"
#include "render/defaultfiles.h"
#include "render/actor.h"
#include "render/viewporthandler.h"

#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_extra.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"
#include "window/key.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>

using namespace euphoria;
using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;

struct CubeAnimation
{
    CubeAnimation()
        : from(quatf::identity())
        , to(quatf::identity())
    {
    }

    std::shared_ptr<euphoria::render::Actor> actor;
    float timer = 0.0f;
    quatf from;
    quatf to;
    float rotation_speed = 1.0f;
    float move_speed = 1.0f;
};

// 3d demo

int
main(int argc, char** argv)
{
    Engine engine;
    if(const auto r = engine.setup(argparse::NameAndArguments::extract(argc, argv)); r != 0)
    {
        return r;
    }

    constexpr int width = 1280;
    constexpr int height = 720;

    if(!engine.create_window("Euphoria 3d demo", width, height, false))
    {
        return -1;
    }

    MaterialShaderCache material_shader_cache {engine.file_system.get()};

    SET_ENUM_FROM_FILE
    (
        engine.file_system.get(),
        vfs::FilePath{"~/texture_types.json"},
        texture_type
    );

    Image image;
    image.setup_no_alpha_support(256, 256);
    const auto wi = on_whole_image(image);
    clear(&image, {NamedColor::red});
    auto rand = core::Random{42};

    for(int i = 0; i < 20; i += 1)
    {
        const rgb color = crgb(palettes::dawnbringer().get_random_color(&rand));
        const auto pos = get_random_point(&rand, wi);
        const auto outer = get_random_in_range(&rand, 55.0f, 100.0f);
        const auto inner = get_random_in_range(&rand, make_range(50.0f));
        draw_circle(&image, color, pos, outer, 10, inner);
    }
    draw_line_antialiased(&image, NamedColor::black, wi.get_top_left(), wi.get_bottom_right());
    draw_rect
    (
        &image,
        {NamedColor::blue},
        Recti::from_top_left_width_height(vec2i{0, 256}, 100, 25)
    );
    draw_line_antialiased(&image, NamedColor::black, wi.get_bottom_left(), wi.get_top_right());
    // todo(Gustav): fix text drawing...
    // DrawText(&image, vec2i(0, 0), "Hello world", Color::Black, 2);
    engine.catalog->register_file_data
    (
        vfs::FilePath{"~/image"},
        image.write(ImageWriteFormat::png)
    );

    TextureCache texture_cache {engine.file_system.get()};

    bool running = true;

    SdlTimer timer;

    auto world = euphoria::render::World {};

    auto box_mesh1 = meshes::create_cube(0.5f);
    box_mesh1.materials[0].set_texture("Diffuse", vfs::FilePath{"./container2.png"});
    box_mesh1.materials[0].set_texture("Specular", vfs::FilePath{"./container2_specular.png"});
    box_mesh1.materials[0].ambient = NamedColor::white; // fix ambient color on material
    box_mesh1.materials[0].specular = NamedColor::white;
    box_mesh1.materials[0].shininess = 120.0f;
    auto box1 = compile_mesh
    (
        box_mesh1,
        &material_shader_cache,
        &texture_cache,
        vfs::DirPath::from_root(),
        "box1"
    );

    auto box_mesh2 = meshes::create_sphere(0.5f, "image");
    box_mesh2.materials[0].set_texture("Specular", vfs::FilePath{"./img-plain/white"});
    box_mesh2.materials[0].ambient = NamedColor::white; // fix ambient color on material
    box_mesh2.materials[0].specular = NamedColor::white;
    box_mesh2.materials[0].shininess = 10.0f;
    auto box2 = compile_mesh
    (
        box_mesh2,
        &material_shader_cache,
        &texture_cache,
        vfs::DirPath::from_root(),
        "box2"
    );

    auto debug_texture = texture_cache.get_texture(vfs::FilePath{"~/image"});

    auto light_mesh = meshes::create_cube(0.2f);
    light_mesh.materials[0].shader = vfs::FilePath{"~/basic_shader"};
    auto light = compile_mesh
    (
        light_mesh,
        &material_shader_cache,
        &texture_cache,
        vfs::DirPath::from_root(),
        "light"
    );
    float light_position = 0.0f;

    const float box_extent_value = 4;
    auto box_extents = Aabb
    {
        vec3f{-box_extent_value, -box_extent_value, -box_extent_value},
        vec3f{box_extent_value, box_extent_value, box_extent_value}
    };

    std::vector<CubeAnimation> animation_handler;

    bool capturing_mouse_movement = false;

    // 0=do nothing, 1=use light actor, 2=follow camera
    int light_update = 1;

    for(int i = 0; i < 20; ++i)
    {
        auto actor = std::make_shared<render::Actor>(rand.get_next_bool() ? box1 : box2);
        world.add_actor(actor);

        CubeAnimation anim;
        anim.actor = actor;
        anim.from = quatf::from_random(&rand);
        anim.to = quatf::from_random(&rand);
        anim.rotation_speed = get_random_in_range(&rand, 0.5f, 1.0f);
        anim.move_speed = get_random_in_range(&rand, 0.5f, 1.0f);
        anim.timer = rand.get_next_float01();


        // generate a position not too close to the center
        vec3f position = vec3f::zero();
        do
        {
            position = get_random_point(&rand, box_extents);
        } while(position.get_length() < 1.4f);

        actor->position = position;
        actor->rotation = anim.from;

        animation_handler.push_back(anim);
    }

    auto light_actor = std::make_shared<Actor>(light);
    world.add_actor(light_actor);
    light_actor->overriden_materials = light_actor->create_override();
    ASSERT(light_actor->overriden_materials->materials.size() == 1);
    auto& light_material = light_actor->overriden_materials->materials[0];

#if 0
  auto loaded_dude = meshes::LoadMesh(&file_system, "nanosuit/nanosuit.obj");
  if(!loaded_dude.error.empty())
  {
    std::cerr << "Failed to load dude: " << loaded_dude.error << "\n";
    return -5;
  }
  auto dude_mesh =
      CompileMesh(loaded_dude.mesh, &material_shader_cache, &texture_cache,
                  vfs::Path::FromDirectory("nanosuit"), "dude");
  auto dude_actor = std::make_shared<Actor>(dude_mesh);
  world.AddActor(dude_actor);
#endif

    Camera3 camera;
    camera.position = vec3f::zero();

    FpsController fps;
    fps.position = vec3f(0, 0, 3);

    auto viewport_handler = euphoria::render::ViewportHandler
    {
        engine.init.get(),
        nullptr
    };
    viewport_handler.set_size(width, height);

    bool paused = true;

    auto run_imgui = [&]()
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);

        ImGui::SetNextWindowSize
        (
            ImVec2(200, 100),
            ImGuiCond_FirstUseEver
        );
        ImGui::Begin("Light");
        imgui::combo
        (
            "Type",
            &world.light.light_type,
            {
                {"Directional", Light::Type::directional},
                {"Point", Light::Type::point},
                {"Spot", Light::Type::spot}
            }
        );
        imgui::imgui_color_edit("Ambient", &world.light.ambient);
        imgui::imgui_color_edit("Diffuse", &world.light.diffuse);
        imgui::imgui_color_edit("Specular", &world.light.specular);
        ImGui::Combo
        (
            "Update",
            &light_update,
            "Do nothing\0Follow actor\0Follow camera\0\0"
        );

        imgui::imgui_angle_slider
        (
            "Cutoff angle Inner",
            &world.light.cutoff_angle_inner,
            no_rotation,
            quarter_turn / 2
        );
        imgui::imgui_angle_slider
        (
            "Cutoff angle Outer",
            &world.light.cutoff_angle_outer,
            no_rotation,
            quarter_turn
        );

        imgui::image(debug_texture.get());

        ImGui::End();

        light_material.ambient = world.light.ambient;
        light_material.diffuse = world.light.diffuse;
        light_material.specular = world.light.specular;
        light_material.shininess = 10;
    };

    auto run_update = [&](float delta)
    {
        light_position = wrap
        (
            make_range<float>(0, 1),
            light_position + delta * 0.1f
        );
        const auto light_pc = PolarCoordinate{light_position, light_position * 2};
        const auto light_pos = light_pc.to_unit_vector() * 2.0f;
        light_actor->position = light_pos;

        switch(light_update)
        {
        case 1:
            world.light.position = light_pos;
            break;
        case 2:
            world.light.position = fps.position;
            world.light.direction = fps.get_rotation().in().get_normalized();
            break;
        }


        for(auto& anim: animation_handler)
        {
            if(paused == false)
            {
                anim.timer += delta * anim.rotation_speed;
                int count = 0;
                while(anim.timer > 1.0f)
                {
                    count += 1;
                    anim.timer -= 1.0f;
                    anim.from = anim.to;
                    anim.to = quatf::from_random(&rand);
                    anim.rotation_speed = get_random_in_range(&rand, 0.3f, 1.0f);
                    anim.move_speed = get_random_in_range(&rand, 0.2f, 3.0f);
                }
                ASSERT(count < 2);
                quatf q = slerp_shortway(anim.from, anim.timer, anim.to);
                anim.actor->rotation = q;
                const auto movement = q.in() * anim.move_speed * delta;
                const auto new_pos = box_extents.wrap
                (
                    anim.actor->position + movement
                );
                anim.actor->position = new_pos; // hard to see movement when everything is moving
            }
        }

        fps.update(delta);
        camera.position = fps.position;
        camera.rotation = fps.get_rotation();
    };

    auto handle_events = [&](bool show_imgui)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(show_imgui)
            {
                imgui::send_events_to_imgui(&e);
            }
            {
                int window_width = 800;
                int window_height = 600;
                if(engine.on_resize(e, &window_width, &window_height))
                {
                    viewport_handler.set_size(window_width, window_height);
                }
            }
            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            case SDL_MOUSEMOTION:
                if(capturing_mouse_movement)
                {
                    fps.look
                    (
                        c_int_to_float(e.motion.xrel),
                        c_int_to_float(e.motion.yrel)
                    );
                }
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                const bool down = e.type == SDL_KEYDOWN;

                fps.on_key(to_key(e.key.keysym), down);

                switch(e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    if(down)
                    {
                        running = false;
                    }
                    break;
                case SDLK_p:
                    if(!down)
                    {
                        paused = !paused;
                    }
                    break;
                case SDLK_TAB:
                    if(!down)
                    {
                        capturing_mouse_movement = !capturing_mouse_movement;
                        engine.window->set_mouse_behaviour
                        (
                            capturing_mouse_movement
                                ? MouseBehaviour::normal
                                : MouseBehaviour::relative_only
                        );
                    }
                    break;
                default:
                    // ignore other keys
                    break;
                }
            }
            break;
            default:
                // ignore other events
                break;
            }
        }
    };

    auto render_demo = [&](bool show_imgui)
    {
        engine.init->clear_screen(NamedColor::black);
        // todo(Gustav): get_full_viewport or somthing different?
        // how does this handle the black bars...?
        world.render(viewport_handler.get_full_viewport(), camera);

        if(show_imgui)
        {
            imgui::imgui_render();
        }

        SDL_GL_SwapWindow(engine.window->window);
    };

    while(running)
    {
        const bool show_imgui = !capturing_mouse_movement;
        const float delta = timer.update();

        if(show_imgui)
        {
            imgui::begin_new_frame();
            run_imgui();
        }

        run_update(delta);
        render_demo(show_imgui);
        handle_events(show_imgui);
    }

    return 0;
}

#if 0
struct View {
Camera[] cameras;
};

struct CameraView {
Camera;
ViewPort area; // glViewport
};

struct Camera {
Mode mode;
World world;
culling info from world, created when camera is added to world
viewMatrix; // position + rotation
projectionMatrix; // ortho/perspective

// 4 planes from the projection matrix
getFrustum();

toViewSpace(); // camera space, eye space
toWorldSpace();
};

// normal, thermo optics/powerup
struct Mode {
Pass[] passes;
};

struct Pass {
tag/list
Shader shader;
};

struct Shader {
texture list: list of texture names mapped to shader variables
};

struct World {
Actor[] objects;
};

struct Actor {
Mesh mesh;
position;
rotation;
modelMatrix;
ToLocalSpace();
ToWorldSpace();
};

struct Mesh {
MeshPart[] meshParts;
};

struct MeshPart {
Material material;
};

struct MaterialList {
maps id, material name, shader variable
};

struct Material {
Texture[] textures;
};
#endif
