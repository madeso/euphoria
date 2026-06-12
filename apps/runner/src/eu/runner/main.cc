#include <utility>

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

#include "eu/runner/script.h"
#include "eu/runner/input.h"
#include "eu/runner/entity.h"

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
using namespace std::string_view_literals;

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
        v3 offset = { 0.0f, 0.0f, 0.0f };
        v3 size = { 1.0f, 1.0f, 1.0f };

        const auto doc = kdl::parse(eu::io::string_from_file(path));
        for (const auto& node: doc)
        {
            if (node.name() == "mesh")
            {
                const auto key = node.args()[0].as_string();
                const auto file = node.args()[1].as_string();

                const auto mesh = eu::io::mesh_from_file(file);
                auto compiled = eu::render::compile_mesh(USE_DEBUG_LABEL_MANY(file) mesh, layout);

                meshes[key] = std::make_unique<eu::render::CompiledMesh>(compiled);
            }
            else if (node.name() == "offset")
            {
                const auto x = node.args()[0].as_number().as<float>();
                const auto y = node.args()[1].as_number().as<float>();
                const auto z = node.args()[2].as_number().as<float>();
                offset = { x, y, z };
            }
            else if (node.name() == "cell_size")
            {
                const auto x = node.args()[0].as_number().as<float>();
                const auto y = node.args()[1].as_number().as<float>();
                const auto z = node.args()[2].as_number().as<float>();
                size = {x, y, z};
            }
            else if (node.name() == "item")
            {
                const auto x = node.args()[0].as_number().as<float>();
                const auto y = node.args()[1].as_number().as<float>();
                const auto z = node.args()[2].as_number().as<float>();
                const v3 pos = { x*size.x, y*size.y, z*size.z };
                const auto item = node.properties().at("item").as_string();
                const auto found = meshes.find(item);
                if (found != meshes.end())
                {
                    render::MeshInWorld car;
                    car.add_to_world(found->second.get(), world, material);
                    car.set_transform(eu::render::transform_from_rotation(offset + pos, Ypr{0_deg, 0_deg, 0_deg}));
                }
                else
                {
                    LOG_WARN("Mesh '{}' not found for item in level file", item);
                }
            }
            else
            {
                LOG_WARN("Unknown node in level file: '{}'", node.name());
            }
        }
    }
};


struct Time
{
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    

    float calculate()
    {
        last = now;
        now = SDL_GetPerformanceCounter();

        const auto diff = static_cast<float>(now - last);
        const auto pfc = static_cast<float>(SDL_GetPerformanceFrequency());
        return diff / pfc;
    }
};


struct MeshComponent : runner::SpatialComponent
{
    render::MeshInWorld car;

    eu::v3 position = { 0.0f, -1.8f, -10.0f };
    eu::Ypr rotation = { .yaw = 30.0_deg, .pitch = 30.0_deg, .roll = 30.0_deg };

    void update_transform()
    {
        set_transform(eu::render::transform_from_rotation(position, rotation));
        car.set_transform(get_transform());
    }

    MeshComponent(render::CompiledMesh* mesh, render::World* render_world, std::shared_ptr<render::Material> material)
    {
        car.add_to_world(mesh, render_world, std::move(material));
        update_transform();
    }

    EU_DEC_COMPONENT_TYPE();
};

EU_IMP_COMPONENT_TYPE(MeshComponent, runner::Component)

struct InputSystem : runner::EntitySystem
{
    runner::Input* input;
    MeshComponent* mesh = nullptr;

    explicit InputSystem(runner::Input* in) : input(in)
    {
    }

    runner::UpdateStageAndPrio get_stage() override
    {
        return
        {
        .stage = runner::UpdateStage::before_physics,
        .prio = 100
        };
    }

    void add_component(runner::Component* component) override
    {
        if (MeshComponent* mc = runner::component_cast<MeshComponent>(component); mc)
        {
            mesh = mc;
        }
    }

    void update(float dt) override
    {
        if (mesh && input && input->get_current_frame().binds[0].current > 0.5f)
        {
            mesh->position += kk::in * dt;
        }

        if (mesh)
        {
            mesh->update_transform();
        }
    }
};

struct TargetComponent : runner::Component
{
    m4 target = m4_identity;

    EU_DEC_COMPONENT_TYPE();
};
EU_IMP_COMPONENT_TYPE(TargetComponent, runner::Component)

struct CameraComponent : runner::SpatialComponent
{
    // todo(Gustav): add extra camera settings...
    EU_DEC_COMPONENT_TYPE();
};
EU_IMP_COMPONENT_TYPE(CameraComponent, runner::SpatialComponent);

