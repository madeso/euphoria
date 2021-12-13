#include "core/mat4.h"
#include "core/random.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"

#include "render/init.h"
#include "render/materialshader.h"
#include "render/texturecache.h"
#include "render/texture.h"
#include "render/viewport.h"
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

#include "gaf_game.h"
#include "gaf_world.h"
#include "gaf_enum.h"

#include "gaf_imgui_game.h"
#include "gaf_imgui_world.h"
#include "gaf_imgui_enum.h"
#include "gaf_imgui_scalingsprite.h"

#include "gaf_pugixml_game.h"
#include "gaf_pugixml_world.h"
#include "gaf_pugixml_enum.h"


using namespace euphoria::core;
using namespace euphoria::render;
using namespace euphoria::window;
using namespace euphoria::editor;

ImVec2
operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2 {lhs.x + rhs.x, lhs.y + rhs.y};
}

struct style_data
{
    scimed_config scc;
    canvas_config cc;
};

struct generic_window
{
    std::string name;
    bool open = true;

    generic_window() = default;
    NONCOPYABLE(generic_window);

    virtual ~generic_window() = default;

    virtual void
    run(style_data* style_data) = 0;
};

template <typename CreateWindowFunction>
void
open_or_focus_window
(
    std::vector<std::shared_ptr<generic_window>>* windows,
    const std::string& title_name,
    CreateWindowFunction create_window_function
)
{
    const auto found = find_first
    (
        *windows,
        [&](std::shared_ptr<generic_window>& wind) -> bool
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


struct scimed_window : public generic_window
{
    euphoria::editor::scimed scimed;

    void
    run(style_data* style_data) override
    {
        scimed.run(style_data->cc, style_data->scc);
    }
};

template <typename TEditorFunction, typename TData>
struct generic_editor_window : public generic_window
{
    TData data;
    TEditorFunction edit_function;

    generic_editor_window(TData d, TEditorFunction edit)
        : data(d)
        , edit_function(edit)
    {
    }

    void
    run(style_data*) override
    {
        edit_function(data);
    }
};

template <typename TEditorFunction, typename TData>
std::shared_ptr<generic_editor_window<TEditorFunction, TData>>
create_generic_window(TData data, TEditorFunction edit)
{
    return std::make_shared<generic_editor_window<TEditorFunction, TData>>(data, edit);
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

struct style_editor_window : generic_window
{
    void
    run(style_data* s) override
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
            *s = style_data {};
        }
    }
};

struct text_editor_window : generic_window
{
    // todo(Gustav): look into using a more complex text editor?
    // https://github.com/BalazsJako/ImGuiColorTextEdit
    std::vector<char> buffer;

    explicit text_editor_window(const std::string& str)
    {
        const auto length = str.size();
        buffer.reserve(length + 1);
        strncpy(&buffer[0], str.c_str(), length);
        buffer[length] = 0;
    }

    void
    run(style_data*) override
    {
        ImGui::InputTextMultiline("", &buffer[0], buffer.capacity(), ImVec2 {-1, -1});
        if(buffer.size() + 2 > buffer.capacity())
        {
            buffer.push_back(0);
            buffer.push_back(0);
        }
    }
};

using Windows = std::vector<std::shared_ptr<generic_window>>;

void
open_or_focus_style_editor(Windows* windows)
{
    open_or_focus_window
    (
        windows,
        "Style editor",
        []()
        {
            return std::make_shared<style_editor_window>();
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
        StringBuilder {} << "File: " << path,
        [&]()
        {
            std::string str;
            if(!fs->read_file_to_string(path, &str))
            {
                str = StringBuilder {} << "Failed to open " << path;
            }
            return std::make_shared<text_editor_window>(str);
        }
    );
}

struct scaling_sprite_cache
    : Cache
    <
        vfs::FilePath,
        scalingsprite::ScalingSprite,
        scaling_sprite_cache
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
    scimed* scimed,
    TextureCache* cache,
    scaling_sprite_cache* shader_cache,
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
    scaling_sprite_cache* sc
)
{
    open_or_focus_window
    (
        windows,
        StringBuilder {} << "scimed: " << file,
        [&]() -> std::shared_ptr<generic_window>
        {
            auto scimed = std::make_shared<scimed_window>();
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
    scaling_sprite_cache* sc
)
{
    auto file = path;
    if(!ends_with(file.path, ".xml"))
    {
        file = file.extend_extension_copy("xml");
    }
    open_or_focus_window
    (
        windows,
        StringBuilder {} << "scimed editor: " << file,
        [&]() -> std::shared_ptr<generic_window>
        {
            auto sprite = sc->get(file);
            return create_generic_window(sprite, [](auto sprite)
            {
                scalingsprite::RunImgui(sprite.get());
            });
        }
    );
}

template <typename T, typename TRun, typename TReadXmlFunction>
void
open_or_focus_on_generic_window
(
    Windows* windows,
    const vfs::FilePath& path,
    vfs::FileSystem* fs,
    const std::string& title,
    TReadXmlFunction read_xml,
    TRun run_function
)
{
    open_or_focus_window
    (
        windows,
        StringBuilder {} << title << ": " << path,
        [=]() -> std::shared_ptr<generic_window>
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
                euphoria::core::read_xml_file_to_gaf_struct<T>(fs, path, read_xml)
            );
            return window;
        }
    );
}


struct file_handler
{
    std::string context_menu;

    explicit file_handler(const std::string& menu)
        : context_menu(menu)
    {
    }

    NONCOPYABLE(file_handler);

    virtual ~file_handler() = default;

    virtual bool
    matches(const vfs::FilePath& path) = 0;

    virtual void
    open(Windows* windows, const vfs::FilePath& path) = 0;
};

template <typename TMatchFunction, typename TOpenFunction>
struct generic_file_handler : public file_handler
{
    TMatchFunction match_function;
    TOpenFunction open_function;
    generic_file_handler
    (
        const std::string& menu,
        TMatchFunction match,
        TOpenFunction open
    )
        : file_handler(menu)
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
std::shared_ptr<file_handler>
create_handler(const std::string& menu, TMatchFunction match, TOpenFunction open)
{
    return std::make_shared<generic_file_handler<TMatchFunction, TOpenFunction>>
    (
        menu,
        match,
        open
    );
}

struct file_handler_list
{
    std::vector<std::shared_ptr<file_handler>> handlers;

    void
    add(std::shared_ptr<file_handler> handler)
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
    engine engine;

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
    scaling_sprite_cache sprite_cache;

    bool running = true;

    file_browser browser {engine.file_system.get()};
    browser.refresh();
    std::vector<std::shared_ptr<generic_window>> windows;
    ::style_data style_data;
    file_handler_list file_types;

    //////////////////////////////////////////////////////////////////////////////
    // File types

    file_types.add
    (
        create_handler
        (
            "Open with Game Data",
            [](const vfs::FilePath &file) -> bool
            {
                return file.path == "~/gamedata.xml";
            },
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<game::Game>
                (
                    windows,
                    file,
                    engine.file_system.get(),
                    "Game",
                    game::ReadXmlElementGame,
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
                return file.path == "~/world.xml";
            },
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<::world::World>
                (
                    windows,
                    file,
                    engine.file_system.get(),
                    "World",
                    world::ReadXmlElementWorld,
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
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_on_generic_window<enumlist::Enumroot>
                (
                    windows,
                    file,
                    engine.file_system.get(),
                    "Enums",
                    enumlist::ReadXmlElementEnumroot,
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
                return ends_with(file.path, ".xml") || ends_with(file.path, ".js");
            },
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_text_file(windows, file, engine.file_system.get());
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
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_scimed(windows, file, &texture_cache, &sprite_cache);
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
            [&](Windows *windows, const vfs::FilePath &file)
            {
                open_or_focus_scimed_editor(windows, file, &sprite_cache);
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

        engine.imgui->start_new_frame();

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
            [](const std::shared_ptr<generic_window>& window)
            {
                return !window->open;
            }
        );

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
