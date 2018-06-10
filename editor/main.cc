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

#include "editor/browser.h"
#include "editor/scimed.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>


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

void
LoadFile(
    Scimed*            scimed,
    TextureCache*      cache,
    FileSystem*        fs,
    const std::string& path)
{
  scimed->texture = cache->GetTexture(path);
  scimed->scaling = std::make_shared<scalingsprite::ScalingSprite>();

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
    Windows* windows, const std::string& file, FileSystem* fs, TextureCache* tc)
{
  OpenOrFocusWindow(
      windows,
      Str{} << "Scimed: " << file,
      [&]() -> std::shared_ptr<GenericWindow> {
        auto scimed = std::make_shared<ScimedWindow>();
        LoadFile(&scimed->scimed, tc, fs, file);
        return scimed;
      });
}

int
main(int argc, char** argv)
{
  SdlLibrary sdl;
  if(sdl.ok == false)
  {
    return -1;
  }

  int width  = 1280;
  int height = 720;

  SdlWindow window{"Euphoria Demo", width, height};
  if(window.window == nullptr)
  {
    return -1;
  }

  SdlGlContext context{&window};

  if(context.context == nullptr)
  {
    return -1;
  }

  Init init{SDL_GL_GetProcAddress};
  if(init.ok == false)
  {
    return -4;
  }

  SetupOpenglDebug();

  const auto current_directory = GetCurrentDirectory();
  const auto base_path         = GetBasePath();
  const auto pref_path         = GetPrefPath();

  ImguiLibrary imgui{window.window, pref_path};
  ImGui::StyleColorsLight();

  Viewport viewport{
      Recti::FromWidthHeight(width, height).SetBottomLeftToCopy(0, 0)};
  viewport.Activate();

  FileSystem file_system;
  auto       catalog = FileSystemRootCatalog::AddRoot(&file_system);

  FileSystemRootFolder::AddRoot(&file_system, current_directory);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");

  SetupDefaultFiles(catalog);

  TextureCache texture_cache{&file_system};

  bool running = true;

  FileBrowser browser{&file_system};
  browser.Refresh();
  std::vector<std::shared_ptr<GenericWindow>> scimeds;
  StyleData                                   style_data = StyleData{};

  OpenOrFocusStyleEditor(&scimeds);

  while(running)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      imgui.ProcessEvents(&e);

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

    imgui.StartNewFrame();

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
          OpenOrFocusStyleEditor(&scimeds);
        }
        ImGui::EndMenu();
      }

      if(ImGui::BeginMenu("Window"))
      {
        for(const auto& window : scimeds)
        {
          if(ImGui::MenuItem(window->name.c_str()))
          {
            ImGui::SetNextWindowFocus();
            ImGui::Begin(window->name.c_str());
            ImGui::End();
          }
        }

        ImGui::EndMenu();
      }
    }
    ImGui::EndMainMenuBar();

    if(ImGui::Begin("Solution explorer"))
    {
      if(browser.Run())
      {
        const auto file = browser.GetSelectedFile();
        if(EndsWith(file, ".json") || EndsWith(file, ".js"))
        {
          OpenOrFocusTextFile(&scimeds, file, &file_system);
        }
        else
        {
          OpenOrFocusScimed(&scimeds, file, &file_system, &texture_cache);
        }
      }
      if(ImGui::BeginPopupContextItem("browser popup"))
      {
        const auto file = browser.GetSelectedFile();

        if(ImguiSelectableOrDisabled(!file.empty(), "Open with text editor"))
        {
          OpenOrFocusTextFile(&scimeds, file, &file_system);
        }
        if(ImguiSelectableOrDisabled(!file.empty(), "Open with scimed editor"))
        {
          OpenOrFocusScimed(&scimeds, file, &file_system, &texture_cache);
        }
        ImGui::EndPopup();
      }
    }
    ImGui::End();

    for(auto& scimed : scimeds)
    {
      ImGui::SetNextWindowSize(ImVec2{300, 300}, ImGuiCond_FirstUseEver);
      if(ImGui::Begin(scimed->name.c_str(), &scimed->open))
      {
        scimed->Run(&style_data);
      }
      ImGui::End();
    }

    // ImGui::ShowMetricsWindow();

    init.ClearScreen(Color::Wheat);
    imgui.Render();

    RemoveMatching(&scimeds, [](const std::shared_ptr<GenericWindow>& window) {
      return !window->open;
    });

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
