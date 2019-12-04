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
#include "core/log.h"

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
#include "render/viewporthandler.h"

#include "window/filesystem.h"
#include "window/imgui_ext.h"
#include "window/imguilibrary.h"
#include "window/sdlglcontext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/timer.h"
#include "window/key.h"
#include "window/engine.h"

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

LOG_SPECIFY_DEFAULT_LOGGER("t3d")

void
HandleEvents(
    bool           show_imgui,
    bool&          running,
    bool&          immersive_mode,
    FpsController& fps,
    float          delta,
    Engine*        engine, ViewportHandler* viewport_handler)
{
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0)
  {
    if(show_imgui)
    {
      engine->imgui->ProcessEvents(&e);
    }

    int window_width = 0;
    int window_height = 0;
    if(engine->HandleResize(e, &window_width, &window_height))
    {
      viewport_handler->SetSize(window_width, window_height);
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
              engine->window->KeepWithin(immersive_mode);
              engine->window->EnableCharEvent(!immersive_mode);
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
  Engine engine;

  if(engine.Setup(argparse::Args::Extract(argc, argv)) == false)
  {
    return -1;
  }

  int width  = 1280;
  int height = 720;

  if(engine.CreateWindow("t3d", width, height) == false)
  {
    return -1;
  }

  ViewportHandler viewport_handler;

  viewport_handler.SetSize(width, height);

  MaterialShaderCache material_shader_cache{engine.file_system.get()};

  // SET_ENUM_VALUES(TextureType, SetupTextureNames);
  SET_ENUM_FROM_FILE(engine.file_system.get(), "texture_types.json", TextureType);

  TextureCache texture_cache{engine.file_system.get()};

  TileLibrary tile_library {engine.file_system.get()};
  tile_library.AddDirectory("world", &material_shader_cache, &texture_cache);

  if(tile_library.tiles.empty())
  {
    LOG_ERROR("No tile loaded!");
    return -2;
  }

  World world {engine.file_system.get()};

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
  engine.window->EnableCharEvent(!immersive_mode);

  int   items_per_row = 5;
  float size          = 3;

  while(running)
  {
    const bool  show_imgui = !immersive_mode;
    const float delta      = timer.Update();

    HandleEvents(
        show_imgui, running, immersive_mode, fps, delta, &engine, &viewport_handler);

    if(show_imgui)
    {
      engine.imgui->StartNewFrame();

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
      ImGui::DragFloat("Speed", &fps.speed, 0.1f, 0.001f, 10.0f);
      ImGui::End();

      ImGui::ShowTestWindow();


      // ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
      ImGui::Begin("Tiles");
      ImGui::InputInt("Items per row", &items_per_row);
      ImGui::DragFloat("Size", &size, 0.01f);

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

    {
      auto viewport = viewport_handler.GetFullViewport();
      engine.init->ClearScreen(Color::LightGray);
      world.Render(viewport, camera);
    }

    if(show_imgui)
    {
      engine.imgui->Render();
    }

    SDL_GL_SwapWindow(engine.window->window);
  }

  return 0;
}
