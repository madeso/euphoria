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

#include "editor/browser.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>


ImVec2
operator+(const ImVec2& lhs, const ImVec2& rhs)
{
  return ImVec2{lhs.x + rhs.x, lhs.y + rhs.y};
}

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

  std::string demo_file;
  FileBrowser browser{&file_system};

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

      if(ImGui::Begin("Browser"))
      {
        InputText("Folder", &demo_file);
        ImGui::SameLine();
        if(ImGui::Button("..."))
        {
          browser.SelectFile(demo_file);
          ImGui::OpenPopup("browse");
        }

#if 0
        ImGui::SetNextWindowSize(ImVec2{300, 300}, ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(
            ImGui::GetCursorPos() + ImGui::GetWindowPos(), ImGuiCond_Always);
        if(ImGui::BeginPopupModal(
               "browse",
               nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ResizeFromAnySide))
        {
          if(browser.Run())
          {
            demo_file = browser.GetSelectedFile();
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
#else
        ImGui::SetNextWindowSize(ImVec2{300, 300}, ImGuiCond_Always);
        if(ImGui::BeginPopup("browse"))
        {
          if(browser.Run())
          {
            demo_file = browser.GetSelectedFile();
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }
#endif
        ImGui::End();
      }
    }

#if 0
    const auto mp   = ImGui::GetIO().MousePos;
    auto*      list = ImGui::GetOverlayDrawList();
    list->AddLine(ImVec2{mp.x, 0}, ImVec2{mp.x, size.y}, IM_COL32_BLACK);
    list->AddLine(ImVec2{0, mp.y}, ImVec2{size.x, mp.y}, IM_COL32_BLACK);
#endif

    init.ClearScreen(Color::Wheat);
    imgui.Render();

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
