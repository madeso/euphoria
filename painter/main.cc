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

#include "core/path.h"

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

#include "painter/canvas.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "window/imgui_ext.h"

LOG_SPECIFY_DEFAULT_LOGGER("painter")

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
  CanvasConfig cc;
  Canvas canvas;
  BezierPath2 path (point2f(0,0));
  int index = -1;

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
      canvas.Begin(cc);
      canvas.ShowGrid(cc);

      if(ImGui::IsMouseReleased(0)) { index = -1; }
      auto handle = [&canvas, &index](const ImVec2& p, int id, float size)
      {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto sp = canvas.WorldToScreen(p);
        const auto me = ImGui::GetMousePos();
        const auto hover = vec2f::FromTo(C(me),C(sp)).GetLengthSquared() < size*size;
        if(index==-1 && hover && ImGui::IsMouseDown(0))
        {
          index = id;
        }
        const auto red = IM_COL32(hover || index==id ? 200 : 100, 0, 0, 255);
        draw_list->AddCircleFilled(sp, size, red);
        if(index==id)
        {
          // capture current drag item...
          if(ImGui::IsMouseDragging())
          {
            const auto d = ImGui::GetMouseDragDelta();
            ImGui::ResetMouseDragDelta();
            // todo: handle scale/zoom
            return std::make_pair(true, vec2f(d.x, d.y));
          }
        }
        return std::make_pair(false, vec2f(0,0));
      };

      int i = 0;
      for(auto& p: path.points)
      {
        i += 1;
        auto r = handle(C(p), i, 10);
        if(r.first)
        {
          p += r.second;
        }
      }

      canvas.ShowRuler(cc);
      canvas.End(cc);
    }
    ImGui::End();

    // ImGui::ShowMetricsWindow();

    engine.init->ClearScreen(Color::Wheat);
    engine.imgui->Render();

    SDL_GL_SwapWindow(engine.window->window);
  }

  return 0;
}

