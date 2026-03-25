#include "SDL.h"
#include "SDL_timer.h"

#include <cassert>
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cstdint>

#include "eustb_textedit_string.h"

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

ENABLE_HIGH_PERFORMANCE_GRAPHICS

/// djb2 initial hash
constexpr eu::u32 initial_hash = 5381;

/// djb2 hash function
eu::u32 hash(unsigned char *str, std::size_t length, eu::u32 seed)
{
    eu::u32 hash = seed;

    for(std::size_t index = 0; index < length; index += 1)
    {
        auto c = str[index];
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

struct IdStack
{
    [[nodiscard]] eu::u32 get(const std::string& str) const
    {
        return hash(reinterpret_cast<unsigned char*>(const_cast<char*>(str.data())), str.size(), last_id());
    }

    [[nodiscard]] eu::u32 get(int id) const
    {
        return hash(reinterpret_cast<unsigned char*>(&id), sizeof(int), last_id());
    }
    void push(const std::string& str)
    {
        stack.push_back(get(str));
    }
    void push(int id)
    {
        stack.push_back(get(id));
    }
    void pop()
    {
        stack.pop_back();
    }
    [[nodiscard]] eu::u32 last_id() const
    {
        return stack.empty() ? initial_hash : *stack.rbegin();
    }
    std::vector<eu::u32> stack;
};

struct KeyboardInput
{
    SDL_Keycode key;
    eu::u16 mod;
};

struct UiState
{
    eu::v2 mouse = {0,0};
    bool mousedown = false;
    std::optional<eu::u32> hot_item = std::nullopt;
    std::optional<eu::u32> active_item = std::nullopt;
    bool active_item_locked = false;

    std::optional<eu::u32> kbd_item = std::nullopt;
    std::optional<eu::u32> last_widget = std::nullopt;
    std::optional<KeyboardInput> key = std::nullopt; // todo(Gustav): change to a vector
    std::optional<char> keychar = std::nullopt; // todo(Gustav): change to a vector

    TextEditState text_state;

    void begin()
    {
        hot_item = std::nullopt;
    }

    void end()
    {
        if (mousedown)
        {
            if (active_item == std::nullopt)
            {
                active_item = std::nullopt;
                active_item_locked = true;
            }
        }
        else
        {
            active_item = std::nullopt;
            active_item_locked = false;
        }

        if (key && key->key == SDLK_TAB)
        {
            kbd_item = std::nullopt;
        }
        key = std::nullopt;
        keychar = std::nullopt;
    }

    [[nodiscard]] bool is_active_free() const
    {
        return active_item_locked == false && active_item.has_value() == false;
    }
};

// Simple button IMGUI widget
bool button_logic(eu::u32 id, const eu::Rect& rect, UiState& uistate)
{
    if (eu::is_within(uistate.mouse, rect))
    {
        uistate.hot_item = id;
        if (uistate.is_active_free() && uistate.mousedown)
        {
            uistate.active_item = id;
        }
    }

    if (uistate.kbd_item == std::nullopt)
    {
        uistate.kbd_item = id;
    }

    // keyboard interaction
    if (uistate.kbd_item == id)
    {
        if(uistate.key.has_value())
        {
            const auto k = *uistate.key;
            uistate.key = std::nullopt;
            switch (k.key)
            {
            case SDLK_TAB:
                uistate.kbd_item = std::nullopt;
                if (k.mod & KMOD_SHIFT)
                    uistate.kbd_item = uistate.last_widget;
                break;
            case SDLK_RETURN:
                return true;
            }
        }
    }

    uistate.last_widget = id;

    // If button is hot and active, but mouse button is not
      // down, the user must have clicked the button.
    if (uistate.mousedown == false &&
        uistate.hot_item == id &&
        uistate.active_item == id)
    {
        return true;
    }

    // Otherwise, no clicky.
    return false;
}

bool basic_button(eu::u32 id, const eu::Rect& rect, UiState& uistate, eu::render::SpriteBatch* batch)
{
    const auto ret = button_logic(id, rect, uistate);

    if (uistate.kbd_item == id)
    {
        eu::render::Quad{
            .tint = eu::colors::red_vermillion
        }.draw(batch, rect.with_inset(eu::Lrtb{ -4 }).with_offset({ 8, -8 }));
    }

    // shadow
    eu::render::Quad{
        .tint = eu::colors::black
    }.draw(batch, rect.with_offset({ 8.0f, -8.0f }));

    if (uistate.hot_item == id)
    {
        if (uistate.active_item == id)
        {
            // Button is both 'hot' and 'active'
            eu::render::Quad{ .tint = eu::colors::white }.draw(batch, rect.with_offset({ 2.0f, -2.0f }));
        }
        else
        {
            // Button is merely 'hot'
            eu::render::Quad{ .tint = eu::colors::white }.draw(batch, rect);
        }
    }
    else
    {
        // button is not hot, but it may be active    
        eu::render::Quad{ .tint = eu::colors::orange }.draw(batch, rect);
        // todo(Gustav): add grays
    }

    return ret;
}

bool button_texture(eu::u32 id, const eu::Rect& rect, UiState& uistate, eu::render::SpriteBatch* batch, const eu::render::Texture2d* texture, const eu::Rect& active, const eu::Rect& hot, const eu::Rect& normal)
{
    const auto ret = button_logic(id, rect, uistate);

    if (uistate.kbd_item == id)
    {
        // keep?
        eu::render::Quad{
            .tint = eu::colors::red_vermillion
        }.draw(batch, rect.with_inset(eu::Lrtb{ -4 }).with_offset({ 8, -8 }));
    }

    if (uistate.hot_item == id)
    {
        if (uistate.active_item == id)
        {
            // Button is both 'hot' and 'active'
            eu::render::Quad{ .texture = texture, .texturecoord = active }.draw(batch, rect);
        }
        else
        {
            // Button is merely 'hot'
            eu::render::Quad{ .texture = texture, .texturecoord = hot }.draw(batch, rect);
        }
    }
    else
    {
        // button is not hot, but it may be active    
        eu::render::Quad{ .texture = texture, .texturecoord = normal }.draw(batch, rect);
        // todo(Gustav): add grays
    }

    return ret;
}

bool slider(eu::u32 id, float* val, const eu::Rect& rect, UiState& uistate, eu::render::SpriteBatch* batch)
{
    const auto grab_size = 16.0f;
    const auto inner = rect.with_inset(eu::Lrtb{ 3.0f });
    const auto avail = inner.get_size().x - grab_size;
    const auto pos = avail * *val;

    const auto grabber_rect = eu::Rect::from_size({ grab_size, inner.get_size().y }).with_bottom_left_at({ inner.left + pos, inner.bottom });

    if (eu::is_within(uistate.mouse, inner))
    {
        uistate.hot_item = id;
        if (uistate.is_active_free() && uistate.mousedown)
        {
            uistate.active_item = id;
        }
    }

    if (uistate.kbd_item == std::nullopt)
    {
        uistate.kbd_item = id;
    }

    if (uistate.kbd_item == id)
    {
        eu::render::Quad{ .tint = eu::colors::red_vermillion }.draw(batch, rect.with_inset(eu::Lrtb{ -6 }));
    }

    eu::render::Quad{ .tint = eu::colors::orange }.draw(batch, rect);
    eu::render::Quad{ .tint = uistate.active_item == id && uistate.hot_item == id ? eu::colors::white : eu::colors::green_bluish }.draw(batch, grabber_rect);
    if (uistate.kbd_item == id)
    {
        if (uistate.key.has_value())
        {
            const auto k = *uistate.key;
            uistate.key = std::nullopt;
            constexpr float change = 0.1f;

            switch (k.key)
            {
            case SDLK_TAB:
                uistate.kbd_item = std::nullopt;
                if (k.mod & KMOD_SHIFT)
                {
                    uistate.kbd_item = uistate.last_widget;
                }
                break;
            case SDLK_LEFT:
                if (*val > 0)
                {
                    *val = std::max(0.0f, *val - change);
                    return true;
                }
                break;
            case SDLK_RIGHT:
                if (*val < 1)
                {
                    *val = std::min(1.0f, *val + change);
                    return true;
                }
                break;
            }
        }
    }

    uistate.last_widget = id;

    if (uistate.active_item == id)
    {
        const auto new_range = eu::to_01(inner, uistate.mouse);
        const auto new_value = eu::keep_within01(new_range.x);
        if (*val != new_value)
        {
            *val = new_value;
            return true;
        }
    }

    return false;
}


void draw_text(eu::render::SpriteBatch* batch, eu::render::DrawableFont* font, const std::string& str, float size, const eu::v2& pos, const eu::Rgb& color)
{
    eu::render::DrawableText text{ font };
    text.set_text(str);
    text.set_size(size);
    text.compile();
    text.draw(batch, pos, color, color);
}

bool textfield(eu::u32 id, std::string* val, eu::render::DrawableFont* font, float size, const eu::v2& pos, UiState& uistate, eu::render::SpriteBatch* batch)
{
    using namespace std;
    bool changed = false;

    auto& text_state = uistate.text_state;

    // Layout
    eu::render::DrawableText text{ font };
    text.set_text(*val);
    text.set_size(size);
    text.compile();
    const auto rect = text.get_extents().with_offset(pos).with_inset(eu::Lrtb{-6});

    if (eu::is_within(uistate.mouse, rect)) {
        uistate.hot_item = id;
        if (uistate.active_item == std::nullopt && uistate.mousedown)
        {
            uistate.active_item = id;
            text_state.focus(val);
        }
    }
    if (uistate.kbd_item == std::nullopt)
        uistate.kbd_item = id;

    if (uistate.kbd_item == id) {
        eu::render::Quad{ .tint = eu::colors::red_vermillion }.draw(batch, rect.with_inset(eu::Lrtb{ -6 }));
    }
    if (uistate.active_item == id || uistate.hot_item == id) {
        eu::render::Quad{ .tint = eu::colors::white }.draw(batch, rect.with_inset(eu::Lrtb{ -6 }));
    } else {
        eu::render::Quad{ .tint = eu::colors::green_bluish }.draw(batch, rect.with_inset(eu::Lrtb{ -6 }));
    }

    // Handle input
    if (uistate.kbd_item == id) {
        if (uistate.key.has_value()) {
            auto k = uistate.key.value();
            const auto shift = k.mod & KMOD_SHIFT;
            switch (k.key) {
                case SDLK_LEFT: text_state.onKeyLeft(shift, val); changed = true; break;
                case SDLK_RIGHT: text_state.onKeyRight(shift, val); changed = true; break;
                case SDLK_HOME: text_state.onKeyLineStart(shift, val); changed = true; break;
                case SDLK_END: text_state.onKeyLineEnd(shift, val); changed = true; break;
                case SDLK_BACKSPACE: text_state.onKeyBackspace(shift, val); changed = true; break;
                case SDLK_DELETE: text_state.onKeyDelete(shift, val); changed = true; break;
                case SDLK_RETURN: // ignore
                case SDLK_TAB:
                    uistate.kbd_item = std::nullopt;
                    if (shift)
                        uistate.kbd_item = uistate.last_widget;
                    uistate.key = std::nullopt;
                    break;
                default: break;
            }
        }
        if (uistate.keychar.has_value() && *uistate.keychar >= 32 && *uistate.keychar < 127) {
            char c = *uistate.keychar;
            text_state.onChar(c, val);
            changed = true;
        }
    }

    // Mouse click to set cursor
    if (uistate.mousedown == false && uistate.hot_item == id && uistate.active_item == id) {
        // Map mouse x to character index
        text_state.click(val, uistate.mouse.x - rect.left, uistate.mouse.y - rect.top);
        uistate.kbd_item = id;
    }

    uistate.last_widget = id;

    // Draw text
    text.draw(batch, pos, eu::colors::black, eu::colors::black);

    // Draw cursor
    if (uistate.kbd_item == id) {
        const auto is_cursor_blink_visible = (SDL_GetTicks() >> 8) & 1;
        if (is_cursor_blink_visible) {
            // Compute cursor x
            float cursor_x = rect.left;
            if (text_state.state.cursor > 0 && text_state.state.cursor <= (int)val->size()) {
                std::string substr = val->substr(0, text_state.state.cursor);
                eu::render::DrawableText t{ font };
                t.set_text(substr);
                t.set_size(size);
                t.compile();
                cursor_x = rect.left + t.get_extents().get_size().x;
            }
            // draw_text(batch, font, "_", size, {cursor_x, pos.y}, eu::colors::black);
            eu::render::Quad{ .tint = eu::colors::black }.draw(batch, eu::Rect::from_size({ 1, font->line_height * size }).with_bottom_left_at({ cursor_x, pos.y }));
        }
    }

    return changed;
}

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

#if 0
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else
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
#endif

    SDL_Window* window = SDL_CreateWindow("Euphoria", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    LOG_INFO("Editor started");
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
