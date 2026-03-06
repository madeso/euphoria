#include "SDL.h"
#include "SDL_timer.h"

#include <cassert>
#include "dependency_glad.h"
#include "OpenSans-Regular.ttf.h"
#include "uv-texture.png.h"
#include "stb_image.h"

#include "log/log.h"
#include "base/memorychunk.h"

#include "render/canvas.h"
#include "render/opengl_states.h"
#include "render/font.h"
#include "render/opengl_utils.h"
#include "render/texture.io.h"
#include "render/enable_high_performance_graphics.h"

ENABLE_HIGH_PERFORMANCE_GRAPHICS

#if 1

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

struct UiState
{
    eu::v2 mouse = {0,0};
    bool mousedown = false;

    std::optional<eu::u32> hot_item = std::nullopt;

    std::optional<eu::u32> active_item = std::nullopt;
    bool active_item_locked = false;

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
    }

    [[nodiscard]] bool is_active_free() const
    {
        return active_item_locked == false && active_item.has_value() == false;
    }
};

// Simple button IMGUI widget
bool button(eu::u32 id, const eu::Rect& rect, UiState& uistate, eu::render::SpriteBatch* batch)
{
    // Check whether the button should be hot
    if (eu::is_within(uistate.mouse, rect))
    {
        uistate.hot_item = id;
        if (uistate.is_active_free() && uistate.mousedown)
        {
            uistate.active_item = id;
        }
    }
    
    // shadow
    batch->quad(std::nullopt, rect.with_offset({8.0f, -8.0f}), std::nullopt, eu::colors::black);
    
    if (uistate.hot_item == id)
    {
        if (uistate.active_item == id)
        {
            // Button is both 'hot' and 'active'
            batch->quad(std::nullopt, rect.with_offset({ 2.0f, -2.0f }), std::nullopt, eu::colors::white);
        }
        else
        {
            // Button is merely 'hot'
            batch->quad(std::nullopt, rect, std::nullopt, eu::colors::white);
        }
    }
    else
    {
        // button is not hot, but it may be active    
        batch->quad(std::nullopt, rect, std::nullopt, eu::colors::orange);
        // todo(Gustav): add grays
    }

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

    batch->quad(std::nullopt, rect, std::nullopt, eu::colors::orange);
    batch->quad(std::nullopt, grabber_rect, std::nullopt, uistate.active_item == id && uistate.hot_item == id ? eu::colors::white : eu::colors::green_bluish);

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

eu::MemoryChunk chunk_from_embed(const embedded_binary& binary)
{
    return { .bytes = reinterpret_cast<const char*>(binary.data), .size = binary.size };
}

int  main(int, char**)
{
    int window_width = 1280;
    int window_height = 720;

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        LOG_ERR("Error initializing SDL: {}", SDL_GetError());
        return -1;
    }

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
    eu::render::DrawableText text{&font};
    text.set_text("Hello world");
    text.set_size(40);
    text.compile();

    const auto sample_texture = eu::render::load_image_from_embedded(SEND_DEBUG_LABEL_MANY("uv-texture.png") UV_TEXTURE_PNG,
        eu::render::TextureEdge::clamp, eu::render::TextureRenderStyle::linear, eu::render::Transparency::exclude, eu::render::ColorData::color_data);

    bool running = true;

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
            layer.batch->quad(std::nullopt, layer.viewport_aabb_in_worldspace.get_bottom(50) , std::nullopt, eu::colors::green_bluish);

            layer.batch->quad(&sample_texture, eu::Rect::from_bottom_left_size({300, 300}, {300, 300}),
                // select bottom right of the texture, assuming it is split in 4 smaller rects
                eu::Rect::from_bottom_left_size({0.5f, 0}, {0.5f, 0.5f}),
                eu::colors::white);

            const auto button_size = eu::v2{ 64, 64 };

            uistate.begin();
            button(idstack.get("a"), eu::Rect::from_bottom_left_size({ 100, 100 }, button_size), uistate, layer.batch);
            button(idstack.get("b"), eu::Rect::from_bottom_left_size({ 200, 100 }, button_size), uistate, layer.batch);
            button(idstack.get("c"), eu::Rect::from_bottom_left_size({ 300, 100 }, button_size), uistate, layer.batch);
            button(idstack.get("d"), eu::Rect::from_bottom_left_size({ 400, 100 }, button_size), uistate, layer.batch);

            const auto slider_size = eu::v2{ 256, 25 };
            static float slider_a = 0.5f;
            static float slider_b = 0.5f;
            slider(idstack.get("slider_a"), &slider_a, eu::Rect::from_bottom_left_size({ 100, 200 }, slider_size), uistate, layer.batch);
            slider(idstack.get("slider_b"), &slider_b, eu::Rect::from_bottom_left_size({ 100, 250 }, slider_size), uistate, layer.batch);
            uistate.end();

            text.draw(layer.batch, {50, 500}, eu::colors::black, eu::colors::purple_redish);
        }
        SDL_GL_SwapWindow(window);
    }

    LOG_INFO("Shutting down");
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#else

