#include "SDL.h"
#include "SDL_timer.h"

#include <cassert>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cstdint>

#include "dependency_glad.h"
#include "OpenSans-Regular.ttf.h"
#include "uv-texture.png.h"
#include "spritesheet.png.h"

#include "log/log.h"
#include "base/memorychunk.h"

#include "render/canvas.h"
#include "render/opengl_states.h"
#include "render/font.h"
#include "render/opengl_utils.h"
#include "render/texture.io.h"
#include "render/enable_high_performance_graphics.h"

#include "data/spritesheet.h"

#include "mrgui/widgets.h"

using namespace eu::mrgui;

ENABLE_HIGH_PERFORMANCE_GRAPHICS

eu::MemoryChunk chunk_from_embed(const embedded_binary& binary)
{
    return { .bytes = reinterpret_cast<const char*>(binary.data), .size = binary.size };
}

int  main(int, char**)
{
    int window_width = 1280;
    int window_height = 720;

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

    SDL_Window* window = SDL_CreateWindow("mrgui sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    eu::render::OpenglStates states;
    eu::render::Render2 render{ &states };

    UiState uistate;
    IdStack idstack;
    eu::render::DrawableFont font{chunk_from_embed(OPENSANS_REGULAR_TTF)};

    const auto sample_texture = eu::render::load_image_from_embedded(SEND_DEBUG_LABEL_MANY("uv-texture.png") UV_TEXTURE_PNG,
        eu::render::TextureEdge::clamp, eu::render::TextureRenderStyle::linear, eu::render::Transparency::exclude, eu::render::ColorData::color_data);

    const auto spritesheet = eu::render::load_image_from_embedded(SEND_DEBUG_LABEL_MANY("spritesheet.png") SPRITESHEET_PNG,
        eu::render::TextureEdge::repeat, eu::render::TextureRenderStyle::linear, eu::render::Transparency::include, eu::render::ColorData::color_data);

    bool running = true;
    std::string editable_text = "Editable text";

    LOG_INFO("Mrgui sample started");
    while (running)
    {
        // events
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_MOUSEMOTION:
                uistate.mouse = { static_cast<float>(e.motion.x), static_cast<float>(window_height - e.motion.y) };
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == 1)
                {
                    uistate.mousedown = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == 1)
                {
                    uistate.mousedown = false;
                }
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    LOG_INFO("Resized");
                    SDL_GetWindowSize(window, &window_width, &window_height);
                }
                break;
            case SDL_KEYDOWN:
                uistate.key = KeyboardInput {.key = e.key.keysym.sym, .mod = e.key.keysym.mod };
                break;
            case SDL_TEXTINPUT:
                {
                // todo(Gustav): handle unicode
                char c = e.text.text[0];
                if ((c & 0xFF80) == 0)
                    uistate.keychar = static_cast<char>(c & 0x7f);
                }
                break;
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }

        // render
        {
            eu::render::RenderCommand cmd {.states = &states, .render = &render, .size = {.width = window_width, .height = window_height} };

            // todo(Gustav): provide a pixel layout
            const auto screen = eu::render::LayoutData{ .style = eu::render::ViewportStyle::extended,
                                                     .requested_width = static_cast<float>(window_width), .requested_height = static_cast<float>(window_height) };

            cmd.clear(eu::colors::blue_sky, screen);

            auto layer = eu::render::with_layer2(cmd, screen);
            eu::render::Quad{ .tint = eu::colors::green_bluish }.draw(layer.batch, layer.viewport_aabb_in_worldspace.get_bottom(50));

            static eu::An rotation = eu::no_rotation;
            static float slider_a = 0.5f;
            static float slider_b = 0.5f;

            eu::render::Quad{
                .texture = &sample_texture,
                // select bottom right of the texture, assuming it is split in 4 smaller rects
                .texturecoord = eu::Rect::from_bottom_left_size({0.5f, 0}, {0.5f, 0.5f}),
                .rotation = eu::render::RotateQuad{.angle = rotation, .center = {slider_a, slider_b} }
            }.draw(layer.batch, eu::Rect::from_bottom_left_size({ 300, 300 }, { 300, 300 }));

            const auto button_size = eu::v2{ 64, 64 };

            uistate.begin();
            if (basic_button(idstack.get("a"), eu::Rect::from_bottom_left_size({ 100, 100 }, button_size), uistate, layer.batch))
            {
                rotation -= eu::An::from_degrees(15.0f);
            }
            if (basic_button(idstack.get("b"), eu::Rect::from_bottom_left_size({ 200, 100 }, button_size), uistate, layer.batch))
            {
                rotation += eu::An::from_degrees(15.0f);
            }
            if (basic_button(idstack.get("c"), eu::Rect::from_bottom_left_size({ 300, 100 }, button_size), uistate, layer.batch))
            {
                rotation = eu::no_rotation;
            }
            button_texture(idstack.get("d"), eu::Rect::from_bottom_left_size({ 400, 100 }, button_size), uistate, layer.batch, &spritesheet,
                ::spritesheet::button_square_flat,
                ::spritesheet::button_square_depth_flat,
                ::spritesheet::button_square_depth_gradient
            );

            const auto slider_size = eu::v2{ 256, 25 };
            slider(idstack.get("slider_a"), &slider_a, eu::Rect::from_bottom_left_size({ 100, 200 }, slider_size), uistate, layer.batch);
            slider(idstack.get("slider_b"), &slider_b, eu::Rect::from_bottom_left_size({ 100, 250 }, slider_size), uistate, layer.batch);
            textfield(idstack.get("textfield"), &editable_text, &font, 20, {100, 300}, uistate, layer.batch);
            uistate.end();

            draw_text(layer.batch, &font, "Hello world", 60, {200, 400 + 200}, eu::colors::black);
        }
        SDL_GL_SwapWindow(window);
    }

    LOG_INFO("Shutting down");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
