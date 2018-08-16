#include <iostream>
#include <vector>

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
      , primary_content(
            light ? solarized_light::base00 : solarized_light::base0)
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
  Color primary_content;
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

template <int SemitonesPerOctave>
struct ToneToFrequencyConverter
{
  float octave_base_frequency = 110;  // a2

  ToneToFrequencyConverter()
  {
    const float base =
        std::pow(2.0f, 1.0f / static_cast<float>(SemitonesPerOctave));
    for(int i = 0; i < SemitonesPerOctave; i += 1)
    {
      semitone_data[i] = std::pow(base, static_cast<float>(i));
    }
  }

  float
  GetFrequency(int semitone) const
  {
    float freq    = octave_base_frequency;
    int   octaves = semitone / SemitonesPerOctave;
    for(int i = 0; i < octaves; i += 1)
    {
      freq = freq * 2;
    }
    return freq * semitone_data[semitone % SemitonesPerOctave];
  }

 private:
  float semitone_data[SemitonesPerOctave] = {
      0.0f,
  };
};

struct PianoColorTheme
{
  Color white;
  Color black;
  Color down;
};

PianoColorTheme
C(const Solarized& s)
{
  PianoColorTheme theme;

  theme.white = s.comments;
  theme.black = s.emphasized_content;
  theme.down  = s.blue;

  return theme;
}

struct PianoKey
{
  void
  Draw(AppBase* canvas, const PianoColorTheme& color_theme)
  {
    Color c = is_black ? color_theme.black : color_theme.white;
    if(is_down)
    {
      c = color_theme.down;
    }
    canvas->FillRect(x, y, w, h, c);
  }

  void
  OnInput(SDL_Keycode input, bool was_pressed)
  {
    if(keycode != 0 && input == keycode)
    {
      is_down = was_pressed;
    }
  }

  bool is_black = false;

  int semitone = 0;

  int x = 10;
  int y = 10;
  int w = 10;
  int h = 10;

  SDL_Keycode keycode = 0;

  bool is_down = false;
};

struct PianoOutput
{
  PianoOutput(float freq, float amp)
      : frequency(freq)
      , amplitude(amp)
  {
  }
  float frequency;
  float amplitude;
};

struct PianoInput
{
  std::vector<PianoKey> keys;

  void
  AddPianoKeysStartingAtC(
      int white_key_width,
      int white_key_height,
      int black_key_width,
      int black_key_height,
      int x,
      int y,
      int spacing,
      int key_offset)
  {
    auto BlackKey = [=](
        int semitone, int x, int y, SDL_Keycode keycode) -> PianoKey {
      PianoKey k;
      k.semitone = semitone;
      k.x        = x;
      k.y        = y;
      k.w        = black_key_width;
      k.h        = black_key_height;
      k.is_black = true;
      k.keycode  = keycode;
      return k;
    };

    auto WhiteKey = [=](
        int semitone, int x, int y, SDL_Keycode keycode) -> PianoKey {
      PianoKey k;
      k.semitone = semitone;
      k.x        = x;
      k.y        = y;
      k.w        = white_key_width;
      k.h        = white_key_height;
      k.is_black = false;
      k.keycode  = keycode;
      return k;
    };

    auto CalcWhiteX = [=](int index) -> int {
      return x + (white_key_width + spacing) * index;
    };
    auto CalcBlackX = [=](int index) -> int {
      const auto half_black = black_key_width / 2;
      const auto start_x    = x + white_key_width + spacing / 2;
      return (white_key_width + spacing) * index + start_x - half_black;
    };

    auto KeyIndex = [=](int index) -> SDL_Keycode {
      const std::vector<SDL_Keycode> qwerty = {SDLK_a,
                                               SDLK_z,
                                               SDLK_s,
                                               SDLK_x,
                                               SDLK_d,
                                               SDLK_c,
                                               SDLK_f,
                                               SDLK_v,
                                               SDLK_g,
                                               SDLK_b,
                                               SDLK_h,
                                               SDLK_n,
                                               SDLK_j,
                                               SDLK_m,
                                               SDLK_k,
                                               SDLK_COMMA,
                                               SDLK_l,
                                               SDLK_PERIOD};
      const int i = index + key_offset;
      if(i < 0)
      {
        return 0;
      }
      if(static_cast<size_t>(i) >= qwerty.size())
      {
        return 0;
      }
      return qwerty[i];
    };

    keys = {
        WhiteKey(0, CalcWhiteX(0), y, KeyIndex(0)),
        WhiteKey(2, CalcWhiteX(1), y, KeyIndex(2)),
        WhiteKey(4, CalcWhiteX(2), y, KeyIndex(4)),

        WhiteKey(5, CalcWhiteX(3), y, KeyIndex(6)),
        WhiteKey(7, CalcWhiteX(4), y, KeyIndex(8)),
        WhiteKey(9, CalcWhiteX(5), y, KeyIndex(10)),
        WhiteKey(11, CalcWhiteX(6), y, KeyIndex(12)),

        // specify black keys after white, since black keys are drawn on top
        BlackKey(1, CalcBlackX(0), y, KeyIndex(1)),
        BlackKey(3, CalcBlackX(1), y, KeyIndex(3)),
        // no black key here
        BlackKey(6, CalcBlackX(3), y, KeyIndex(7)),
        BlackKey(8, CalcBlackX(4), y, KeyIndex(9)),
        BlackKey(10, CalcBlackX(5), y, KeyIndex(11)),
    };
  }

  void
  Draw(AppBase* canvas, const PianoColorTheme& color_theme)
  {
    for(auto& key : keys)
    {
      key.Draw(canvas, color_theme);
    }
  }

  void
  OnInput(SDL_Keycode input, Uint16, bool down)
  {
    for(auto& key : keys)
    {
      key.OnInput(input, down);
    }
  }

  ToneToFrequencyConverter<12> converter;

  PianoOutput
  GetAudioOutput()
  {
    bool down     = false;
    int  semitone = 0;

    for(auto& key : keys)
    {
      if(key.is_down)
      {
        semitone = key.semitone;
        down     = true;
      }
    }

    // 3 - because our converter starts at A, and the piano starts at C
    // and C is 3 semitones up from A, we could calculate a new base frequency
    // but I'm too lazy for that.
    return PianoOutput{converter.GetFrequency(semitone + 3),
                       down ? 1.0f : 0.0f};
  }
};

class App : public AppBase
{
 public:
  PianoInput piano;

  App()
  {
    piano.AddPianoKeysStartingAtC(60, 200, 20, 100, 10, 10, 3, 1);
  }

  void
  Draw() override
  {
    const auto colors = Solarized{light_ui};

    ClearScreen(colors.background);

    piano.Draw(this, C(colors));
  }

  float
  SynthSample(float time) override
  {
    return 0.5f * amplitude * sin(frequency * 2.0f * pi * time);
  }

  void
  OnKey(SDL_Keycode key, Uint16 mod, bool down)
  {
    if(key == SDLK_TAB && !down)
    {
      light_ui = !light_ui;
    }

    piano.OnInput(key, mod, down);
    const auto out = piano.GetAudioOutput();
    frequency      = out.frequency;
    amplitude      = out.amplitude;
  }

 private:
  bool  light_ui  = true;
  float frequency = 0;
  float amplitude = 0;
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
