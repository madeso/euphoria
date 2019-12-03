#include "core/aabb.h"
#include "core/axisangle.h"
// #include "core/draw.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/mat4.h"
#include "core/os.h"
#include "core/path.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/texturetypes.h"
#include "core/str.h"
#include "core/mat4.h"
#include "core/fpscontroller.h"
#include "core/camera.h"

// #include "render/camera.h"
#include "render/defaultfiles.h"
#include "render/materialshadercache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/viewport.h"
#include "render/world.h"
#include <render/compiledmesh.h>
#include <render/debuggl.h>
#include <render/init.h>
#include <render/materialshader.h>
#include <render/texturecache.h>

#include "window/filesystem.h"
#include "window/imgui_ext.h"
#include "window/imguilibrary.h"
#include "window/sdlglcontext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/timer.h"
#include "window/key.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>

#include "t3d/tilelibrary.h"

using namespace euphoria::core;
using namespace euphoria::core::vfs;
using namespace euphoria::t3d;
using namespace euphoria::render;
using namespace euphoria::window;

void
HandleEvents(
    bool           show_imgui,
    bool&          running,
    bool&          immersive_mode,
    FpsController& fps,
    float          delta,
    ImguiLibrary&  imgui,
    SdlWindow&     window)
{
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0)
  {
    if(show_imgui)
    {
      imgui.ProcessEvents(&e);
    }
    switch(e.type)
    {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEMOTION:
        if(!show_imgui)
        {
          fps.Look(e.motion.xrel, e.motion.yrel);
        }
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
      {
        const bool down = e.type == SDL_KEYDOWN;

        if(!show_imgui)
        {
          fps.HandleKey(ToKey(e.key.keysym), down);
        }

        switch(e.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            if(down)
            {
              running = false;
            }
            break;
          case SDLK_TAB:
            if(!down)
            {
              immersive_mode = !immersive_mode;
              window.KeepWithin(immersive_mode);
              window.EnableCharEvent(!immersive_mode);
            }
            break;
          default:
            // ignore other keys
            break;
        }
      }
      break;
      default:
        // ignore other events
        break;
    }
  }

  fps.Update(delta);
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

  SdlWindow window{"t3d", width, height};
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
  if(!init.ok)
  {
    return -4;
  }

  SetupOpenglDebug();

  const auto current_directory = GetCurrentDirectory();
  const auto base_path         = GetBasePath();
  const auto pref_path         = GetPrefPath();

  ImguiLibrary imgui{window.window, &context, pref_path};

  Viewport viewport{
      Recti::FromWidthHeight(width, height).SetBottomLeftToCopy(0, 0)};
  viewport.Activate();

  FileSystem file_system;
  auto       catalog = FileSystemRootCatalog::AddRoot(&file_system);

  FileSystemRootFolder::AddRoot(&file_system, current_directory);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");

  SetupDefaultFiles(catalog);

  MaterialShaderCache material_shader_cache{&file_system};

  // SET_ENUM_VALUES(TextureType, SetupTextureNames);
  SET_ENUM_FROM_FILE(&file_system, "texture_types.json", TextureType);

  TextureCache texture_cache{&file_system};

  TileLibrary tile_library;
  tile_library.AddDirectory("world", &material_shader_cache, &texture_cache);

  World world {&file_system};

  std::vector<std::shared_ptr<Actor>> actors;

  for(auto tile : tile_library.tiles)
  {
    auto actor = std::make_shared<Actor>(tile->mesh);
    world.AddActor(actor);
    actors.emplace_back(actor);
  }

  bool running = true;

  SdlTimer timer;


  bool immersive_mode = false;

  Camera camera {};
  camera.position = vec3f(0, 0, 0);

  FpsController fps;
  fps.position = vec3f{0, 0, 3};
  window.EnableCharEvent(!immersive_mode);

  int   items_per_row = 5;
  float size          = 3;

  while(running)
  {
    const bool  show_imgui = !immersive_mode;
    const float delta      = timer.Update();

    HandleEvents(
        show_imgui, running, immersive_mode, fps, delta, imgui, window);

    if(show_imgui)
    {
      imgui.StartNewFrame();

      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);

      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
      ImGui::Begin("Light");
      ImGui::Combo(
          "Type",
          reinterpret_cast<int*>(world.light.GetTypeMod()),
          "Directional\0Point\0Spot\0\0");
      ImGuiColorEdit3("Ambient", world.light.ModifyAmbient());
      ImGuiColorEdit3("Diffuse", world.light.ModifyDiffuse());
      ImGuiColorEdit3("Specular", world.light.ModifySpecular());
      ImGui::End();

      ImGui::Begin("Camera");
      ImGui::InputFloat("Speed", &fps.speed);
      ImGui::End();

      ImGui::ShowTestWindow();


      // ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
      ImGui::Begin("Tiles");
      ImGui::InputInt("Items per row", &items_per_row);
      ImGui::InputFloat("Size", &size);

      if(!tile_library.tiles.empty())
      {
        ImGui::ListBoxHeader("Tiles");

        for(auto tile : tile_library.tiles)
        {
          std::string display = Str{} << tile->name << ": "
                                      << tile->aabb.GetSize();
          ImGui::Selectable(display.c_str());
        }

        ImGui::ListBoxFooter();
      }
      ImGui::End();
    }

    int col = 0;
    int row = 0;

    if(items_per_row > 0)
    {
      for(auto a : actors)
      {
        const auto x = col * size;
        const auto z = row * size;
        a->SetPosition(vec3f{x, 0, z});

        col += 1;
        if(col > items_per_row)
        {
          col = 0;
          row += 1;
        }
      }
    }


    camera.position = fps.position;
    camera.rotation = fps.GetRotation();

    init.ClearScreen(Color::LightGray);
    world.Render(viewport, camera);

    if(show_imgui)
    {
      imgui.Render();
    }

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