struct FollowCameraSystem : runner::EntitySystem
{
    runner::UpdateStageAndPrio get_stage() override
    {
        return
        {
            .stage = runner::UpdateStage::after_physics,
            .prio = 100
        };
    }

    TargetComponent* target = nullptr;
    CameraComponent* camera = nullptr;
    void add_component(runner::Component* component) override
    {
        if (auto* tar = runner::component_cast<TargetComponent>(component))
        {
            target = tar;
        }
        else if (auto* cam = runner::component_cast<CameraComponent>(component))
        {
            camera = cam;
        }
    }
    void update(float dt) override
    {
        if (!target) { return; }
        if (!camera) { return; }

        const auto nt = target->target.get_translated(kk::out * 3);
        camera->set_transform(nt);
    }
};

namespace tag
{
constexpr Hsh CameraSpatialSource = "camera_spatial_src_tag"sv;
constexpr Hsh CameraDestination = "camera_dst_tag"sv;
}

struct UpdateCameraTarget : runner::WorldSystem
{
    runner::SpatialComponent* src_entity = nullptr;
    TargetComponent* dst_entity = nullptr;

    runner::UpdateStageAndPrio get_stage() override
    {
        return {
            .stage = runner::UpdateStage::after_physics,
            .prio = 100
        };
    }
    
    void add_component(runner::Entity* entity, runner::Component* component) override
    {
        if (entity->has_tag(tag::CameraSpatialSource))
        {
            if (auto* spatial = entity->root; spatial)
            {
                LOG_INFO("Found spatial source");
                src_entity = spatial;
            }
            else
            {
                LOG_WARN("Entity with tag {} is missing spatial component", tag::CameraSpatialSource);
            }
        }
        else if (entity->has_tag(tag::CameraDestination))
        {
            if (auto* target = runner::component_cast<TargetComponent>(component))
            {
                dst_entity = target;
            }
            else
            {
                // todo(Gustav): make HashSt printable TargetComponent::type()
                LOG_WARN("Entity with tag {} is missing target component", tag::CameraDestination);
            }
        }
    }

    void update(float dt) override
    {
        if (!src_entity) { return; }
        if (!dst_entity) { return; }

        dst_entity->target = src_entity->get_transform();
    }
};

struct CameraFetcherSystem : runner::WorldSystem
{
    runner::UpdateStageAndPrio get_stage() override
    {
        return {
            .stage = runner::UpdateStage::end_frame,
            .prio = 100
        };
    }

    CameraComponent* camera = nullptr;
    m4 transform = m4_identity;

    void add_component(runner::Entity* entity, runner::Component* component) override
    {
        if (auto cam = runner::component_cast<CameraComponent>(component))
        {
            camera = cam;
        }
    }

    void update(float dt) override
    {
        if (!camera) return;
        transform = camera->get_transform();
    }

    render::Camera fetch() const
    {
        render::Camera r;
        r.position = transform.get_translation();
        return r;
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

    {
        if (GLAD_GL_VERSION_1_0) {
            const std::string gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
            const std::string gl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
            const std::string gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
            LOG_INFO("OpenGL Vendor:   {0}", gl_vendor);
            LOG_INFO("OpenGL Renderer: {0}", gl_renderer);
            LOG_INFO("OpenGL Version:  {0}", gl_version);
        }

        if (GLAD_GL_VERSION_2_0) {
            const std::string gl_shading_language_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
            LOG_INFO("Version GLSL:   {0}", gl_shading_language_version);
        }
    }

    if (!GLAD_GL_VERSION_4_3)
    {
        LOG_ERR("OpenGL 4.3 not supported ({0}.{1} detected), aborting...", GLVersion.major, GLVersion.minor);
        return -1;
    }

#if FF_HAS(EU_DEBUG_RUNNER)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    if (false == ImGui_ImplSDL2_InitForOpenGL(window, glContext))
    {
        LOG_ERR("Failed to init ImGui SDL2 backend");
        return -1;
    }
    if (false == ImGui_ImplOpenGL3_Init(glsl_version))
    {
        LOG_ERR("Failed to init ImGui OpenGL3 backend");
        return -1;
    }

    bool show_demo_window = true;
#endif

    eu::render::State states;
    eu::render::Render2 render{ &states };

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

    eu::runner::Input input;
    if (false == input.load_from_file("input.kdl"))
    {
        return -1;
    }

    eu::runner::Script script;
    if (false == script.run_file("main.lax"))
    {
        return -1;
    }

    eu::render::World render_world;
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
        render_world.meshes.emplace_back(plane);
        plane->transform = eu::m4::from_translation({ 0.0f, -3.0f, 0.0f });

        render_world.lights.point_lights.emplace_back();
    }

