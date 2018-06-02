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
#include "window/imgui.h"
#include "window/fpscontroller.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>

struct FileBrowser
{
  std::string              current_folder;
  int                      selected_file = -1;
  std::vector<std::string> files         = {"file1.cc", "cat.cc"};

  void
  Run()
  {
    if(ImGui::Begin("Browser"))
    {
      InputText("URL", &current_folder);
      if(ImGui::Button("Refresh"))
      {
        // files = file_system.ListFiles(current_folder);
        files.resize(0);
        files.emplace_back("dog");
        files.emplace_back("cat");
      }
      ImGui::ListBox(
          "Files",
          &selected_file,
          [](void* data, int idx, const char** out_text) -> bool {
            auto* ff  = static_cast<std::vector<std::string>*>(data);
            *out_text = (*ff)[idx].c_str();
            return true;
          },
          &files,
          files.size(),
          10);
      ImGui::End();
    }
  }
};

int
main(int argc, char** argv)
{
  SdlLibrary sdl;
  if(sdl.ok == false)
  {
    return -1;
  }

  int        width  = 1280;
  int        height = 720;
  const auto size   = ImVec2{width, height};

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

  FileBrowser browser;

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
      ImGui::EndMainMenuBar();

      browser.Run();
    }

    const auto mp   = ImGui::GetIO().MousePos;
    auto*      list = ImGui::GetOverlayDrawList();
    list->AddLine(ImVec2{mp.x, 0}, ImVec2{mp.x, size.y}, IM_COL32_BLACK);
    list->AddLine(ImVec2{0, mp.y}, ImVec2{size.x, mp.y}, IM_COL32_BLACK);

    init.ClearScreen(Color::Wheat);
    imgui.Render();

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
