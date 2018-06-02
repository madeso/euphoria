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

  SdlTimer timer;

  bool paused = true;

  while(running)
  {
    const bool  show_imgui = true;
    const float delta      = timer.Update();

    if(show_imgui)
    {
      imgui.StartNewFrame();
    }

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
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
          const bool down = e.type == SDL_KEYDOWN;

          switch(e.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              if(down)
              {
                running = false;
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

    init.ClearScreen(Color::DarkslateGray);
    if(show_imgui)
    {
      imgui.Render();
    }

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
