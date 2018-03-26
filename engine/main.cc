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
#include "core/proto.h"

#include "render/debuggl.h"
#include "render/fonts.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"

#include "gui/root.h"
#include "window/key.h"

#include "engine/loadworld.h"
#include "engine/systems.h"
#include "engine/duk.h"
#include "engine/dukintegration.h"
#include "engine/dukmathbindings.h"
#include "engine/input.h"

#include "game.pb.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine")

game::Game
LoadGameData(FileSystem* fs)
{
  game::Game game;
  const auto err = LoadProtoJson(fs, &game, "gamedata.json");
  if(!err.empty())
  {
    LOG_ERROR("Failed to load gamedata.json: " << err);
  }
  return game;
}

void
RunMainScriptFile(Duk* duk, FileSystem* fs, const std::string& path)
{
  std::string content;
  const bool  loaded = fs->ReadFileToString(path, &content);
  if(!loaded)
  {
    LOG_ERROR("Unable to open " << path << " for running");
    return;
  }
  std::string error;
  const bool  eval = duk->eval_string(content, &error, nullptr);
  if(!eval)
  {
    LOG_ERROR("Failed to run " << path << ": " << error);
  }
}

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

  const auto current_directory = GetCurrentDirectory();
  FileSystem file_system;
  file_system.SetWrite(
      std::make_shared<FileSystemWriteFolder>(current_directory));
  FileSystemRootFolder::AddRoot(&file_system, current_directory);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");
  FileSystemDefaultShaders::AddRoot(&file_system, "shaders");
  auto catalog = FileSystemRootCatalog::AddRoot(&file_system);
  catalog->RegisterFileString(
      "debug_font.json",
      R"(  {"size": 30, "sources": [{"builtin": {}}] }  )");
  TextureCache cache{&file_system};

  game::Game gamedata = LoadGameData(&file_system);

  int width  = 800;
  int height = 600;

  SDL_Window* window = SDL_CreateWindow(
      gamedata.title().c_str(),
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

  // todo: update theese during runtime
  bool has_crashed = true;
  Font font{&file_system, &cache, "debug_font.json"};
  Text crash_message{&font};
  crash_message.SetText(ParsedText::FromText("something failed... hahah"));

  Input input;

  for(const auto& bind : gamedata.binds())
  {
    auto key = ToKey(bind.key());
    if(key == Key::INVALID)
    {
      LOG_ERROR("Invalid key: " << bind.key());
      key = Key::UNBOUND;
    }

    input.Add(std::make_shared<BoundVar>(bind.name(), key));
  }

  Shader shader;
  attributes2d::PrebindShader(&shader);
  shader.Load(&file_system, "shaders/sprite");
  SpriteRenderer renderer(&shader);
  FontCache      font_cache{&file_system, &cache};

  // Sprite player(cache.GetTexture("player.png"));
  // objects.Add(&player);

  Duk duk;
  BindMath(&duk);

  Systems systems;
  AddSystems(&systems, &duk);
  World          world{&systems};
  DukIntegration integration{&systems, &world, &duk};
  RunMainScriptFile(&duk, &file_system, "main.js");


  LoadWorld(&file_system, &world, &cache, "game.json");

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
      if(has_crashed)
      {
        if(e.type == SDL_KEYUP)
        {
          const auto key = ToKey(e.key.keysym);
          if(key == Key::ESCAPE)
          {
            running = false;
          }
        }
      }
      else
      {
        if(e.type == SDL_MOUSEMOTION)
        {
          window_mouse_x = e.motion.x;
          window_mouse_y = e.motion.y;
        }
        else if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN)
        {
          const bool down = e.type == SDL_KEYDOWN;
          const auto key  = ToKey(e.key.keysym);
          input.SetKeyState(key, down ? 1.0f : 0.0f);
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
    }

    if(has_crashed == false)
    {
      integration.BindKeys(&duk, input);
    }

    if(has_crashed)
    {
      init.ClearScreen(Color::CornflowerBlue);
      crash_message.Draw(&renderer, vec2f{20, 20}, Color::Black);
    }
    else
    {
      init.ClearScreen(Color::DarkslateGray);
      world.Draw(&renderer);
    }

    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
