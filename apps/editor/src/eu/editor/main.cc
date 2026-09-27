// todo(Gustav): look into glad support
#define SDL_FUNCTION_POINTER_IS_VOID_POINTER

#include "SDL.h"

#include <cassert>
#include <string>

#include "OpenSans-Regular.ttf.h"

#include "eu/log/log.h"
#include "eu/base/memorychunk.h"

#include "eu/render/canvas.h"
#include "eu/render/state.h"
#include "eu/render/font.h"
#include "eu/render/opengl_utils.h"
#include "eu/render/texture.io.h"

#include "eu/render/enable_high_performance_graphics.h"

#include "dear_imgui/imgui.h"
#include "dear_imgui/imgui_internal.h"

#include "dear_imgui/backends/imgui_impl_sdl3.h"
#include "dear_imgui/backends/imgui_impl_opengl3.h"

#include "eu/imgui/ui.h"
#include "eu/imgui/init.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

ENABLE_HIGH_PERFORMANCE_GRAPHICS

eu::MemoryChunk chunk_from_embed(const embedded_binary& binary)
{
    return { .bytes = reinterpret_cast<const char*>(binary.data), .size = binary.size };
}

enum class AppState
{
    continue_running, exit_failure, exit_ok
};

struct App
{
    App() = default;
    ~App();

    App(const App&) = delete;
    App(App&&) = delete;
    void operator=(const App&) = delete;
    void operator=(App&&) = delete;

    AppState create();
    AppState iterate();
    AppState on_event(const SDL_Event& ev);
    void destroy();

    int window_width = 0;
    int window_height = 0;
    SDL_Window* window = nullptr;
    SDL_GLContextState* glContext = nullptr;
    bool show_demo_window = true;

    std::unique_ptr<eu::render::State> states;
    std::unique_ptr<eu::render::Render2> render;
};

App::~App()
{
    destroy();
}

AppState App::create()
{
    const char* glsl_version = "#version 130";

    SDL_SetAppMetadata("Euphoria editor", "v0.1", "com.madeso.euphoria");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false)
    {
        LOG_ERR("Error initializing SDL: {}", SDL_GetError());
        return AppState::exit_failure;
    }

    const auto app_scale = eu::imgui::calculate_app_scale();
    std::tie(window_width, window_height) = eu::imgui::rescale_window(1280, 720, app_scale);

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

    window = SDL_CreateWindow("Editor sample",
        window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        LOG_ERR("Error creating window: {}", SDL_GetError());
        return AppState::exit_failure;
    }

    glContext = SDL_GL_CreateContext(window);
    SDL_GetWindowSize(window, &window_width, &window_height);
    if (!glContext) {
        LOG_ERR("Error creating gl context: {}", SDL_GetError());
        return AppState::exit_failure;
    }

    /* OpenGL setup */
    const int glad_result = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (glad_result == 0)
    {
        LOG_ERR("Failed to init glad, error: {0}", glad_result);
        return AppState::exit_failure;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

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

    {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(
            const_cast<void*>(static_cast<const void*>(OPENSANS_REGULAR_TTF_data)),
            OPENSANS_REGULAR_TTF_size,
            18.0f,
            &config
        );
        IM_ASSERT(font != nullptr);

        eu::imgui::setup_scale(app_scale);
    }

    states.reset(new eu::render::State());
    render.reset(new eu::render::Render2(states.get()));

    LOG_INFO("Editor started");
    return AppState::continue_running;
}

AppState App::on_event(const SDL_Event& ev)
{
    ImGui_ImplSDL3_ProcessEvent(&ev);
    switch (ev.type)
    {
    case SDL_EVENT_WINDOW_RESIZED:
        LOG_INFO("Resized");
        SDL_GetWindowSize(window, &window_width, &window_height);
        break;
    case SDL_EVENT_QUIT:
        return AppState::exit_ok;
    default:
        // ignore other events
        break;
    }

    return AppState::continue_running;
}

AppState App::iterate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    if (ImGui::Begin("Properties"))
    {
        static eu::v3 pos = {0,0,0};
        static eu::v3 rot = {0, 0, 0};
        static eu::v3 scale = {1, 1, 1};

        eu::imgui::label("Position");
        ImGui::DragFloat3("##Position", pos.get_data_ptr());

        eu::imgui::label("Rotation");
        ImGui::DragFloat3("##Rotation", rot.get_data_ptr());

        eu::imgui::label("Scale");
        ImGui::DragFloat3("##Scale", scale.get_data_ptr());

        eu::imgui::centered_button("Add component");
    }
    ImGui::End();

    {
        eu::render::RenderCommand cmd {.states = states.get(), .render = render.get(), .size = {.width = window_width, .height = window_height} };

        // todo(Gustav): provide a pixel layout
        const auto screen = eu::render::LayoutData{ .style = eu::render::ViewportStyle::extended,
                                                 .requested_width = static_cast<float>(window_width), .requested_height = static_cast<float>(window_height) };

        cmd.clear(eu::colors::blue_sky, screen);

        auto layer = eu::render::with_layer2(cmd, screen);
        eu::render::Quad{ .tint = eu::colors::green_bluish }.draw(layer.batch, layer.viewport_aabb_in_worldspace.get_bottom(50));
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    return AppState::continue_running;
}

void App::destroy()
{
    render.reset();
    states.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    LOG_INFO("Shutting down");
    SDL_GL_DestroyContext(glContext);
    glContext = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
}

// binding
SDL_AppResult to_sdl_result(AppState state)
{
    switch (state)
    {
    case AppState::continue_running: return SDL_APP_CONTINUE;
    case AppState::exit_failure: return SDL_APP_FAILURE;
    case AppState::exit_ok: return SDL_APP_SUCCESS;
    }

    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** out_app, int, char**)
{
    auto app = new App();
    const auto create_result = app->create();
    if (create_result == AppState::continue_running)
    {
        *out_app = app;
        return SDL_APP_CONTINUE;
    }

    delete app;
    return to_sdl_result(create_result);
}

SDL_AppResult SDL_AppIterate(void* app_arg)
{
    auto app = static_cast<App*>(app_arg);
    const auto result = app->iterate();
    return to_sdl_result(result);
}

SDL_AppResult SDL_AppEvent(void* app_arg, SDL_Event* ev)
{
    auto app = static_cast<App*>(app_arg);
    const auto result = app->on_event(*ev);
    return to_sdl_result(result);
}

void SDL_AppQuit(void* app_arg, SDL_AppResult result)
{
    if (result == SDL_APP_FAILURE)
    {
        LOG_ERR("App crashed :(");
    }

    auto app = static_cast<App*>(app_arg);
    delete app;
    app = nullptr;
}