#include "dependency_nuklear.h"
#include "dependency_nuklear_impl.h"


constexpr int MAX_VERTEX_MEMORY = 512 * 1024;
constexpr int MAX_ELEMENT_MEMORY = 128 * 1024;

/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the defines */
#define INCLUDE_ALL
// #define INCLUDE_STYLE
// #define INCLUDE_CALCULATOR
// #define INCLUDE_CANVAS
#define INCLUDE_OVERVIEW
// #define INCLUDE_CONFIGURATOR
// #define INCLUDE_NODE_EDITOR

#ifdef INCLUDE_ALL
#define INCLUDE_STYLE
#define INCLUDE_CALCULATOR
#define INCLUDE_CANVAS
#define INCLUDE_OVERVIEW
#define INCLUDE_CONFIGURATOR
#define INCLUDE_NODE_EDITOR
#endif

#define NK_ASSERT(expr) assert(expr)
constexpr nk_color nk_white = { 255,255,255,255 };

#define LEN(a) (sizeof(a)/sizeof(a)[0])

#define NK_COLOR_MAP(NK_COLOR)\
    NK_COLOR(NK_COLOR_TEXT,                     175,175,175,255) \
    NK_COLOR(NK_COLOR_WINDOW,                   45, 45, 45, 255) \
    NK_COLOR(NK_COLOR_HEADER,                   40, 40, 40, 255) \
    NK_COLOR(NK_COLOR_BORDER,                   65, 65, 65, 255) \
    NK_COLOR(NK_COLOR_BUTTON,                   50, 50, 50, 255) \
    NK_COLOR(NK_COLOR_BUTTON_HOVER,             40, 40, 40, 255) \
    NK_COLOR(NK_COLOR_BUTTON_ACTIVE,            35, 35, 35, 255) \
    NK_COLOR(NK_COLOR_TOGGLE,                   100,100,100,255) \
    NK_COLOR(NK_COLOR_TOGGLE_HOVER,             120,120,120,255) \
    NK_COLOR(NK_COLOR_TOGGLE_CURSOR,            45, 45, 45, 255) \
    NK_COLOR(NK_COLOR_SELECT,                   45, 45, 45, 255) \
    NK_COLOR(NK_COLOR_SELECT_ACTIVE,            35, 35, 35,255)  \
    NK_COLOR(NK_COLOR_SLIDER,                   38, 38, 38, 255) \
    NK_COLOR(NK_COLOR_SLIDER_CURSOR,            100,100,100,255) \
    NK_COLOR(NK_COLOR_SLIDER_CURSOR_HOVER,      120,120,120,255) \
    NK_COLOR(NK_COLOR_SLIDER_CURSOR_ACTIVE,     150,150,150,255) \
    NK_COLOR(NK_COLOR_PROPERTY,                 38, 38, 38, 255) \
    NK_COLOR(NK_COLOR_EDIT,                     38, 38, 38, 255) \
    NK_COLOR(NK_COLOR_EDIT_CURSOR,              175,175,175,255) \
    NK_COLOR(NK_COLOR_COMBO,                    45, 45, 45, 255) \
    NK_COLOR(NK_COLOR_CHART,                    120,120,120,255) \
    NK_COLOR(NK_COLOR_CHART_COLOR,              45, 45, 45, 255) \
    NK_COLOR(NK_COLOR_CHART_COLOR_HIGHLIGHT,    255, 0,  0, 255) \
    NK_COLOR(NK_COLOR_SCROLLBAR,                40, 40, 40, 255) \
    NK_COLOR(NK_COLOR_SCROLLBAR_CURSOR,         100,100,100,255) \
    NK_COLOR(NK_COLOR_SCROLLBAR_CURSOR_HOVER,   120,120,120,255) \
    NK_COLOR(NK_COLOR_SCROLLBAR_CURSOR_ACTIVE,  150,150,150,255) \
    NK_COLOR(NK_COLOR_TAB_HEADER,               40, 40, 40,255)  \
    NK_COLOR(NK_COLOR_KNOB,                     38, 38, 38, 255) \
    NK_COLOR(NK_COLOR_KNOB_CURSOR,              100,100,100,255) \
    NK_COLOR(NK_COLOR_KNOB_CURSOR_HOVER,        120,120,120,255) \
    NK_COLOR(NK_COLOR_KNOB_CURSOR_ACTIVE,       150,150,150,255)

