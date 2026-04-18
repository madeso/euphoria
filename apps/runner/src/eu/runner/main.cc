#include "SDL.h"

// #include <string>

#include "eu/log/log.h"
#include "eu/base/memorychunk.h"

#include "eu/core/geom.builder.h"
#include "eu/core/geom.h"

#include "eu/io/file.h"
#include "eu/io/mesh.h"

#include "eu/render/canvas.h"
#include "eu/render/state.h"
#include "eu/render/font.h"
#include "eu/render/opengl_utils.h"
#include "eu/render/texture.io.h"
#include "eu/render/camera.h"
#include "eu/render/enable_high_performance_graphics.h"
#include "eu/render/postproc.h"
#include "eu/render/renderer.h"
#include "eu/render/world.h"


ENABLE_HIGH_PERFORMANCE_GRAPHICS

// todo(Gustav): move to io
eu::render::ShaderSource load_shader(const std::string& name)
{
    return {
        .vertex = eu::io::string_from_file(name + ".vert.glsl"),
        .fragment = eu::io::string_from_file(name + ".frag.glsl")
    };
}

eu::Size size_from_v2(const eu::v2& v)
{
    return eu::Size{ .width = eu::int_from_float(v.x), .height = eu::int_from_float(v.y) };
}

std::shared_ptr<eu::render::Texture2d> load_texture(const std::string& path, eu::render::ColorData cd,
    eu::render::TextureEdge texture_edge = eu::render::TextureEdge::repeat,
    eu::render::Transparency transparency = eu::render::Transparency::exclude)
{
    const auto bin = eu::io::bytes_from_file(path);
    return std::make_shared<eu::render::Texture2d>(
        load_image_from_embedded(SEND_DEBUG_LABEL_MANY(path) embedded_binary{reinterpret_cast<const unsigned int*>(bin.data()), static_cast<unsigned int>(bin.size())}, texture_edge, eu::render::TextureRenderStyle::mipmap, transparency, cd)
    );
}

std::shared_ptr<eu::render::Texture2d> create_texture(DEBUG_LABEL_ARG_MANY eu::render::SingleColor pixel_color, eu::render::ColorData cd)
{
    return std::make_shared<eu::render::Texture2d>(
        load_image_from_color(SEND_DEBUG_LABEL_MANY(debug_label) pixel_color, eu::render::TextureEdge::repeat, eu::render::TextureRenderStyle::pixel, eu::render::Transparency::exclude, cd)
    );
}



int main(int, char**)
{
    int window_width = 1280;
    int window_height = 720;
    const char* glsl_version = "#version 130";

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        LOG_ERR("Error initializing SDL: {}", SDL_GetError());
        return -1;
    }

