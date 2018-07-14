#include <core/mat4.h>
#include "core/draw.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/filesystem.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/filesystemimagegenerator.h"
#include "core/path.h"
#include "core/os.h"
#include "core/range.h"
#include "core/camera.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"

#include <render/init.h>
#include <render/debuggl.h>
#include <render/materialshader.h>
#include <render/compiledmesh.h>
#include <render/texturecache.h>
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/materialshadercache.h"
#include "render/defaultfiles.h"

#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_ext.h"
#include "window/fpscontroller.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"

#include "editor/browser.h"
#include "editor/scimed.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>

#include "gaf_game.h"
#include "gaf_world.h"
#include "gaf_enum.h"

LOG_SPECIFY_DEFAULT_LOGGER("editor")

ImVec2
operator+(const ImVec2& lhs, const ImVec2& rhs)
{
  return ImVec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

struct StyleData
{
  ScimedConfig scc = ScimedConfig{};
  CanvasConfig cc  = CanvasConfig{};
};

struct GenericWindow
{
  std::string name;
  bool        open = true;

  virtual void
  Run(StyleData* style_data) = 0;
};

template <typename CreateWindowFunction>
void
OpenOrFocusWindow(
    std::vector<std::shared_ptr<GenericWindow>>* windows,
    const std::string&                           title_name,
    CreateWindowFunction                         create_window_function)
{
  const auto found =
      Search(*windows, [&](std::shared_ptr<GenericWindow>& wind) -> bool {
        return wind->name == title_name;
      });

  if(found == windows->end())
  {
    auto the_window  = create_window_function();
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
  Scimed scimed;

  void
  Run(StyleData* style_data) override
  {
    scimed.Run(style_data->cc, style_data->scc);
  }
};

template <typename TEditorFunction, typename TData>
struct GenericEditorWindow : public GenericWindow
{
  TData           data;
  TEditorFunction edit_function;

  GenericEditorWindow(TData d, TEditorFunction edit)
      : data(d)
      , edit_function(edit)
  {
  }

  void
  Run(StyleData* style_data) override
  {
    edit_function(data);
  }
};

template <typename TEditorFunction, typename TData>
std::shared_ptr<GenericEditorWindow<TEditorFunction, TData>>
CreateGenericWindow(TData data, TEditorFunction edit)
{
  return std::make_shared<GenericEditorWindow<TEditorFunction, TData>>(
      data, edit);
}

void
ColorEdit4(const char* label, ImU32* color)
{
  ImVec4 temp = ImColor{*color};

  if(ImGui::ColorEdit4(label, &temp.x))
  {
    *color = ImColor(temp);
  }
}

struct StyleEditorWindow : GenericWindow
{
  void
  Run(StyleData* s) override
  {
    ColorEdit4("Background color", &s->cc.background_color);
    ColorEdit4("Grid color", &s->cc.grid_color);
    ColorEdit4("Split color", &s->scc.split_color);
    ColorEdit4("Sizer color", &s->scc.sizer_color);

    ImGui::InputFloat("Grid Size", &s->cc.grid_size, 1.0f, 5.0f);
    ImGui::InputFloat("Zoom speed", &s->cc.zoom_speed);
    ImGui::InputInt("Anchor size", &s->scc.anchor_size);
    ImGui::InputInt("Sizer distance", &s->scc.sizer_distance);
    ImGui::InputInt("Sizer text distance", &s->scc.sizer_text_distance);

    if(ImGui::Button("Set Default"))
    {
      *s = StyleData{};
    }
  }
};

struct TextEditorWindow : GenericWindow
{
  // todo: look into using a more complex text editor?
  // https://github.com/BalazsJako/ImGuiColorTextEdit
  std::vector<char> buffer;

  explicit TextEditorWindow(const std::string& str)
  {
    const auto length = str.size();
    buffer.reserve(length + 1);
    strncpy(&buffer[0], str.c_str(), length);
    buffer[length] = 0;
  }

  void
  Run(StyleData* s) override
  {
    ImGui::InputTextMultiline(
        "", &buffer[0], buffer.capacity(), ImVec2{-1, -1});
    if(buffer.size() + 2 > buffer.capacity())
    {
      buffer.push_back(0);
      buffer.push_back(0);
    }
  }
};

using Windows = std::vector<std::shared_ptr<GenericWindow>>;

void
OpenOrFocusStyleEditor(Windows* windows)
{
  OpenOrFocusWindow(windows, "Style editor", []() {
    return std::make_shared<StyleEditorWindow>();
  });
}

void
OpenOrFocusTextFile(Windows* windows, const std::string& path, FileSystem* fs)
{
  OpenOrFocusWindow(windows, Str{} << "File: " << path, [&]() {
    std::string str;
    if(!fs->ReadFileToString(path, &str))
    {
      str = Str{} << "Failed to open " << path;
    }
    return std::make_shared<TextEditorWindow>(str);
  });
}

struct ScalingSpriteCache : public Cache<
                                std::string,
                                scalingsprite::ScalingSprite,
                                ScalingSpriteCache>
{
  std::shared_ptr<scalingsprite::ScalingSprite>
  Create(const std::string& path)
  {
    // todo: load from filename
    return std::make_shared<scalingsprite::ScalingSprite>();
  }
};

void
LoadFile(
    Scimed*             scimed,
    TextureCache*       cache,
    ScalingSpriteCache* scache,
    FileSystem*         fs,
    const std::string&  path)
{
  scimed->texture = cache->GetTexture(path);
  scimed->scaling = scache->Get(path + ".json");

  if(scimed->texture)
  {
    if(scimed->scaling->rows.empty())
    {
      scimed->scaling->rows.emplace_back(scimed->texture->GetHeight());
    }

    if(scimed->scaling->cols.empty())
    {
      scimed->scaling->cols.emplace_back(scimed->texture->GetWidth());
    }
  }
}

void
OpenOrFocusScimed(
    Windows*            windows,
    const std::string&  file,
    FileSystem*         fs,
    TextureCache*       tc,
    ScalingSpriteCache* sc)
{
  OpenOrFocusWindow(
      windows,
      Str{} << "Scimed: " << file,
      [&]() -> std::shared_ptr<GenericWindow> {
        auto scimed = std::make_shared<ScimedWindow>();
        LoadFile(&scimed->scimed, tc, sc, fs, file);
        return scimed;
      });
}

void
OpenOrFocusScimedEditior(
    Windows*            windows,
    const std::string&  path,
    FileSystem*         fs,
    ScalingSpriteCache* sc)
{
  std::string file = path;
  if(!EndsWith(file, ".json"))
  {
    file += ".json";
  }
  OpenOrFocusWindow(
      windows,
      Str{} << "Scimed editor: " << file,
      [&]() -> std::shared_ptr<GenericWindow> {
        auto sprite = sc->Get(file);
        return CreateGenericWindow(
            sprite, [](auto sprite) { scalingsprite::RunImgui(sprite.get()); });
      });
}

template <typename T, typename TOpen, typename TRun>
void
OpenOrFocusOnGenericWindow(
    Windows*           windows,
    const std::string& path,
    FileSystem*        fs,
    const std::string& title,
    TOpen              open_function,
    TRun               run_function)
{
  OpenOrFocusWindow(
      windows,
      Str{} << title << ": " << path,
      [=]() -> std::shared_ptr<GenericWindow> {
        auto window = CreateGenericWindow(T{}, [=](T& t) { run_function(&t); });
        const auto err = LoadProtoJson(fs, &window->data, path);
        if(!err.empty())
        {
          LOG_ERROR("Failed to load: " << err);
        }
        return window;
      });
}

struct FileHandler
{
  std::string context_menu;

  explicit FileHandler(const std::string& menu)
      : context_menu(menu)
  {
  }

  virtual ~FileHandler() = default;

  virtual bool
  Matches(const std::string& path) = 0;

  virtual void
  Open(Windows* windows, const std::string& path) = 0;
};

template <typename TMatchFunction, typename TOpenFunction>
struct GenericFileHandler : public FileHandler
{
  TMatchFunction match_function;
  TOpenFunction  open_function;
  GenericFileHandler(
      const std::string& menu, TMatchFunction match, TOpenFunction open)
      : FileHandler(menu)
      , match_function(match)
      , open_function(open)
  {
  }

  bool
  Matches(const std::string& path) override
  {
    return match_function(path);
  }

  void
  Open(Windows* windows, const std::string& path) override
  {
    return open_function(windows, path);
  }
};

template <typename TMatchFunction, typename TOpenFunction>
std::shared_ptr<FileHandler>
CreateHandler(const std::string& menu, TMatchFunction match, TOpenFunction open)
{
  return std::make_shared<GenericFileHandler<TMatchFunction, TOpenFunction>>(
      menu, match, open);
}

struct FileHandlerList
{
  std::vector<std::shared_ptr<FileHandler>> handlers;

  void
  Add(std::shared_ptr<FileHandler> handler)
  {
    handlers.emplace_back(handler);
  }

  bool
  Open(Windows* windows, const std::string& path)
  {
    for(auto& handler : handlers)
    {
      if(handler->Matches(path))
      {
        handler->Open(windows, path);
        return true;
      }
    }

    return false;
  }

  void
  RunImguiSelectable(Windows* windows, const std::string& path)
  {
    // todo: come up with a better name for this function
    for(auto& handler : handlers)
    {
      if(ImguiSelectableOrDisabled(
             !path.empty(), handler->context_menu.c_str()))
      {
        handler->Open(windows, path);
      }
    }
  }
};

struct ViewportHandler
{
  void
  SetSize(int width, int height)
  {
    Viewport viewport{
        Recti::FromWidthHeight(width, height).SetBottomLeftToCopy(0, 0)};
    viewport.Activate();
  }
};

int
main(int argc, char** argv)
{
  Engine engine;

  if(engine.Setup() == false)
  {
    return -1;
  }


  int window_width  = 1280;
  int window_height = 720;

  if(!engine.CreateWindow("Euphoria Editor", window_width, window_height, true))
  {
    return -1;
  }

  TextureCache       texture_cache{engine.file_system.get()};
  ScalingSpriteCache sprite_cache;

  ViewportHandler viewport_handler;
  viewport_handler.SetSize(window_width, window_height);


  bool running = true;

  FileBrowser browser{engine.file_system.get()};
  browser.Refresh();
  std::vector<std::shared_ptr<GenericWindow>> windows;
  StyleData                                   style_data = StyleData{};
  FileHandlerList                             file_types;

  //////////////////////////////////////////////////////////////////////////////
  // File types

  file_types.Add(CreateHandler(
      "Open with Game Data",
      [](const std::string& file) -> bool { return file == "gamedata.json"; },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusOnGenericWindow<game::Game>(
            windows,
            file,
            engine.file_system.get(),
            "Game",
            [](auto* s, const std::string& data) {},
            [](auto* s) { game::RunImgui(s); });
      }));

  file_types.Add(CreateHandler(
      "Open with World Editor",
      [](const std::string& file) -> bool { return file == "world.json"; },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusOnGenericWindow<world::World>(
            windows,
            file,
            engine.file_system.get(),
            "World",
            [](auto* s, const std::string& data) {},
            [](auto* s) { world::RunImgui(s); });
      }));
  file_types.Add(CreateHandler(
      "Open with Enum Editor",
      [](const std::string& file) -> bool { return false; },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusOnGenericWindow<enumlist::Enumroot>(
            windows,
            file,
            engine.file_system.get(),
            "Enums",
            [](auto* s, const std::string& data) {},
            [](auto* s) { enumlist::RunImgui(s); });
      }));

  file_types.Add(CreateHandler(
      "Open with text editor",
      [](const std::string& file) -> bool {
        return EndsWith(file, ".json") || EndsWith(file, ".js");
      },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusTextFile(windows, file, engine.file_system.get());
      }));

  file_types.Add(CreateHandler(
      "Open with scimed editor",
      [](const std::string& file) -> bool { return EndsWith(file, ".png"); },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusScimed(
            windows,
            file,
            engine.file_system.get(),
            &texture_cache,
            &sprite_cache);
      }));

  file_types.Add(CreateHandler(
      "Open with auto scimed editor",
      [](const std::string& file) -> bool { return false; },
      [&](Windows* windows, const std::string& file) {
        OpenOrFocusScimedEditior(
            windows, file, engine.file_system.get(), &sprite_cache);
      }));

  //////////////////////////////////////////////////////////////////////////////
  // main loop

  while(running)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      engine.imgui->ProcessEvents(&e);

      if(engine.HandleResize(e, &window_width, &window_height))
      {
        viewport_handler.SetSize(window_width, window_height);
      }

      switch(e.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        default:
          // ignore other events
          break;
      }
    }

    engine.imgui->StartNewFrame();

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
          OpenOrFocusStyleEditor(&windows);
        }
        ImGui::EndMenu();
      }

      if(ImGui::BeginMenu("Window"))
      {
        for(const auto& win : windows)
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
        const auto  wsize   = ImGui::GetWindowSize();
        const auto  spacing = ImGui::GetItemsLineHeightWithSpacing() * 2;
        const auto  space   = ImVec2{wsize.x - spacing, wsize.y - spacing};
        const float small   = 30;
        const float big     = 60;
        const auto sizer = [=](int id) -> float {
          return id == 1 ? big : small;
        };
        for(int yi = 0; yi < 3; yi += 1)
        {
          ImGui::PushID(yi);
          for(int xi = 0; xi < 3; xi += 1)
          {
            ImGui::PushID(xi);
            if(ImGui::Button("a", ImVec2{sizer(xi), sizer(yi)}))
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

      if(browser.Run())
      {
        const auto file = browser.GetSelectedFile();
        file_types.Open(&windows, file);
      }
      if(ImGui::BeginPopupContextItem("browser popup"))
      {
        const auto file = browser.GetSelectedFile();
        file_types.RunImguiSelectable(&windows, file);
        ImGui::EndPopup();
      }
    }
    ImGui::End();

    for(auto& win : windows)
    {
      ImGui::SetNextWindowSize(ImVec2{300, 300}, ImGuiCond_FirstUseEver);
      if(ImGui::Begin(win->name.c_str(), &win->open))
      {
        win->Run(&style_data);
      }
      ImGui::End();
    }

    // ImGui::ShowMetricsWindow();

    engine.init->ClearScreen(Color::Wheat);
    engine.imgui->Render();

    RemoveMatching(&windows, [](const std::shared_ptr<GenericWindow>& window) {
      return !window->open;
    });

    SDL_GL_SwapWindow(engine.window->window);
  }

  return 0;
}
