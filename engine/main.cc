#include <SDL.h>
#include <iostream>

#include "core/log.h"
#include "core/str.h"
#include "core/interpolate.h"
#include "core/os.h"
#include "core/componentsystem.h"
#include "core/filesystemimagegenerator.h"
#include "core/filesystemdefaultshaders.h"
#include "core/proto.h"
#include "core/viewport.h"

#include "core/sol.h"

#include "render/debuggl.h"
#include "render/fonts.h"
#include "render/init.h"
#include "render/fontcache.h"
#include "render/scalablesprite.h"
#include "render/shaderattribute2d.h"
#include "render/texturecache.h"
#include "render/viewport.h"
#include "render/shader.h"
#include "render/spriterender.h"

#include "gui/root.h"

#include "window/key.h"
#include "window/imguilibrary.h"
#include "window/imgui_ext.h"
#include "window/filesystem.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/engine.h"

#include "engine/loadworld.h"
#include "engine/systems.h"
#include "engine/dukintegration.h"
#include "engine/dukmathbindings.h"
#include "engine/dukprint.h"
#include "engine/input.h"
#include "engine/objectemplate.h"
#include "engine/components.h"
#include "engine/cameradata.h"

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
RunMainScriptFile(Sol* duk, FileSystem* fs, const std::string& path)
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
  const auto eval = duk->lua.script(
      content,
      [](lua_State*, sol::protected_function_result pfr) { return pfr; });
  if(!eval.valid())
  {
    const sol::error  err           = eval;
    const std::string error_message = Str() << "Failed to run " << path << ": "
                                            << err.what();
    LOG_ERROR(error_message);
    return RunResult::Error(error_message);
  }

  return RunResult::Ok();
}

struct ViewportHandler
{
  Init*                init;
  std::vector<Shader*> shaders;
  CameraData*          camera = nullptr;

  ViewportHandler(Init* i, CameraData* cam)
      : init(i)
      , camera(cam)
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

    camera->screen =
        Rectf::FromWidthHeight(vp.virtual_width, vp.virtual_height);

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

Rgb
GetColor(std::shared_ptr<game::Color> c)
{
  if(c == nullptr)
  {
    return Color::DarkslateGray;
  }

  if(c->hex != nullptr)
  {
    return Rgb::FromHex(colorutil::FromStringToHex(*c->hex));
  }

  LOG_ERROR("Unable to parse color");
  return Color::CornflowerBlue;
}

// engine
int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	// L is the lua state, which you can wrap in a state_view if necessary
	// maybe_exception will contain exception, if it exists
	// description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
	std::cout << "An exception occurred in a function, here's what it says ";
	if (maybe_exception) {
		std::cout << "(straight from the exception): ";
		const std::exception& ex = *maybe_exception;
		std::cout << ex.what() << std::endl;
	}
	else {
		std::cout << "(from the description parameter): ";
		std::cout.write(description.data(), description.size());
		std::cout << std::endl;
	}

	// you must push 1 element onto the stack to be 
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(L, description);
}
int
main(int argc, char** argv)
{
  Engine engine;
  if(engine.Setup() == false)
  {
    return -1;
  }

  engine.file_system->SetWrite(
      std::make_shared<FileSystemWriteFolder>(GetCurrentDirectory()));

  TextureCache cache{engine.file_system.get()};

  game::Game gamedata    = LoadGameData(engine.file_system.get());
  const auto clear_color = GetColor(gamedata.clear_color);

  int window_width  = 800;
  int window_height = 600;

  if(engine.CreateWindow(gamedata.title, window_width, window_height, true) ==
     false)
  {
    return -1;
  }

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
  shader.Load(engine.file_system.get(), "shaders/sprite");
  SpriteRenderer renderer(&shader);
  FontCache      font_cache{engine.file_system.get(), &cache};

  // Sprite player(cache.GetTexture("player.png"));
  // objects.Add(&player);

  Sol duk;
  duk.lua.set_exception_handler(&my_exception_handler);
  duk.lua.open_libraries(sol::lib::base, sol::lib::package);
  AddPrint(&duk);
  BindMath(&duk);
  Input::Bind(&duk);

  Systems    systems;
  World      world{&systems};
  Components components{&world.reg};
  AddSystems(&systems, &duk, &components);
  ObjectCreator templates;
  LoadTemplatesButOnlyNames(gamedata, &templates);
  CameraData camera_data;

  DukIntegration integration{
      &systems, &world, &duk, &templates, &components, &camera_data};
  const auto error_run_main =
      RunMainScriptFile(&duk, engine.file_system.get(), "main.lua");
  if(!error_run_main.ok)
  {
    has_crashed          = true;
    crash_message_string = error_run_main.message;
  }
  LoadTemplates(
      gamedata, &templates, &integration.Registry(), &cache, &components);

  Use(&shader);
  shader.SetUniform(shader.GetUniform("image"), 0);

  ViewportHandler viewport_handler{engine.init.get(), &camera_data};
  viewport_handler.Add(&shader);

  viewport_handler.SetSize(
      GetViewport(gamedata.viewport, window_width, window_height));

  LoadWorld(
      engine.file_system.get(),
      &world,
      &integration.Registry(),
      "world.json",
      &templates,
      &duk);

  Uint64 now  = SDL_GetPerformanceCounter();
  Uint64 last = 0;

  SDL_StartTextInput();

  bool running = true;

  int window_mouse_x = 0;
  int window_mouse_y = 0;
  SDL_GetMouseState(&window_mouse_x, &window_mouse_y);
  bool mouse_lmb_down = false;

  integration.BindKeys(&duk, input);

  engine.init->Use2d();

  while(running)
  {
    last           = now;
    now            = SDL_GetPerformanceCounter();
    const float dt = (now - last) * 1.0f / SDL_GetPerformanceFrequency();
    SDL_Event   e;

    if(duk.has_error && !has_crashed)
    {
      has_crashed = true;
      crash_message_string = duk.error;
    }

    engine.imgui->StartNewFrame();

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

    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
      {
        running = false;
      }
      if(engine.HandleResize(e, &window_width, &window_height))
      {
        viewport_handler.SetSize(
            GetViewport(gamedata.viewport, window_width, window_height));
      }

      if(has_crashed)
      {
        engine.imgui->ProcessEvents(&e);
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
      engine.init->ClearScreen(Color::Black);
      viewport_handler.SetSize(
          GetViewport(gamedata.viewport, window_width, window_height), false);
    }

    if(has_crashed)
    {
      // todo: fix crash rendering, perhaps by using a debug gui or imgui
      // nothing much is required just a better overflow detection
      // when rendering the error, perhaps making the error more visible
      // though clicking around and debugging might be useful...
      engine.init->ClearScreen(Color::CornflowerBlue);

      if(BeginFixedOverlay(ImguiCorner::Center, "Crashed"))
      {
        ImGui::TextDisabled("%s", crash_message_string.c_str());
        if(ImGui::Button("Quit"))
        {
          running = false;
        }
        ImGui::End();
      }
    }
    else
    {
      engine.init->ClearScreen(clear_color);
      world.Draw(&renderer);
    }

    engine.imgui->Render();
    SDL_GL_SwapWindow(engine.window->window);

    world.reg.RemoveRemoved();
  }

  return 0;
}
