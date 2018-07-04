#include <SDL2/SDL.h>
#include <iostream>

#include "core/log.h"
#include "core/str.h"

#include "render/shader.h"
#include "render/spriterender.h"

#include "core/interpolate.h"
#include "core/os.h"
#include "core/componentsystem.h"
#include "core/filesystemimagegenerator.h"
#include "core/filesystemdefaultshaders.h"
#include "core/proto.h"
#include "core/viewport.h"

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
#include "core/duk.h"
#include "engine/dukintegration.h"
#include "engine/dukmathbindings.h"
#include "engine/dukprint.h"
#include "engine/input.h"
#include "engine/objectemplate.h"
#include "engine/components.h"

#include "window/imguilibrary.h"
#include "window/imgui_ext.h"
#include "window/filesystem.h"

#include "imgui/imgui.h"

#include "gaf_game.h"

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

struct RunResult
{
  bool        ok;
  std::string message;

  static const RunResult
  Ok()
  {
    return RunResult{true, ""};
  };

  static const RunResult
  Error(const std::string& message)
  {
    return RunResult{false, message};
  };

 private:
  RunResult(bool b, const std::string& str)
      : ok(b)
      , message(str)
  {
  }
};

RunResult
RunMainScriptFile(Duk* duk, FileSystem* fs, const std::string& path)
{
  std::string content;
  const bool  loaded = fs->ReadFileToString(path, &content);
  if(!loaded)
  {
    const std::string error_message = Str() << "Unable to open " << path
                                            << " for running";
    LOG_ERROR(error_message);
    return RunResult::Error(error_message);
  }
  std::string error;
  const bool  eval = duk->EvalString(content, path, &error, nullptr);
  if(!eval)
  {
    const std::string error_message = Str() << "Failed to run " << path << ": "
                                            << error;
    LOG_ERROR(error_message);
    return RunResult::Error(error_message);
  }

  return RunResult::Ok();
}

struct ViewportHandler
{
  Init*                init;
  std::vector<Shader*> shaders;

  explicit ViewportHandler(Init* i)
      : init(i)
  {
  }

  void
  Add(Shader* shader)
  {
    shaders.emplace_back(shader);
  }

  void
  SetSize(const ViewportDef& vp, bool shaders_too = true)
  {
    if(shaders_too)
    {
      const mat4f projection =
          init->GetOrthoProjection(vp.virtual_width, vp.virtual_height);
      for(auto* shader : shaders)
      {
        shader->SetUniform(shader->GetUniform("projection"), projection);
      }
    }

    Viewport viewport{vp.screen_rect};
    viewport.Activate();
  }
};