NK_GLOBAL const struct nk_color
nk_default_color_style[NK_COLOR_COUNT] = {
#define NK_COLOR(a,b,c,d,e) {b,c,d,e},
    NK_COLOR_MAP(NK_COLOR)
#undef NK_COLOR
};

#ifdef INCLUDE_STYLE
#include "demo/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
#include "demo/calculator.c"
#endif
#ifdef INCLUDE_CANVAS
#include "demo/canvas.c"
#endif
#ifdef INCLUDE_OVERVIEW
#include "demo/overview.c"
#endif
#ifdef INCLUDE_CONFIGURATOR
#include "demo/style_configurator.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
#include "demo/node_editor.c"
#endif

struct media {
    GLint skin;
    struct nk_image menu;
    struct nk_image check;
    struct nk_image check_cursor;
    struct nk_image option;
    struct nk_image option_cursor;
    struct nk_image header;
    struct nk_image window;
    struct nk_image scrollbar_inc_button;
    struct nk_image scrollbar_inc_button_hover;
    struct nk_image scrollbar_dec_button;
    struct nk_image scrollbar_dec_button_hover;
    struct nk_image button;
    struct nk_image button_hover;
    struct nk_image button_active;
    struct nk_image tab_minimize;
    struct nk_image tab_maximize;
    struct nk_image slider;
    struct nk_image slider_hover;
    struct nk_image slider_active;
};


