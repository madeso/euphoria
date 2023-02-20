#include "log/log.h"

#include "core/mat4.h"
#include "core/random.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/viewport.h"

#include "render/init.h"
#include "render/materialshader.h"
#include "render/texturecache.h"
#include "render/texture.h"
#include "render/materialshadercache.h"
#include "render/viewporthandler.h"

#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_extra.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"

#include "editor/browser.h"
#include "editor/scimed.h"

#include "SDL.h"
#include <memory>

#include "imgui_stdlib.h"

#include "gaf_game.h"
#include "gaf_world.h"
#include "gaf_enum.h"

#include "gaf_imgui_game.h"
#include "gaf_imgui_world.h"
#include "gaf_imgui_enum.h"
#include "gaf_imgui_scalingsprite.h"

#include "gaf_rapidjson_game.h"
#include "gaf_rapidjson_world.h"
#include "gaf_rapidjson_enum.h"


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::editor;

ImVec2
operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2 {lhs.x + rhs.x, lhs.y + rhs.y};
}

struct StyleData
{
    ScimedConfig scc;
    CanvasConfig cc;
};

struct GenericWindow
{
    std::string name;
    bool open = true;

    GenericWindow() = default;
    NONCOPYABLE(GenericWindow);

    virtual ~GenericWindow() = default;

    virtual void
    run(StyleData* style_data) = 0;
};

template <typename CreateWindowFunction>
void
open_or_focus_window
(
    std::vector<std::shared_ptr<GenericWindow>>* windows,
    const std::string& title_name,
    CreateWindowFunction create_window_function
)
{
    const auto found = find_first
    (
        *windows,
        [&](std::shared_ptr<GenericWindow>& wind) -> bool
        {
            return wind->name == title_name;
        }
    );

    if(found == windows->end())
    {
        auto the_window = create_window_function();
        the_window->name = title_name;
        windows->emplace_back(the_window);
    }
    else
    {
        ImGui::SetNextWindowFocus();
        ImGui::Begin(title_name.c_str());
        ImGui::End();
    }
}


struct ScimedWindow : public GenericWindow
{
    euphoria::editor::Scimed scimed;

    void
    run(StyleData* style_data) override
    {
        scimed.run(style_data->cc, style_data->scc);
    }
};

template <typename TEditorFunction, typename TData>
struct GenericEditorWindow : public GenericWindow
{
    TData data;
    TEditorFunction edit_function;

    GenericEditorWindow(TData d, TEditorFunction edit)
        : data(d)
        , edit_function(edit)
    {
    }

    void
    run(StyleData*) override
    {
        edit_function(data);
    }
};

template <typename TEditorFunction, typename TData>
std::shared_ptr<GenericEditorWindow<TEditorFunction, TData>>
create_generic_window(TData data, TEditorFunction edit)
{
    return std::make_shared<GenericEditorWindow<TEditorFunction, TData>>(data, edit);
}

void
color_edit4(const char* label, ImU32* color)
{
    ImVec4 temp = ImColor {*color};

    if(ImGui::ColorEdit4(label, &temp.x))
    {
        *color = ImColor(temp);
    }
}

struct StyleEditorWindow : GenericWindow
{
    void
    run(StyleData* s) override
    {
        color_edit4("Background color", &s->cc.background_color);
        color_edit4("Grid color", &s->cc.grid_color);
        color_edit4("Split color", &s->scc.split_color);
        color_edit4("Sizer color", &s->scc.sizer_color);

        ImGui::InputFloat("Grid Size", &s->cc.grid_size, 1.0f, 5.0f);
        ImGui::InputFloat("Zoom speed", &s->cc.zoom_speed);
        ImGui::InputInt("Anchor size", &s->scc.anchor_size);
        ImGui::InputInt("Sizer distance", &s->scc.sizer_distance);
        ImGui::InputInt("Sizer text distance", &s->scc.sizer_text_distance);

        if(ImGui::Button("Set Default"))
        {
            *s = StyleData {};
        }
    }
};

