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
#include "eu/render/compiledmesh.h"
#include "eu/render/enable_high_performance_graphics.h"
#include "eu/render/postproc.h"
#include "eu/render/renderer.h"
#include "eu/render/world.h"

#include "eu/kdl/kdl.h"


#if FF_HAS(EU_DEBUG_RUNNER)
#include "eu/imgui/ui.h"

#include "dear_imgui/imgui.h"
#include "dear_imgui/backends/imgui_impl_sdl2.h"
#include "dear_imgui/backends/imgui_impl_opengl3.h"

static void imgui_color(const char* const label, eu::Rgb* color)
{
    ImGui::ColorEdit3(label, &color->r);
}
#endif


ENABLE_HIGH_PERFORMANCE_GRAPHICS

using namespace eu;

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
        load_image_from_embedded(SEND_DEBUG_LABEL_MANY(path) embedded_binary { reinterpret_cast<const unsigned int*>(bin.data()), static_cast<unsigned int>(bin.size()) }, texture_edge, eu::render::TextureRenderStyle::mipmap, transparency, cd)
    );
}

std::shared_ptr<eu::render::Texture2d> create_texture(DEBUG_LABEL_ARG_MANY eu::core::SingleColor pixel_color, eu::render::ColorData cd)
{
    return std::make_shared<eu::render::Texture2d>(
        load_image_from_color(SEND_DEBUG_LABEL_MANY(debug_label) pixel_color, eu::render::TextureEdge::repeat, eu::render::TextureRenderStyle::pixel, eu::render::Transparency::exclude, cd)
    );
}

struct Level
{
    std::unordered_map<std::string, std::unique_ptr<eu::render::CompiledMesh>> meshes;
    std::vector<std::unique_ptr<render::MeshInWorld>> instances;

    void load(const std::string& path, render::World* world, std::shared_ptr<render::DefaultMaterial> material, const core::CompiledGeomVertexAttributes& layout)
    {
        // todo(Gustav): this is very hacky but just something so we can play a level
        v3 size = { 1.0f, 1.0f, 1.0f };

        const auto doc = kdl::parse(eu::io::u8string_from_file(path));
        for (const auto& node: doc)
        {
            if (node.name() == u8"mesh")
            {
                const auto key = node.args()[0].as<std::string>();
                const auto file = node.args()[1].as<std::string>();

                const auto mesh = eu::io::mesh_from_file(file);
                auto compiled = eu::render::compile_mesh(USE_DEBUG_LABEL_MANY(file) mesh, layout);

                meshes[key] = std::make_unique<eu::render::CompiledMesh>(compiled);
            }
            else if (node.name() == u8"cell_size")
            {
                const auto x = node.args()[0].as<float>();
                const auto y = node.args()[1].as<float>();
                const auto z = node.args()[2].as<float>();
                size = {x, y, z};
            }
            else if (node.name() == u8"item")
            {
                const auto x = node.args()[0].as<float>();
                const auto y = node.args()[1].as<float>();
                const auto z = node.args()[2].as<float>();
                const v3 pos = { x*size.x, y*size.y, z*size.z };
                const auto item = node.properties().at(u8"item").as<std::string>();
                const auto found = meshes.find(item);
                if (found != meshes.end())
                {
                    render::MeshInWorld car;
                    car.add_to_world(found->second.get(), world, material);
                    car.set_transform(eu::render::transform_from_rotation(pos, Ypr{0_deg, 0_deg, 0_deg}));
                }
                else
                {
                    LOG_WARN("Mesh '{}' not found for item in level file", item);
                }
            }
            else
            {
                // todo(Gustav): add better printing with node name
                LOG_WARN("Unknown node in level file");
            }
        }
    }
};


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