ViewportDef
GetViewport(const game::Viewport& vp, int window_width, int window_height)
{
  switch(vp.type)
  {
    case game::ViewportType::FitWithBlackBars:
      return ViewportDef::FitWithBlackBars(
          vp.width, vp.height, window_width, window_height);
    case game::ViewportType::ScreenPixel:
      return ViewportDef::ScreenPixel(window_width, window_height);
    case game::ViewportType::Extend:
      return ViewportDef::Extend(
          vp.width, vp.height, window_width, window_height);
    default:
      DIE("Unhandled viewport case");
      return ViewportDef::ScreenPixel(window_width, window_height);
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
  auto         catalog = FileSystemRootCatalog::AddRoot(&file_system);
  TextureCache cache{&file_system};

  game::Game gamedata = LoadGameData(&file_system);

  int window_width  = 800;
  int window_height = 600;

  SDL_Window* window = SDL_CreateWindow(
      gamedata.title.c_str(),
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      window_width,
      window_height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if(window == NULL)
  {
    LOG_ERROR("Failed to create window " << SDL_GetError());
    return -1;
  }

  const auto pref_path = GetPrefPath();

  ImguiLibrary imgui{window, pref_path};

  SDL_GL_CreateContext(window);
  Init init{SDL_GL_GetProcAddress, Init::BlendHack::EnableHack};

  if(init.ok == false)
  {
    return -4;
  }

  SetupOpenglDebug();

  // todo: update theese during runtime
  std::string crash_message_string;
  bool        has_crashed = false;

  Input input;

  for(const auto& bind : gamedata.binds)
  {
    auto key = ToKey(bind.key);
    if(key == Key::INVALID)
    {
      LOG_ERROR("Invalid key: " << bind.key);
      key = Key::UNBOUND;
    }

    input.Add(std::make_shared<BoundVar>(bind.name, key));
  }

  Shader shader;
  attributes2d::PrebindShader(&shader);
  shader.Load(&file_system, "shaders/sprite");
  SpriteRenderer renderer(&shader);
  FontCache      font_cache{&file_system, &cache};

  // Sprite player(cache.GetTexture("player.png"));
  // objects.Add(&player);

  Duk duk;
  AddPrint(&duk);
  BindMath(&duk);
  Input::Bind(&duk);

  Systems    systems;
  World      world{&systems};
  Components components{&world.reg};
  AddSystems(&systems, &duk, &components);
  ObjectCreator templates;
  LoadTemplatesButOnlyNames(gamedata, &templates);

  DukIntegration integration{&systems, &world, &duk, &templates, &components};
  const auto error_run_main = RunMainScriptFile(&duk, &file_system, "main.js");
  if(!error_run_main.ok)
  {
    has_crashed          = true;
    crash_message_string = error_run_main.message;
  }
  LoadTemplates(
      gamedata, &templates, &integration.Registry(), &cache, &components);

  LoadWorld(
      &file_system,
      &world,
      &integration.Registry(),
      "world.json",
      &templates,
      duk.AsContext(),
      &duk);

  Use(&shader);
  shader.SetUniform(shader.GetUniform("image"), 0);

  ViewportHandler viewport_handler{&init};
  viewport_handler.Add(&shader);

  viewport_handler.SetSize(
      GetViewport(gamedata.viewport, window_width, window_height));

  Uint64 now  = SDL_GetPerformanceCounter();
  Uint64 last = 0;

  SDL_StartTextInput();

  bool running = true;

  int window_mouse_x = 0;
  int window_mouse_y = 0;
  SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
  bool mouse_lmb_down = false;

  integration.BindKeys(&duk, input);

  while(running)
  {
    last           = now;
    now            = SDL_GetPerformanceCounter();
    const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
    SDL_Event   e;

    imgui.StartNewFrame();

    if(!has_crashed)
    {
      try
      {
        world.Update(dt);
      }
      catch(const std::exception& ex)
      {
        has_crashed          = true;
        crash_message_string = ex.what();
      }
    }

    input.UpdateState();

    const auto window_id = SDL_GetWindowID(window);

    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
      {
        running = false;
      }
      if(e.type == SDL_WINDOWEVENT)
      {
        if(e.window.windowID == window_id)
        {
          switch(e.window.event)
          {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              SDL_GetWindowSize(window, &window_width, &window_height);
              viewport_handler.SetSize(
                  GetViewport(gamedata.viewport, window_width, window_height));
              break;
            default:
              break;
          }
        }
      }

      if(has_crashed)
      {
        imgui.ProcessEvents(&e);
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

    if(gamedata.viewport.type == game::ViewportType::FitWithBlackBars)
    {
      // LOG_INFO("Clearing black" << window_width << " " << window_height);
      viewport_handler.SetSize(
          ViewportDef::ScreenPixel(window_width, window_height), false);
      init.ClearScreen(Color::Black);
      viewport_handler.SetSize(
          GetViewport(gamedata.viewport, window_width, window_height), false);
    }

    if(has_crashed)
    {
      // todo: fix crash rendering, perhaps by using a debug gui or imgui
      // nothing much is required just a better overflow detection
      // when rendering the error, perhaps making the error more visible
      // though clicking around and debugging might be useful...
      init.ClearScreen(Color::CornflowerBlue);

      if(BeginFixedOverlay(ImguiCorner::Center, "Crashed"))
      {
        ImGui::TextDisabled("%s", crash_message_string.c_str());
        /*ImGui::InputTextMultiline(
            "##source",
            crash_message_string.c_str(),
            crash_message_string.length(),
            ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16),
            ImGuiInputTextFlags_ReadOnly); */
        if(ImGui::Button("Quit"))
        {
          running = false;
        }
        ImGui::End();
      }
    }
    else
    {
      init.ClearScreen(Color::DarkslateGray);
      world.Draw(&renderer);
    }

    imgui.Render();
    SDL_GL_SwapWindow(window);

    world.reg.RemoveRemoved();
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
