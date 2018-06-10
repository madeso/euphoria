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

struct ScimedWindow : public GenericWindow
{
  Scimed scimed;

  void
  Run(StyleData* style_data) override
  {
    scimed.Run(style_data->cc, style_data->scc);
  }
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
    windows->emplace_back(create_window_function(title_name));
  }
  else
  {
    ImGui::SetNextWindowFocus();
    ImGui::Begin(title_name.c_str());
    ImGui::End();
  }
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

        OpenOrFocusWindow(
            &scimeds,
            Str{} << "Scimed: " << file,
            [&](const std::string& title_name)
                -> std::shared_ptr<GenericWindow> {
              auto scimed  = std::make_shared<ScimedWindow>();
              scimed->name = title_name;
              scimed->scimed.LoadFile(&texture_cache, &file_system, file);
              return scimed;
            });
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
