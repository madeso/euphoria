#include <SDL2/SDL.h>
#include "synth.h"

class App
{
 public:
  App()
      : playing(false)
  {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
      throw "Failed to init";
    }

    if(SDL_CreateWindowAndRenderer(
           640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
      SDL_LogError(
          SDL_LOG_CATEGORY_APPLICATION,
          "Couldn't create window and renderer: %s",
          SDL_GetError());
      throw "Failed to set videomode";
    }

    setupAudio();
  }

  ~App()
  {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  };

  void
  Draw()
  {
    if(playing)
    {
      FillRect(25, 25, 100, 150, 0xFFFFFF);
    }
  }

  void
  OnRender()
  {
    SDL_RenderClear(renderer);
    Draw();
    SDL_RenderPresent(renderer);
  }

  void
  OnKey(SDL_Keycode, Uint16, bool down)
  {
    playing = down;
  }

  void
  Audiocallback(Uint8* stream, int len)
  {
    if(playing)
    {
      unsigned int l    = len / 2;
      float*       fbuf = new float[l];
      memset(fbuf, 0, sizeof(fbuf));
      synth.SynthSample(l, fbuf);
      while(l--)
      {
        float f = fbuf[l];
        if(f < -1.0)
          f = -1.0;
        if(f > 1.0)
          f                  = 1.0;
        ((Sint16*)stream)[l] = (Sint16)(f * 32767);
      }
      delete[] fbuf;
    }
    else
      memset(stream, 0, len);
  }

 private:
  void
  FillRect(int x, int y, int w, int h, int color)
  {
    // SDL_Rect rect = {x, y, w, h};
    // SDL_FillRect(display, &rect, color);
  }

  void
  setupAudio();

 private:
  Synth synth;
  bool  playing;

  SDL_Window*   window;
  SDL_Renderer* renderer;
};

static void
SDLAudioCallback(void* userdata, Uint8* stream, int len)
{
  App* app = (App*)userdata;
  app->Audiocallback(stream, len);
}

void
App::setupAudio()
{
  SDL_AudioSpec des;
  des.freq     = 44100;
  des.format   = AUDIO_S16SYS;
  des.channels = 1;
  des.samples  = 512;
  des.callback = SDLAudioCallback;
  des.userdata = this;
  if(0 != SDL_OpenAudio(&des, nullptr))
    throw "Failed to start audio";
  SDL_PauseAudio(0);
}

int
main(int argc, char* argv[])
{
  App       app;
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

  return 1;
}