    // add demo mesh
    eu::runner::World runner_world;
    std::unordered_map<std::string, std::unique_ptr<render::CompiledMesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<eu::render::Texture2d>> textures;

    const auto load_mesh = [&](const std::string& f) -> render::CompiledMesh*
        {
            const auto found = meshes.find(f);
            if (found != meshes.end())
            {
                return found->second.get();
            }
            const auto mesh = eu::io::mesh_from_file(f);
            const auto compiled = eu::render::compile_mesh(USE_DEBUG_LABEL_MANY(f) mesh, renderer.default_geom_layout());
            auto data = std::make_unique<render::CompiledMesh>(compiled);
            auto* ret = data.get();
            meshes.insert_or_assign(f, std::move(data));
            return ret;
        };

    const auto load_color_texture = [&](const std::string& f) -> std::shared_ptr<render::Texture2d>
        {
            const auto found = textures.find(f);
            if (found != textures.end())
            {
                return found->second;
            }
            const auto loaded = load_texture(f, eu::render::ColorData::color_data);
            textures.insert_or_assign(f, loaded);
            return loaded;
        };

    runner_world.add_system(std::make_unique<UpdateCameraTarget>());
    
    CameraFetcherSystem* camera_fetcher;
    {
        auto fetch = std::make_unique<CameraFetcherSystem>();
        camera_fetcher = fetch.get();
        runner_world.add_system(std::move(fetch));
    }

    {
        auto* car = runner_world.add_entity();
        car->add_tag(tag::CameraSpatialSource);

        // mesh component
        // debug/axis.glb"
        // "models/vehicle-truck-purple.glb"
        // "models/textures/colormap.png"
        // render::CompiledMesh* mesh, render::World* render_world, std::shared_ptr<render::Material> material
        auto* mesh = load_mesh("models/vehicle-truck-purple.glb");
        auto material = renderer.make_default_material();
        material->diffuse = load_color_texture("models/textures/colormap.png");
        material->specular = assets.white;
        auto comp = std::make_unique<MeshComponent>(mesh, &render_world, material);
        car->add_component(std::move(comp));

        // input component
        car->add_system(std::make_unique<InputSystem>(&input));
    }
    {
        auto* cam = runner_world.add_entity();
        cam->add_component(std::make_unique<TargetComponent>());
        cam->add_component(std::make_unique<CameraComponent>());
        cam->add_system(std::make_unique<FollowCameraSystem>());
        cam->add_tag(tag::CameraDestination);
    }
    {
        Level level;
        auto material = renderer.make_default_material();
        material->diffuse = load_color_texture("models/textures/colormap.png");
        material->specular = assets.white;
        level.load("level.kdl", &render_world, material, renderer.default_geom_layout());
    }

    LOG_INFO("Runner started");
    bool running = true;
    Time time;
    while (running)
    {
        const auto dt = time.calculate();
        
        runner_world.update(runner::UpdateStage::start_frame, dt);
        
        SDL_Event e;
        input.update();
        while (SDL_PollEvent(&e) != 0)
        {
#if FF_HAS(EU_DEBUG_RUNNER)
            ImGui_ImplSDL2_ProcessEvent(&e);
#endif

            switch (e.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                input.on_key(e.key.keysym.sym, e.type == SDL_KEYDOWN);
                break;
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

        // todo(Gustav): lookup unity frame, when updates are called and update stages
        runner_world.update(runner::UpdateStage::before_physics, dt);
        runner_world.update(runner::UpdateStage::physics, dt);
        runner_world.update(runner::UpdateStage::after_physics, dt);

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
            imgui_color("Clear color", &render_world.clear_color);
            eu::imgui::simple_gamma_slider("Gamma/Brightness", &renderer.settings.gamma, -1.0f);
            ImGui::Checkbox("Use bloom", &renderer.settings.use_bloom);
            ImGui::DragFloat("Bloom cutoff", &renderer.settings.bloom_cutoff, 0.001f);
            ImGui::SliderFloat("Softness", &renderer.settings.bloom_softness, 0.0f, 1.0f);
            ImGui::DragInt("Bloom blur steps", &renderer.settings.bloom_blur_steps);
            effects.gui(&imgui_shader_cache);
        }
        ImGui::End();

        // todo(Gustav): add introspection ui
        // if (ImGui::Begin("Demo"))
        // {
        //     imgui::drag("Position", &position);
        //     imgui::drag("Rotation", &rotation);
        // }
        // ImGui::End();
#endif

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
            // todo(Gustav): get transform from camera entity
            effects.update(dt);
            eu::render::Camera camera = camera_fetcher->fetch();
            effects.render(eu::render::PostProcArg{
                .world = &render_world,
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

        runner_world.update(runner::UpdateStage::end_frame, dt);
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