struct TextEditorWindow : GenericWindow
{
    // todo(Gustav): look into using a more complex text editor?
    // https://github.com/BalazsJako/ImGuiColorTextEdit
    std::string text;

    explicit TextEditorWindow(const std::string& str)
        : text(str)
    {
    }

    void
    run(StyleData*) override
    {
        ImGui::InputTextMultiline("##text", &text, ImVec2 {-1, -1});
    }
};

using Windows = std::vector<std::shared_ptr<GenericWindow>>;

void
open_or_focus_style_editor(Windows* windows)
{
    open_or_focus_window
    (
        windows,
        "Style editor",
        []()
        {
            return std::make_shared<StyleEditorWindow>();
        }
    );
}

void
open_or_focus_text_file
(
    Windows* windows,
    const vfs::FilePath& path,
    vfs::FileSystem* fs
)
{
    open_or_focus_window
    (
        windows,
        "File: {}"_format(path),
        [&]()
        {
            auto str = fs->read_file_to_string(path);
            if(!str)
            {
                str = "Failed to open {}"_format(path);
            }
            return std::make_shared<TextEditorWindow>(*str);
        }
    );
}

struct ScalingSpriteCache
    : Cache
    <
        vfs::FilePath,
        scalingsprite::ScalingSprite,
        ScalingSpriteCache
    >
{
    std::shared_ptr<scalingsprite::ScalingSprite>
    create(const vfs::FilePath&) // NOLINT load from filename
    {
        // todo(Gustav): load from filename
        auto ss = std::make_shared<scalingsprite::ScalingSprite>();
        return ss;
    }
};

void
load_file
(
    Scimed* scimed,
    TextureCache* cache,
    ScalingSpriteCache* shader_cache,
    const vfs::FilePath& path
)
{
    scimed->texture = cache->get_texture(path);
    scimed->scaling = shader_cache->get(path.extend_extension_copy("json"));

    if(scimed->texture)
    {
        if(scimed->scaling->rows.empty())
        {
            scimed->scaling->rows.emplace_back(scimed->texture->height);
        }

        if(scimed->scaling->cols.empty())
        {
            scimed->scaling->cols.emplace_back(scimed->texture->width);
        }
    }
}

void
open_or_focus_scimed
(
    Windows* windows,
    const vfs::FilePath& file,
    TextureCache* tc,
    ScalingSpriteCache* sc
)
{
    open_or_focus_window
    (
        windows,
        "scimed: {}"_format(file),
        [&]() -> std::shared_ptr<GenericWindow>
        {
            auto scimed = std::make_shared<ScimedWindow>();
            load_file(&scimed->scimed, tc, sc, file);
            return scimed;
        }
    );
}

void
open_or_focus_scimed_editor
(
    Windows* windows,
    const vfs::FilePath& path,
    ScalingSpriteCache* sc
)
{
    auto file = path;
    if(!ends_with(file.path, ".json"))
    {
        file = file.extend_extension_copy("json");
    }
    open_or_focus_window
    (
        windows,
        "scimed editor: {}"_format(file),
        [&]() -> std::shared_ptr<GenericWindow>
        {
            auto sprite = sc->get(file);
            return create_generic_window(sprite, [](auto sp)
            {
                scalingsprite::RunImgui(sp.get());
            });
        }
    );
}

template <typename T, typename TRun, typename TReadJsonFunction>
void
open_or_focus_on_generic_window
(
    Windows* windows,
    const vfs::FilePath& path,
    vfs::FileSystem* fs,
    const std::string& title,
    TReadJsonFunction read_json,
    TRun run_function
)
{
    open_or_focus_window
    (
        windows,
        "{}: {}"_format(title, path),
        [=]() -> std::shared_ptr<GenericWindow>
        {
            auto window = create_generic_window
            (
                T{},
                [=](T &t)
                {
                    run_function(&t);
                }
            );
            // todo(Gustav): don't open window if loading failed...
            window->data = euphoria::core::get_default_but_log_errors
            (
                euphoria::core::read_json_file_to_gaf_struct<T>(fs, path, read_json)
            );
            return window;
        }
    );
}