#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);	// Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    // const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);  // was 0 in dear imgui example??
#endif

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (!window) {
        LOG_ERR("Error creating window: {}", SDL_GetError());
        return -1;
    }
    
    auto* glContext = SDL_GL_CreateContext(window);
    SDL_GetWindowSize(window, &window_width, &window_height);
    if (!glContext) {
        LOG_ERR("Error creating gl context: {}", SDL_GetError());
        return -1;
    }

    /* OpenGL setup */
    const int glad_result = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (glad_result == 0)
    {
        LOG_ERR("Failed to init glad, error: {0}", glad_result);
        return -1;
    }

    {
        const std::string gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const std::string gl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const std::string gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const std::string gl_shading_language_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        LOG_INFO("Vendor:         {0}", gl_vendor);
        LOG_INFO("Renderer:       {0}", gl_renderer);
        LOG_INFO("Version OpenGL: {0}", gl_version);
        LOG_INFO("Version GLSL:   {0}", gl_shading_language_version);
    }

    eu::render::State states;
    eu::render::Render2 render{ &states };

    bool running = true;

    eu::render::Assets assets;

    assets.black = create_texture(SEND_DEBUG_LABEL_MANY("black") eu::render::color_from_rgba(0, 0, 0, 255), eu::render::ColorData::dont_care);
    assets.white = create_texture(SEND_DEBUG_LABEL_MANY("white") eu::render::color_from_rgba(255, 255, 255, 255), eu::render::ColorData::dont_care);

    assets.default_shader_source = load_shader("default_shader");
    assets.skybox_shader_source = load_shader("skybox");

    assets.pp_vert_glsl = eu::io::string_from_file("pp_vert.glsl");
    assets.pp_realize_frag_glsl = eu::io::string_from_file("pp_realize.frag.glsl");
    assets.pp_extract_frag_glsl = eu::io::string_from_file("pp_extract.frag.glsl");
    assets.pp_ping_pong_blur_frag_glsl = eu::io::string_from_file("pp_ping_pong_blur.frag.glsl");

    eu::render::Renderer renderer{&states, &assets, eu::render::RenderSettings{} };
    if (renderer.is_loaded() == false)
    {
        return -1;
    }

    eu::render::World world;
    eu::render::EffectStack effects;

    // add demo world
    {
        constexpr auto PLANE_SIZE = 100.0f;
        auto plane_geom = eu::render::compile_geom(
            USE_DEBUG_LABEL_MANY("plane")
            eu::core::geom::create_xz_plane(PLANE_SIZE, PLANE_SIZE, false).to_geom(),
            renderer.default_geom_layout()
        );

        auto material = renderer.make_default_material();
        material->diffuse = load_texture("sprites/smoke.png", eu::render::ColorData::dont_care);
        material->specular = assets.white;

        auto plane = make_mesh_instance(plane_geom, material);
        world.meshes.emplace_back(plane);
        plane->world_position = { 0.0f, -3.0f, 0.0f };
    }

    // add demo mesh
    {
        const auto geom = eu::io::geom_from_file("models/vehicle-truck-purple.glb");
        // const auto geom = eu::core::geom::create_box(1.0f, 1.0f, 1.0f, eu::core::geom::NormalsFacing::Out).to_geom();
        auto compiled_geom = eu::render::compile_geom(
            USE_DEBUG_LABEL_MANY("truck")
            geom,
            renderer.default_geom_layout()
        );

        auto material = renderer.make_default_material();
        material->diffuse = load_texture("models/textures/colormap.png", eu::render::ColorData::color_data);
        material->specular = assets.white;

        auto instance = make_mesh_instance(compiled_geom, material);
        world.meshes.emplace_back(instance);
        instance->world_position = { 0.0f, 0.0f, -10.0f };
    }
    
    LOG_INFO("Runner started");
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    LOG_INFO("Resized");
                    SDL_GetWindowSize(window, &window_width, &window_height);
                }
                break;
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }

        eu::render::RenderCommand cmd{ .states = &states, .render = &render, .size = {.width = window_width, .height = window_height} };

        // clear
        {
            {
                const auto screen = eu::render::LayoutData{ .style = eu::render::ViewportStyle::extended,
                                        .requested_width = 800, .requested_height = 600 };
                cmd.clear(eu::colors::black, screen);
            }
        }

        // render 3d world
        {
            const auto screen = eu::render::LayoutData{ .style = eu::render::ViewportStyle::black_bars,
                                    .requested_width = 800, .requested_height = 600 };
            cmd.clear(eu::colors::blue_sky, screen);
            auto layer = eu::render::with_layer3(cmd, screen);
            eu::render::Camera camera;
            camera.position = { 0.0f, 0.0f, 0.0f };
            effects.render(eu::render::PostProcArg{
                .world = &world,
                .window_size = size_from_v2(layer.screen.get_size()),
                .final_rect = layer.screen,
                .camera = &camera,
                .renderer = &renderer
            });
        }

        // render hud
        {
            const auto screen = eu::render::LayoutData{ .style = eu::render::ViewportStyle::black_bars,
                                                     .requested_width = 1280, .requested_height = 720 };

            auto layer = eu::render::with_layer2(cmd, screen);
            const float hud_size = 100.0f;
            eu::render::Quad{ .tint = eu::colors::blue_sky }.draw(layer.batch, layer.viewport_aabb_in_worldspace.get_bottom(hud_size).get_right(hud_size).with_inset(eu::Lrtb{5.0f}));
        }

        SDL_GL_SwapWindow(window);
    }

    LOG_INFO("Shutting down");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
