#include <SDL2/SDL.h>
const float pi = 3.14159f;

class AppBase
{
 public:
  AppBase()
      : ok(true)
      , background_color(0x000000FF)
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
      SetRenderColor(background_color);
      SDL_RenderClear(renderer);
      Draw();
      SDL_RenderPresent(renderer);
    }
  }


 protected:
  void
  SetRenderColor(int color)
  {
    const auto r = static_cast<Uint8>((color & 0xFF000000) >> 24);
    const auto g = static_cast<Uint8>((color & 0x00FF0000) >> 16);
    const auto b = static_cast<Uint8>((color & 0x0000FF00) >> 8);
    const auto a = static_cast<Uint8>((color & 0x000000FF));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
  }

  void
  FillRect(int x, int y, int w, int h, int color)
  {
    SetRenderColor(color);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(renderer, &rect);
  }

  void
  SetBackgroundColor(int color)
  {
    background_color = color;
  }

 private:
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
    AppBase* app = (AppBase*)userdata;
    app->AudioCallback(stream, len);
  }

 public:
  bool ok;

 private:
  int   background_color;
  int   sample_frequency;
  float time;
  int   samples_consumed;

  SDL_Window*   window;
  SDL_Renderer* renderer;
};

class App : public AppBase
{
 public:
  void
  Draw() override
  {
    if(playing)
    {
      FillRect(25, 25, 100, 150, 0xFFFFFFFF);
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
  OnKey(SDL_Keycode, Uint16, bool down)
  {
    playing = down;
  }

 private:
  bool playing = false;
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