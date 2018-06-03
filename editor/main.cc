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

template <typename T, typename G>
int
IndexOf(const std::vector<T>& ss, const G& finder)
{
  int index = 0;
  for(const auto& s : ss)
  {
    if(finder(s))
    {
      return index;
    }
    index += 1;
  }

  return -1;
}

struct FileBrowser
{
  std::string             current_folder;
  int                     selected_file = -1;
  std::vector<ListedFile> files;

  FileSystem* file_system = nullptr;
  explicit FileBrowser(FileSystem* fs)
      : current_folder(Path::FromRoot().GetAbsolutePath())
      , file_system(fs)
  {
  }

  std::string
  GetSelectedFile()
  {
    if(selected_file >= 0 && selected_file < files.size())
    {
      const auto suggested = files[selected_file];
      if(!EndsWith(suggested.name, '/'))
      {
        if(current_folder.empty())
        {
          return suggested.name;
        }
        else
        {
          return current_folder + suggested.name;
        }
      }
    }
    return "";
  }

  void
  SelectFile(const std::string& p)
  {
    const auto file = Path::FromFile(p);
    current_folder  = file.GetDirectory().GetAbsolutePath();
    Refresh();
    if(!p.empty())
    {
      selected_file = IndexOf(files, [&](const ListedFile& n) -> bool {
        return n.name == file.GetFileName();
      });
    }
  }

  void
  Refresh()
  {
    files = file_system->ListFiles(Path::FromDirectory(current_folder));
    if(!current_folder.empty())
    {
      // files.emplace_back();
      files.insert(files.begin(), ListedFile{"../", true});
    }
    // files.insert(files.begin(), f.begin(), f.end());
    selected_file = -1;
  }

  bool
  Run()
  {
    bool selected = false;
    InputText("URL", &current_folder);
    // ImGui::PushItemWidth(-1);
    ImGui::ListBoxHeader("", ImVec2{-1, -1});
    int index = 0;
    for(const auto& item : files)
    {
      const bool custom = item.is_builtin;
      if(custom)
      {
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor{0, 0, 255}.Value);
      }
      if(ImGui::Selectable(item.name.c_str(), index == selected_file))
      {
        selected_file = index;
      }
      if(custom)
      {
        ImGui::PopStyleColor();
      }

      if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
      {
        if(EndsWith(item.name, '/'))
        {
          if(item.name == "../")
          {
            current_folder = Path::FromDirectory(current_folder)
                                 .GetParentDirectory()
                                 .GetAbsolutePath();
          }
          else
          {
            current_folder += item.name;
          }
          Refresh();
        }
        else
        {
          selected = true;
        }
      }
      index += 1;
    }
    ImGui::ListBoxFooter();
    // ImGui::PopItemWidth();

    return selected;
  }
};

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

        ImGui::End();
      }
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
