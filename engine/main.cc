#include <SDL2/SDL.h>
#include <iostream>

#include "core/log.h"

#include "render/shader.h"
#include "render/spriterender.h"

#include "core/interpolate.h"
#include "core/os.h"
#include "core/componentsystem.h"
#include "core/filesystemimagegenerator.h"
#include "core/filesystemdefaultshaders.h"

#include "render/debuggl.h"
#include "render/fonts.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"

#include "gui/root.h"

#include "engine/components.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine")

int
main(int argc, char** argv)
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
  {
    LOG_ERROR("Failed to init SDL: " << SDL_GetError());
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  int width  = 800;
  int height = 600;

  SDL_Window* window = SDL_CreateWindow(
      "Engine",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      width,
      height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if(window == NULL)
  {
    LOG_ERROR("Failed to create window " << SDL_GetError());
    return -1;
  }

  SDL_GL_CreateContext(window);
  Init init{SDL_GL_GetProcAddress, Init::BlendHack::EnableHack};

  if(init.ok == false)
  {
    return -4;
  }

  SetupOpenglDebug();

  const auto current_directory = GetCurrentDirectory();
  FileSystem file_system;
  file_system.SetWrite(
      std::make_shared<FileSystemWriteFolder>(current_directory));
  FileSystemRootFolder::AddRoot(&file_system, current_directory);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");
  FileSystemDefaultShaders::AddRoot(&file_system, "shaders");

  TextureCache cache{&file_system};
  Shader       shader;
  attributes2d::PrebindShader(&shader);
  shader.Load(&file_system, "shaders/sprite");
  SpriteRenderer renderer(&shader);
  FontCache      font_cache{&file_system, &cache};


  // Sprite player(cache.GetTexture("player.png"));
  // objects.Add(&player);

  Systems systems;
  World   world;
  world.systems = &systems;
  AddSystems(&systems);
  LoadWorld(&world, "game.json", &cache);

  const mat4f projection = init.GetOrthoProjection(width, height);
  Use(&shader);
  shader.SetUniform(shader.GetUniform("image"), 0);
  shader.SetUniform(shader.GetUniform("projection"), projection);

  Viewport viewport{
      Recti::FromWidthHeight(width, height).SetBottomLeftToCopy(0, 0)};
  viewport.Activate();

  Uint64 NOW  = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  SDL_StartTextInput();

  bool running = true;

  int window_mouse_x = 0;
  int window_mouse_y = 0;
  SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
  bool mouse_lmb_down = false;

  while(running)
  {
    LAST           = NOW;
    NOW            = SDL_GetPerformanceCounter();
    const float dt = (NOW - LAST) * 1.0f / SDL_GetPerformanceFrequency();
    SDL_Event   e;

    world.Update(dt);

    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
      {
        running = false;
      }
      else if(e.type == SDL_MOUSEMOTION)
      {
        window_mouse_x = e.motion.x;
        window_mouse_y = e.motion.y;
      }
      else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
      {
        const bool down = e.type == SDL_MOUSEBUTTONDOWN;
        window_mouse_x  = e.button.x;
        window_mouse_y  = e.button.y;
        if(e.button.button == SDL_BUTTON_LEFT)
        {
          mouse_lmb_down = down;
        }
      }
      else if(e.type == SDL_TEXTINPUT)
      {
        const std::string& input = e.text.text;
      }
    }

    init.ClearScreen(Color::DarkslateGray);
    world.Draw(&renderer);

    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