struct FileHandler
{
    std::string context_menu;

    explicit FileHandler(const std::string& menu)
        : context_menu(menu)
    {
    }

    NONCOPYABLE(FileHandler);

    virtual ~FileHandler() = default;

    virtual bool
    matches(const vfs::FilePath& path) = 0;

    virtual void
    open(Windows* windows, const vfs::FilePath& path) = 0;
};

template <typename TMatchFunction, typename TOpenFunction>
struct GenericFileHandler : public FileHandler
{
    TMatchFunction match_function;
    TOpenFunction open_function;
    GenericFileHandler
    (
        const std::string& menu,
        TMatchFunction match,
        TOpenFunction open
    )
        : FileHandler(menu)
        , match_function(match)
        , open_function(open)
    {
    }

    bool
    matches(const vfs::FilePath& path) override
    {
        return match_function(path);
    }

    void
    open(Windows* windows, const vfs::FilePath& path) override
    {
        return open_function(windows, path);
    }
};


template <typename TMatchFunction, typename TOpenFunction>
std::shared_ptr<FileHandler>
create_handler(const std::string& menu, TMatchFunction match, TOpenFunction open)
{
    return std::make_shared<GenericFileHandler<TMatchFunction, TOpenFunction>>
    (
        menu,
        match,
        open
    );
}

struct FileHandlerList
{
    std::vector<std::shared_ptr<FileHandler>> handlers;

    void
    add(std::shared_ptr<FileHandler> handler)
    {
        handlers.emplace_back(handler);
    }

    bool
    open(Windows* windows, const vfs::FilePath& path)
    {
        // todo(Gustav): replace with find and action instead
        for(auto& handler: handlers) // NOLINT
        {
            if(handler->matches(path))
            {
                handler->open(windows, path);
                return true;
            }
        }

        return false;
    }

    void
    run_imgui_selectable(Windows* windows, const std::optional<vfs::FilePath>& path)
    {
        // todo(Gustav): come up with a better name for this function
        for(auto& handler: handlers)
        {
            if
            (
                imgui::selectable_or_disabled
                (
                    path.has_value(),
                    handler->context_menu.c_str()
                )
            )
            {
                handler->open(windows, path.value());
            }
        }
    }
};