static GLuint
image_load(const char* filename)
{
    int x, y, n;
    GLuint tex;
    unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
    if (!data)
    {
        LOG_ERR("failed to load image: {}", filename);
        std::exit(-1);
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return tex;
}

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
int main(int, char **)
{
    int window_width = 1280;
    int window_height = 720;

    SDL_Window *win;
    SDL_GLContext glContext;
    int win_width, win_height;
    bool running = true;

    /* GUI */
    struct nk_context *ctx;
    struct nk_colorf bg;
    struct media media;

    #ifdef INCLUDE_CONFIGURATOR
    static struct nk_color color_table[NK_COLOR_COUNT];
    memcpy(color_table, nk_default_color_style, sizeof(color_table));
    #endif

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    win = SDL_CreateWindow("Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    glContext = SDL_GL_CreateContext(win);
    SDL_GetWindowSize(win, &win_width, &win_height);

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
    glViewport(0, 0, window_width, window_height);

    ctx = nk_sdl_init(win);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_sdl_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &roboto->handle);*/}

    /* style.c */
    #ifdef INCLUDE_STYLE
    /* ease regression testing during Nuklear release process; not needed for anything else */
    #ifdef STYLE_WHITE
    set_style(ctx, THEME_WHITE);
    #elif defined(STYLE_RED)
    set_style(ctx, THEME_RED);
    #elif defined(STYLE_BLUE)
    set_style(ctx, THEME_BLUE);
    #elif defined(STYLE_DARK)
    set_style(ctx, THEME_DARK);
    #endif
    #endif

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    {   /* skin */
        glEnable(GL_TEXTURE_2D);
        media.skin = image_load("../skins/gwen.png");
        media.check = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 32, 15, 15));
        media.check_cursor = nk_subimage_id(media.skin, 512, 512, nk_rect(450, 34, 11, 11));
        media.option = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 64, 15, 15));
        media.option_cursor = nk_subimage_id(media.skin, 512, 512, nk_rect(451, 67, 9, 9));
        media.header = nk_subimage_id(media.skin, 512, 512, nk_rect(128, 0, 127, 24));
        media.window = nk_subimage_id(media.skin, 512, 512, nk_rect(128, 23, 127, 104));
        media.scrollbar_inc_button = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 256, 15, 15));
        media.scrollbar_inc_button_hover = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 320, 15, 15));
        media.scrollbar_dec_button = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 224, 15, 15));
        media.scrollbar_dec_button_hover = nk_subimage_id(media.skin, 512, 512, nk_rect(464, 288, 15, 15));
        media.button = nk_subimage_id(media.skin, 512, 512, nk_rect(384, 336, 127, 31));
        media.button_hover = nk_subimage_id(media.skin, 512, 512, nk_rect(384, 368, 127, 31));
        media.button_active = nk_subimage_id(media.skin, 512, 512, nk_rect(384, 400, 127, 31));
        media.tab_minimize = nk_subimage_id(media.skin, 512, 512, nk_rect(451, 99, 9, 9));
        media.tab_maximize = nk_subimage_id(media.skin, 512, 512, nk_rect(467, 99, 9, 9));
        media.slider = nk_subimage_id(media.skin, 512, 512, nk_rect(418, 33, 11, 14));
        media.slider_hover = nk_subimage_id(media.skin, 512, 512, nk_rect(418, 49, 11, 14));
        media.slider_active = nk_subimage_id(media.skin, 512, 512, nk_rect(418, 64, 11, 14));

        /* window */
        ctx->style.window.background = nk_rgb(204, 204, 204);
        ctx->style.window.fixed_background = nk_style_item_image(media.window);
        ctx->style.window.border_color = nk_rgb(67, 67, 67);
        ctx->style.window.combo_border_color = nk_rgb(67, 67, 67);
        ctx->style.window.contextual_border_color = nk_rgb(67, 67, 67);
        ctx->style.window.menu_border_color = nk_rgb(67, 67, 67);
        ctx->style.window.group_border_color = nk_rgb(67, 67, 67);
        ctx->style.window.tooltip_border_color = nk_rgb(67, 67, 67);
        ctx->style.window.scrollbar_size = nk_vec2(16, 16);
        ctx->style.window.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.window.padding = nk_vec2(8, 4);
        ctx->style.window.border = 3;

        /* window header */
        ctx->style.window.header.normal = nk_style_item_image(media.header);
        ctx->style.window.header.hover = nk_style_item_image(media.header);
        ctx->style.window.header.active = nk_style_item_image(media.header);
        ctx->style.window.header.label_normal = nk_rgb(95, 95, 95);
        ctx->style.window.header.label_hover = nk_rgb(95, 95, 95);
        ctx->style.window.header.label_active = nk_rgb(95, 95, 95);

        /* scrollbar */
        ctx->style.scrollv.normal = nk_style_item_color(nk_rgb(184, 184, 184));
        ctx->style.scrollv.hover = nk_style_item_color(nk_rgb(184, 184, 184));
        ctx->style.scrollv.active = nk_style_item_color(nk_rgb(184, 184, 184));
        ctx->style.scrollv.cursor_normal = nk_style_item_color(nk_rgb(220, 220, 220));
        ctx->style.scrollv.cursor_hover = nk_style_item_color(nk_rgb(235, 235, 235));
        ctx->style.scrollv.cursor_active = nk_style_item_color(nk_rgb(99, 202, 255));
        ctx->style.scrollv.dec_symbol = NK_SYMBOL_NONE;
        ctx->style.scrollv.inc_symbol = NK_SYMBOL_NONE;
        ctx->style.scrollv.show_buttons = nk_true;
        ctx->style.scrollv.border_color = nk_rgb(81, 81, 81);
        ctx->style.scrollv.cursor_border_color = nk_rgb(81, 81, 81);
        ctx->style.scrollv.border = 1;
        ctx->style.scrollv.rounding = 0;
        ctx->style.scrollv.border_cursor = 1;
        ctx->style.scrollv.rounding_cursor = 2;

        /* scrollbar buttons */
        ctx->style.scrollv.inc_button.normal = nk_style_item_image(media.scrollbar_inc_button);
        ctx->style.scrollv.inc_button.hover = nk_style_item_image(media.scrollbar_inc_button_hover);
        ctx->style.scrollv.inc_button.active = nk_style_item_image(media.scrollbar_inc_button_hover);
        ctx->style.scrollv.inc_button.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.inc_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.inc_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.inc_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.inc_button.text_active = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.inc_button.border = 0.0f;

        ctx->style.scrollv.dec_button.normal = nk_style_item_image(media.scrollbar_dec_button);
        ctx->style.scrollv.dec_button.hover = nk_style_item_image(media.scrollbar_dec_button_hover);
        ctx->style.scrollv.dec_button.active = nk_style_item_image(media.scrollbar_dec_button_hover);
        ctx->style.scrollv.dec_button.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.dec_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.dec_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.dec_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.dec_button.text_active = nk_rgba(0, 0, 0, 0);
        ctx->style.scrollv.dec_button.border = 0.0f;

        /* checkbox toggle */
        {
            struct nk_style_toggle* toggle;
            toggle = &ctx->style.checkbox;
            toggle->normal = nk_style_item_image(media.check);
            toggle->hover = nk_style_item_image(media.check);
            toggle->active = nk_style_item_image(media.check);
            toggle->cursor_normal = nk_style_item_image(media.check_cursor);
            toggle->cursor_hover = nk_style_item_image(media.check_cursor);
            toggle->text_normal = nk_rgb(95, 95, 95);
            toggle->text_hover = nk_rgb(95, 95, 95);
            toggle->text_active = nk_rgb(95, 95, 95);
        }

        /* option toggle */
        {
            struct nk_style_toggle* toggle;
            toggle = &ctx->style.option;
            toggle->normal = nk_style_item_image(media.option);
            toggle->hover = nk_style_item_image(media.option);
            toggle->active = nk_style_item_image(media.option);
            toggle->cursor_normal = nk_style_item_image(media.option_cursor);
            toggle->cursor_hover = nk_style_item_image(media.option_cursor);
            toggle->text_normal = nk_rgb(95, 95, 95);
            toggle->text_hover = nk_rgb(95, 95, 95);
            toggle->text_active = nk_rgb(95, 95, 95);
        }

        /* default button */
        ctx->style.button.normal = nk_style_item_image(media.button);
        ctx->style.button.hover = nk_style_item_image(media.button_hover);
        ctx->style.button.active = nk_style_item_image(media.button_active);
        ctx->style.button.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.button.text_normal = nk_rgb(95, 95, 95);
        ctx->style.button.text_hover = nk_rgb(95, 95, 95);
        ctx->style.button.text_active = nk_rgb(95, 95, 95);

        /* default text */
        ctx->style.text.color = nk_rgb(95, 95, 95);

        /* contextual button */
        ctx->style.contextual_button.normal = nk_style_item_color(nk_rgb(206, 206, 206));
        ctx->style.contextual_button.hover = nk_style_item_color(nk_rgb(229, 229, 229));
        ctx->style.contextual_button.active = nk_style_item_color(nk_rgb(99, 202, 255));
        ctx->style.contextual_button.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.contextual_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.contextual_button.text_normal = nk_rgb(95, 95, 95);
        ctx->style.contextual_button.text_hover = nk_rgb(95, 95, 95);
        ctx->style.contextual_button.text_active = nk_rgb(95, 95, 95);

        /* menu button */
        ctx->style.menu_button.normal = nk_style_item_color(nk_rgb(206, 206, 206));
        ctx->style.menu_button.hover = nk_style_item_color(nk_rgb(229, 229, 229));
        ctx->style.menu_button.active = nk_style_item_color(nk_rgb(99, 202, 255));
        ctx->style.menu_button.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.menu_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.menu_button.text_normal = nk_rgb(95, 95, 95);
        ctx->style.menu_button.text_hover = nk_rgb(95, 95, 95);
        ctx->style.menu_button.text_active = nk_rgb(95, 95, 95);

        /* tree */
        ctx->style.tab.text = nk_rgb(95, 95, 95);
        ctx->style.tab.tab_minimize_button.normal = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.tab_minimize_button.hover = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.tab_minimize_button.active = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.tab_minimize_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_minimize_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_minimize_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_minimize_button.text_active = nk_rgba(0, 0, 0, 0);

        ctx->style.tab.tab_maximize_button.normal = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.tab_maximize_button.hover = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.tab_maximize_button.active = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.tab_maximize_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_maximize_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_maximize_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.tab_maximize_button.text_active = nk_rgba(0, 0, 0, 0);

        ctx->style.tab.node_minimize_button.normal = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.node_minimize_button.hover = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.node_minimize_button.active = nk_style_item_image(media.tab_minimize);
        ctx->style.tab.node_minimize_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_minimize_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_minimize_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_minimize_button.text_active = nk_rgba(0, 0, 0, 0);

        ctx->style.tab.node_maximize_button.normal = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.node_maximize_button.hover = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.node_maximize_button.active = nk_style_item_image(media.tab_maximize);
        ctx->style.tab.node_maximize_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_maximize_button.text_normal = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_maximize_button.text_hover = nk_rgba(0, 0, 0, 0);
        ctx->style.tab.node_maximize_button.text_active = nk_rgba(0, 0, 0, 0);

        /* selectable */
        ctx->style.selectable.normal = nk_style_item_color(nk_rgb(206, 206, 206));
        ctx->style.selectable.hover = nk_style_item_color(nk_rgb(206, 206, 206));
        ctx->style.selectable.pressed = nk_style_item_color(nk_rgb(206, 206, 206));
        ctx->style.selectable.normal_active = nk_style_item_color(nk_rgb(185, 205, 248));
        ctx->style.selectable.hover_active = nk_style_item_color(nk_rgb(185, 205, 248));
        ctx->style.selectable.pressed_active = nk_style_item_color(nk_rgb(185, 205, 248));
        ctx->style.selectable.text_normal = nk_rgb(95, 95, 95);
        ctx->style.selectable.text_hover = nk_rgb(95, 95, 95);
        ctx->style.selectable.text_pressed = nk_rgb(95, 95, 95);
        ctx->style.selectable.text_normal_active = nk_rgb(95, 95, 95);
        ctx->style.selectable.text_hover_active = nk_rgb(95, 95, 95);
        ctx->style.selectable.text_pressed_active = nk_rgb(95, 95, 95);

        /* slider */
        ctx->style.slider.normal = nk_style_item_hide();
        ctx->style.slider.hover = nk_style_item_hide();
        ctx->style.slider.active = nk_style_item_hide();
        ctx->style.slider.bar_normal = nk_rgb(156, 156, 156);
        ctx->style.slider.bar_hover = nk_rgb(156, 156, 156);
        ctx->style.slider.bar_active = nk_rgb(156, 156, 156);
        ctx->style.slider.bar_filled = nk_rgb(156, 156, 156);
        ctx->style.slider.cursor_normal = nk_style_item_image(media.slider);
        ctx->style.slider.cursor_hover = nk_style_item_image(media.slider_hover);
        ctx->style.slider.cursor_active = nk_style_item_image(media.slider_active);
        ctx->style.slider.cursor_size = nk_vec2(16.5f, 21);
        ctx->style.slider.bar_height = 1;

        /* progressbar */
        ctx->style.progress.normal = nk_style_item_color(nk_rgb(231, 231, 231));
        ctx->style.progress.hover = nk_style_item_color(nk_rgb(231, 231, 231));
        ctx->style.progress.active = nk_style_item_color(nk_rgb(231, 231, 231));
        ctx->style.progress.cursor_normal = nk_style_item_color(nk_rgb(63, 242, 93));
        ctx->style.progress.cursor_hover = nk_style_item_color(nk_rgb(63, 242, 93));
        ctx->style.progress.cursor_active = nk_style_item_color(nk_rgb(63, 242, 93));
        ctx->style.progress.border_color = nk_rgb(114, 116, 115);
        ctx->style.progress.padding = nk_vec2(0, 0);
        ctx->style.progress.border = 2;
        ctx->style.progress.rounding = 1;

        /* combo */
        ctx->style.combo.normal = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.hover = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.active = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.border_color = nk_rgb(95, 95, 95);
        ctx->style.combo.label_normal = nk_rgb(95, 95, 95);
        ctx->style.combo.label_hover = nk_rgb(95, 95, 95);
        ctx->style.combo.label_active = nk_rgb(95, 95, 95);
        ctx->style.combo.border = 1;
        ctx->style.combo.rounding = 1;

        /* combo button */
        ctx->style.combo.button.normal = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.button.hover = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.button.active = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.combo.button.text_background = nk_rgb(216, 216, 216);
        ctx->style.combo.button.text_normal = nk_rgb(95, 95, 95);
        ctx->style.combo.button.text_hover = nk_rgb(95, 95, 95);
        ctx->style.combo.button.text_active = nk_rgb(95, 95, 95);

        /* property */
        ctx->style.property.normal = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.hover = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.active = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.border_color = nk_rgb(81, 81, 81);
        ctx->style.property.label_normal = nk_rgb(95, 95, 95);
        ctx->style.property.label_hover = nk_rgb(95, 95, 95);
        ctx->style.property.label_active = nk_rgb(95, 95, 95);
        ctx->style.property.sym_left = NK_SYMBOL_TRIANGLE_LEFT;
        ctx->style.property.sym_right = NK_SYMBOL_TRIANGLE_RIGHT;
        ctx->style.property.rounding = 10;
        ctx->style.property.border = 1;

        /* edit */
        ctx->style.edit.normal = nk_style_item_color(nk_rgb(240, 240, 240));
        ctx->style.edit.hover = nk_style_item_color(nk_rgb(240, 240, 240));
        ctx->style.edit.active = nk_style_item_color(nk_rgb(240, 240, 240));
        ctx->style.edit.border_color = nk_rgb(62, 62, 62);
        ctx->style.edit.cursor_normal = nk_rgb(99, 202, 255);
        ctx->style.edit.cursor_hover = nk_rgb(99, 202, 255);
        ctx->style.edit.cursor_text_normal = nk_rgb(95, 95, 95);
        ctx->style.edit.cursor_text_hover = nk_rgb(95, 95, 95);
        ctx->style.edit.text_normal = nk_rgb(95, 95, 95);
        ctx->style.edit.text_hover = nk_rgb(95, 95, 95);
        ctx->style.edit.text_active = nk_rgb(95, 95, 95);
        ctx->style.edit.selected_normal = nk_rgb(99, 202, 255);
        ctx->style.edit.selected_hover = nk_rgb(99, 202, 255);
        ctx->style.edit.selected_text_normal = nk_rgb(95, 95, 95);
        ctx->style.edit.selected_text_hover = nk_rgb(95, 95, 95);
        ctx->style.edit.border = 1;
        ctx->style.edit.rounding = 2;

        /* property buttons */
        ctx->style.property.dec_button.normal = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.dec_button.hover = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.dec_button.active = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.dec_button.text_background = nk_rgba(0, 0, 0, 0);
        ctx->style.property.dec_button.text_normal = nk_rgb(95, 95, 95);
        ctx->style.property.dec_button.text_hover = nk_rgb(95, 95, 95);
        ctx->style.property.dec_button.text_active = nk_rgb(95, 95, 95);
        ctx->style.property.inc_button = ctx->style.property.dec_button;

        /* property edit */
        ctx->style.property.edit.normal = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.edit.hover = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.edit.active = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.property.edit.border_color = nk_rgba(0, 0, 0, 0);
        ctx->style.property.edit.cursor_normal = nk_rgb(95, 95, 95);
        ctx->style.property.edit.cursor_hover = nk_rgb(95, 95, 95);
        ctx->style.property.edit.cursor_text_normal = nk_rgb(216, 216, 216);
        ctx->style.property.edit.cursor_text_hover = nk_rgb(216, 216, 216);
        ctx->style.property.edit.text_normal = nk_rgb(95, 95, 95);
        ctx->style.property.edit.text_hover = nk_rgb(95, 95, 95);
        ctx->style.property.edit.text_active = nk_rgb(95, 95, 95);
        ctx->style.property.edit.selected_normal = nk_rgb(95, 95, 95);
        ctx->style.property.edit.selected_hover = nk_rgb(95, 95, 95);
        ctx->style.property.edit.selected_text_normal = nk_rgb(216, 216, 216);
        ctx->style.property.edit.selected_text_hover = nk_rgb(216, 216, 216);

        /* chart */
        ctx->style.chart.background = nk_style_item_color(nk_rgb(216, 216, 216));
        ctx->style.chart.border_color = nk_rgb(81, 81, 81);
        ctx->style.chart.color = nk_rgb(95, 95, 95);
        ctx->style.chart.selected_color = nk_rgb(255, 0, 0);
        ctx->style.chart.border = 1;
    }

    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) running = false;
            nk_sdl_handle_event(&evt);
        }
        nk_sdl_handle_grab(); /* optional grabbing behavior */
        nk_input_end(ctx);

        // Skinning gui demo
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 300, 400),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
        {
            int i;
            float id;
            static int slider = 10;
            static int field_len;
            static nk_size prog_value = 60;
            static int current_weapon = 0;
            static char field_buffer[64];
            static float pos;
            static const char* weapons[] = { "Fist","Pistol","Shotgun","Plasma","BFG" };
            const float step = (2 * 3.141592654f) / 32;

            nk_layout_row_static(ctx, 30, 120, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Label", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 30, 2);
            nk_check_label(ctx, "inactive", 0);
            nk_check_label(ctx, "active", 1);
            nk_option_label(ctx, "active", 1);
            nk_option_label(ctx, "inactive", 0);

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_slider_int(ctx, 0, &slider, 16, 1);
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_progress(ctx, &prog_value, 100, NK_MODIFIABLE);

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_edit_string(ctx, NK_EDIT_FIELD, field_buffer, &field_len, 64, nk_filter_default);
            nk_property_float(ctx, "#X:", -1024.0f, &pos, 1024.0f, 1, 1);
            current_weapon = nk_combo(ctx, weapons, LEN(weapons), current_weapon, 25, nk_vec2(nk_widget_width(ctx), 200));

            nk_layout_row_dynamic(ctx, 100, 1);
            if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255, 0, 0), nk_rgb(150, 0, 0), 32, 0.0f, 1.0f)) {
                nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 0, 255), nk_rgb(0, 0, 150), 32, -1.0f, 1.0f);
                nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0, 255, 0), nk_rgb(0, 150, 0), 32, -1.0f, 1.0f);
                for (id = 0, i = 0; i < 32; ++i) {
                    nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                    nk_chart_push_slot(ctx, (float)cos(id), 1);
                    nk_chart_push_slot(ctx, (float)sin(id), 2);
                    id += step;
                }
            }
            nk_chart_end(ctx);

            nk_layout_row_dynamic(ctx, 250, 1);
            if (nk_group_begin(ctx, "Standard", NK_WINDOW_BORDER | NK_WINDOW_BORDER))
            {
                if (nk_tree_push(ctx, NK_TREE_NODE, "Window", NK_MAXIMIZED)) {
                    static int selected[8];
                    if (nk_tree_push(ctx, NK_TREE_NODE, "Next", NK_MAXIMIZED)) {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        for (i = 0; i < 4; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_LEFT, &selected[i]);
                        nk_tree_pop(ctx);
                    }
                    if (nk_tree_push(ctx, NK_TREE_NODE, "Previous", NK_MAXIMIZED)) {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        for (i = 4; i < 8; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected" : "Unselected", NK_TEXT_LEFT, &selected[i]);
                        nk_tree_pop(ctx);
                    }
                    nk_tree_pop(ctx);
                }
                nk_group_end(ctx);
            }
        }
        nk_end(ctx);

        // Old gui demo
        /* GUI */
        if (nk_begin(ctx, "Demo 2", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum { EASY, HARD };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                printf("button pressed!\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
#ifdef INCLUDE_CALCULATOR
        calculator(ctx);
#endif

        // including the canvas makes the gui weird...
#ifdef INCLUDE_CANVAS
        // canvas(ctx);
#endif

#ifdef INCLUDE_OVERVIEW
        overview(ctx);
#endif

#ifdef INCLUDE_CONFIGURATOR
        style_configurator(ctx, color_table);
#endif

#ifdef INCLUDE_NODE_EDITOR
        node_editor(ctx);
#endif
        /* ----------------------------------------- */


        /* Draw */
        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(win);
    }

    glDeleteTextures(1, (const GLuint*)&media.skin);
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
#endif
