#include <core/mat4.h>
#include "core/draw.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_path.h"
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

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>


// LOG_SPECIFY_DEFAULT_LOGGER("painter")

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

  if(!engine.CreateWindow("Painter", window_width, window_height, true))
  {
    return -1;
  }

  // ViewportHandler viewport_handler;
  // viewport_handler.SetSize(window_width, window_height);


  bool running = true;

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
        // viewport_handler.SetSize(window_width, window_height);
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
        ImGui::EndMenu();
      }
    }
    ImGui::EndMainMenuBar();

    ImGui::SetNextWindowSize(ImVec2{300, 300}, ImGuiCond_FirstUseEver);
    if(ImGui::Begin("painter"))
    {
      // win->Run(&style_data);
    }
    ImGui::End();

    // ImGui::ShowMetricsWindow();

    engine.init->ClearScreen(Color::Wheat);
    engine.imgui->Render();

    SDL_GL_SwapWindow(engine.window->window);
  }

  return 0;
}