#if FF_HAS(EU_DEBUG_RUNNER)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
#endif

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

    assets.black = create_texture(SEND_DEBUG_LABEL_MANY("black") eu::core::color_from_rgba(0, 0, 0, 255), eu::render::ColorData::dont_care);
    assets.white = create_texture(SEND_DEBUG_LABEL_MANY("white") eu::core::color_from_rgba(255, 255, 255, 255), eu::render::ColorData::dont_care);

    assets.default_shader_source = load_shader("default_shader");
    assets.skybox_shader_source = load_shader("skybox");

    assets.pp_vert_glsl = eu::io::string_from_file("pp_vert.glsl");
    assets.pp_realize_frag_glsl = eu::io::string_from_file("pp_realize.frag.glsl");
    assets.pp_extract_frag_glsl = eu::io::string_from_file("pp_extract.frag.glsl");
    assets.pp_ping_pong_blur_frag_glsl = eu::io::string_from_file("pp_ping_pong_blur.frag.glsl");

    eu::render::Renderer renderer{ &states, &assets, eu::render::RenderSettings{} };
    if (renderer.is_loaded() == false)
    {
        return -1;
    }

    eu::render::World world;
    eu::render::EffectStack effects;

#if FF_HAS(EU_DEBUG_RUNNER)
    imgui::ImguiShaderCache imgui_shader_cache;
#endif

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
        plane->transform = eu::m4::from_translation({ 0.0f, -3.0f, 0.0f });

        world.lights.point_lights.emplace_back();
    }

    // add demo mesh
    render::CompiledMesh carmesh;
    render::MeshInWorld car;
    eu::v3 position = { 0.0f, -1.8f, -10.0f };
    eu::Ypr rotation = { .yaw = 30.0_deg, .pitch = 30.0_deg, .roll = 30.0_deg };
    {
        const auto mesh = eu::io::mesh_from_file("models/vehicle-truck-purple.glb");
        carmesh = eu::render::compile_mesh(USE_DEBUG_LABEL_MANY("truck") mesh, renderer.default_geom_layout());

        auto material = renderer.make_default_material();
        material->diffuse = load_texture("models/textures/colormap.png", eu::render::ColorData::color_data);
        material->specular = assets.white;

        car.add_to_world(&carmesh, &world, material);
        car.set_transform(eu::render::transform_from_rotation(position, rotation));

        Level level;
        level.load("level.kdl", &world, material, renderer.default_geom_layout());
    }

    LOG_INFO("Runner started");
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
#if FF_HAS(EU_DEBUG_RUNNER)
            ImGui_ImplSDL2_ProcessEvent(&e);
#endif

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

#if FF_HAS(EU_DEBUG_RUNNER)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (ImGui::Begin("Renderer"))
        {
            imgui_color("Clear color", &world.clear_color);
            eu::imgui::simple_gamma_slider("Gamma/Brightness", &renderer.settings.gamma, -1.0f);
            ImGui::Checkbox("Use bloom", &renderer.settings.use_bloom);
            ImGui::DragFloat("Bloom cutoff", &renderer.settings.bloom_cutoff, 0.001f);
            ImGui::SliderFloat("Softness", &renderer.settings.bloom_softness, 0.0f, 1.0f);
            ImGui::DragInt("Bloom blur steps", &renderer.settings.bloom_blur_steps);
            effects.gui(&imgui_shader_cache);
        }
        ImGui::End();

        if (ImGui::Begin("Demo"))
        {
            imgui::drag("Position", &position);
            imgui::drag("Rotation", &rotation);
        }
        ImGui::End();
#endif

        car.set_transform(eu::render::transform_from_rotation(position, rotation));

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
            eu::render::Quad{ .tint = eu::colors::blue_sky }.draw(layer.batch, layer.viewport_aabb_in_worldspace.get_bottom(hud_size).get_right(hud_size).with_inset(eu::Lrtb{ 5.0f }));
        }

#if FF_HAS(EU_DEBUG_RUNNER)
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        SDL_GL_SwapWindow(window);
    }

#if FF_HAS(EU_DEBUG_RUNNER)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif

    LOG_INFO("Shutting down");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
