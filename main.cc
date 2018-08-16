#include <SDL2/SDL.h>
const float pi = 3.14159f;

using Color = int;

class AppBase
{
 public:
  AppBase()
      : ok(true)
      , sample_frequency(44100)
      , time(0)
      , samples_consumed(0)
      , window(nullptr)
      , renderer(nullptr)
  {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
      SDL_LogError(
          SDL_LOG_CATEGORY_APPLICATION, "Unable to init: %s", SDL_GetError());
      ok = false;
      return;
    }

    if(SDL_CreateWindowAndRenderer(
           640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
      SDL_LogError(
          SDL_LOG_CATEGORY_APPLICATION,
          "Couldn't create window and renderer: %s",
          SDL_GetError());
      ok = false;
    }

    SDL_AudioSpec spec;
    SDL_memset(&spec, 0, sizeof(spec));
    spec.freq     = sample_frequency;
    spec.format   = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples  = 1024;
    spec.callback = SDLAudioCallback;
    spec.userdata = this;
    if(0 != SDL_OpenAudio(&spec, nullptr))
    {
      SDL_LogError(
          SDL_LOG_CATEGORY_APPLICATION,
          "Failed to setup audio: %s",
          SDL_GetError());
      ok = false;
    }
    else
    {
      SDL_PauseAudio(0);
    }
  }

  ~AppBase()
  {
    if(renderer)
    {
      SDL_DestroyRenderer(renderer);
    }
    if(window)
    {
      SDL_DestroyWindow(window);
    }
  };

  virtual void
  Draw() = 0;

  virtual float
  SynthSample(float time) = 0;

  void
  OnRender()
  {
    if(renderer)
    {
      Draw();
      SDL_RenderPresent(renderer);
    }
  }


 protected:
  void
  ClearScreen(int color)
  {
    SetRenderColor(color);
    SDL_RenderClear(renderer);
  }

  void
  FillRect(int x, int y, int w, int h, Color color)
  {
    SetRenderColor(color);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(renderer, &rect);
  }

 private:
  void
  SetRenderColor(Color color)
  {
    const auto r = static_cast<Uint8>((color & 0xFF000000) >> 24);
    const auto g = static_cast<Uint8>((color & 0x00FF0000) >> 16);
    const auto b = static_cast<Uint8>((color & 0x0000FF00) >> 8);
    const auto a = static_cast<Uint8>((color & 0x000000FF));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
  }

  void
  AudioCallback(Uint8* stream, int bytes)
  {
    const int len    = bytes / 2;
    auto*     output = reinterpret_cast<Sint16*>(stream);

    const Sint16 max_amplitude = 32767;

    for(int i = 0; i < len; i += 1)
    {
      auto sample = SynthSample(
          time + (i + samples_consumed) / static_cast<float>(sample_frequency));
      if(sample > 1)
      {
        sample = 1;
      }
      if(sample < -1)
      {
        sample = -1;
      }
      output[i] = static_cast<Sint16>(max_amplitude * sample);
    }

    samples_consumed += len;
    if(samples_consumed >= sample_frequency)
    {
      samples_consumed -= sample_frequency;
      time += 1;
    }
  }

  static void
  SDLAudioCallback(void* userdata, Uint8* stream, int len)
  {
    auto* app = (AppBase*)userdata;
    app->AudioCallback(stream, len);
  }

 public:
  bool ok;

 private:
  int   sample_frequency;
  float time;
  int   samples_consumed;

  SDL_Window*   window;
  SDL_Renderer* renderer;
};

// solarized light color spec
namespace solarized_light
{
  Color base03  = 0x002B36FF;
  Color base02  = 0x073642FF;
  Color base01  = 0x586E75FF;
  Color base00  = 0x657B83FF;
  Color base0   = 0x839496FF;
  Color base1   = 0x93A1A1FF;
  Color base2   = 0xEEE8D5FF;
  Color base3   = 0xFDF6E3FF;
  Color yellow  = 0xB58900FF;
  Color orange  = 0xCB4B16FF;
  Color red     = 0xDC322FFF;
  Color magenta = 0xD33682FF;
  Color violet  = 0x6C71C4FF;
  Color blue    = 0x268BD2FF;
  Color cyan    = 0x2AA198FF;
  Color green   = 0x859900FF;
}  // namespace solarized_light

struct Solarized
{
  explicit Solarized(bool light)
      : really_strong_border(
            light ? solarized_light::base03 : solarized_light::base3)
      , strong_border(light ? solarized_light::base02 : solarized_light::base2)
      , emphasized_content(
            light ? solarized_light::base01 : solarized_light::base1)
      , body_text(light ? solarized_light::base00 : solarized_light::base0)
      , not_used(light ? solarized_light::base0 : solarized_light::base00)
      , comments(light ? solarized_light::base1 : solarized_light::base01)
      , background_highlight(
            light ? solarized_light::base2 : solarized_light::base02)
      , background(light ? solarized_light::base3 : solarized_light::base03)
      , yellow(solarized_light::yellow)
      , orange(solarized_light::orange)
      , red(solarized_light::red)
      , magenta(solarized_light::magenta)
      , violet(solarized_light::violet)
      , blue(solarized_light::blue)
      , cyan(solarized_light::cyan)
      , green(solarized_light::green)

  {
  }
  Color really_strong_border;
  Color strong_border;
  Color emphasized_content;
  Color body_text;
  Color not_used;
  Color comments;
  Color background_highlight;
  Color background;

  Color yellow;
  Color orange;
  Color red;
  Color magenta;
  Color violet;
  Color blue;
  Color cyan;
  Color green;
};

class App : public AppBase
{
 public:
  void
  Draw() override
  {
    const auto colors = Solarized{light_ui};

    ClearScreen(colors.background);
    if(playing)
    {
      FillRect(25, 25, 100, 150, colors.body_text);
    }
  }

  float
  SynthSample(float time) override
  {
    if(playing)
    {
      return 0.5f * sin(440.0f * 2.0f * pi * time);
    }
    else
    {
      return 0;
    }
  }

  void
  OnKey(SDL_Keycode key, Uint16, bool down)
  {
    if(key == SDLK_TAB && !down)
    {
      light_ui = !light_ui;
    }

    if(key == SDLK_SPACE)
    {
      playing = down;
    }
  }

 private:
  bool light_ui = true;
  bool playing  = false;
};

int
main(int argc, char* argv[])
{
  App app;
  if(!app.ok)
  {
    return -1;
  }

  SDL_Event event;

  bool run = true;
  while(run)
  {
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          run = false;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          app.OnKey(
              event.key.keysym.sym,
              event.key.keysym.mod,
              event.type == SDL_KEYDOWN);
          break;
        default:
          break;
      }
    }

    app.OnRender();
  }

  SDL_Quit();

  return 0;
}