int
main(int argc, char** argv)
{
    EUPH_INIT_LOGGING();
    Engine engine;

    if (const auto r = engine.setup(argparse::NameAndArguments::extract(argc, argv)); r != 0)
    {
        return r;
    }

    int window_width = 1280;
    int window_height = 720;

    if(
        !engine.create_window
        (
            "Euphoria Editor",
            window_width,
            window_height,
            true
        )
    )
    {
        return -1;
    }

    TextureCache texture_cache {engine.file_system.get()};
    ScalingSpriteCache sprite_cache;

    bool running = true;

    FileBrowser browser {engine.file_system.get()};
    browser.refresh();
    std::vector<std::shared_ptr<GenericWindow>> windows;
    ::StyleData style_data;
    FileHandlerList file_types;

    //////////////////////////////////////////////////////////////////////////////
    // File types

    file_types.add
    (
        create_handler
        (
            "Open with Game Data",
            [](const vfs::FilePath &file) -> bool
            {
                return file.path == "~/gamedata.json";
            },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<game::Game>
                (
                    wins,
                    file,
                    engine.file_system.get(),
                    "Game",
                    game::ReadJsonGame,
                    [](auto *s)
                    { game::RunImgui(s); }
                );
            }
        )
    );

    file_types.add
    (
        create_handler
        (
            "Open with World Editor",
            [](const vfs::FilePath &file) -> bool
            {
                return file.path == "~/world.json";
            },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<::world::World>
                (
                    wins,
                    file,
                    engine.file_system.get(),
                    "World",
                    world::ReadJsonWorld,
                    [](auto *s)
                    {
                        ::world::RunImgui(s);
                    }
                );
            }
        )
    );
    file_types.add
    (
        create_handler
        (
            "Open with Enum Editor",
            [](const vfs::FilePath &) -> bool
            { return false; },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<enumlist::Enumroot>
                (
                    wins,
                    file,
                    engine.file_system.get(),
                    "Enums",
                    enumlist::ReadJsonEnumroot,
                    [](auto* s)
                    {
                        enumlist::RunImgui(s);
                    }
                );
            }
        )
    );

    file_types.add
    (
        create_handler
        (
            "Open with text editor",
            [](const vfs::FilePath &file) -> bool
            {
                return ends_with(file.path, ".json") || ends_with(file.path, ".js");
            },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_text_file(wins, file, engine.file_system.get());
            }
        )
    );

    file_types.add
    (
        create_handler
        (
            "Open with scimed editor",
            [](const vfs::FilePath &file) -> bool
            {
                return file.get_extension() == "png";
            },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_scimed(wins, file, &texture_cache, &sprite_cache);
            }
        )
    );

    file_types.add
    (
        create_handler
        (
            "Open with auto scimed editor",
            [](const vfs::FilePath &) -> bool
            { return false; },
            [&](Windows* wins, const vfs::FilePath &file)
            {
                open_or_focus_scimed_editor(wins, file, &sprite_cache);
            }
        )
    );

    auto viewport_handler = euphoria::render::ViewportHandler
    {
        engine.init.get(),
        nullptr
    };
    viewport_handler.set_size(window_width, window_height);

    //////////////////////////////////////////////////////////////////////////////
    // main loop

    auto handle_events = [&]()
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            imgui::process_imgui_events(&e);

            if(engine.on_resize(e, &window_width, &window_height))
            {
                viewport_handler.set_size(window_width, window_height);
            }

            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }
    };

    while(running)
    {
        handle_events();

        imgui::start_new_frame();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", "Ctrl+Q"))
                {
                    running = false;
                }
                if(ImGui::MenuItem("Style editor"))
                {
                    open_or_focus_style_editor(&windows);
                }
                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Window"))
            {
                for(const auto& win: windows)
                {
                    if(ImGui::MenuItem(win->name.c_str()))
                    {
                        ImGui::SetNextWindowFocus();
                        ImGui::Begin(win->name.c_str());
                        ImGui::End();
                    }
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        if(ImGui::Begin("Solution explorer"))
        {
            if(ImGui::BeginCombo("test combo", "[val]"))
            {
                const float small = 30;
                const float big = 60;
                const auto sizer = [=](int id) -> float {
                    return id == 1 ? big : small;
                };
                for(int yi = 0; yi < 3; yi += 1)
                {
                    ImGui::PushID(yi);
                    for(int xi = 0; xi < 3; xi += 1)
                    {
                        ImGui::PushID(xi);
                        if(ImGui::Button("a", ImVec2 {sizer(xi), sizer(yi)}))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                        if(xi < 2)
                        {
                            ImGui::SameLine();
                        }
                        ImGui::PopID();
                    }
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            if(browser.run())
            {
                const auto file = browser.get_selected_file();
                if(file.has_value())
                {
                    file_types.open(&windows, file.value());
                }
            }
            if(ImGui::BeginPopupContextItem("browser popup"))
            {
                const auto file = browser.get_selected_file();
                file_types.run_imgui_selectable(&windows, file);
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        for(auto& win: windows)
        {
            ImGui::SetNextWindowSize(ImVec2 {300, 300}, ImGuiCond_FirstUseEver);
            if(ImGui::Begin(win->name.c_str(), &win->open))
            {
                win->run(&style_data);
            }
            ImGui::End();
        }

        // ImGui::ShowMetricsWindow();

        engine.init->clear_screen(NamedColor::light_gray);
        imgui::imgui_render();

        remove_matching
        (
            &windows,
            [](const std::shared_ptr<GenericWindow>& window)
            {
                return !window->open;
            }
        );

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
