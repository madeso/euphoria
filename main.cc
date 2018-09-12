#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>

#include <glad/glad.h>

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>

#include "imgui_extra.h"

#include "font_noto_sans_display.h"
#include "solarized.h"

const float pi = 3.14159f;

class AppBase
{
 public:
  AppBase()
      : ok(true)
  {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
      SDL_LogError(
          SDL_LOG_CATEGORY_APPLICATION, "Unable to init: %s", SDL_GetError());
      ok = false;
      return;
    }

    SetupWindow("musik maskin");
    SetupAudioCallbacks();
  }

  void
  SetupWindow(const char* const title)
  {
// Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window)
    {
      std::cerr << "Failed to create window.\n";
      ok = false;
      return;
    }

    gl_context = SDL_GL_CreateContext(window);
    if(!gl_context)
    {
      std::cerr << "Failed to init GL context.\n";
      ok = false;
      return;
    }

    SDL_GL_SetSwapInterval(1);  // Enable vsync

    if(!gladLoadGL())
    {
      std::cerr << "Failed to load GL.\n";
      ok = false;
      return;
    }

    std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << "\n";

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    io.Fonts->AddFontFromMemoryCompressedTTF(
        NotoSansDisplay_compressed_data, NotoSansDisplay_compressed_size, 16);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf",
    // 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf",
    // 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // NULL, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != NULL);

    SetupStyle();

    SetupSolarized(true);
  }

  void
  SetupStyle()
  {
    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameBorderSize  = 1.0f;
    style.WindowPadding    = ImVec2(10, 10);
    style.FramePadding     = ImVec2(6, 2);
    style.ItemSpacing      = ImVec2(6, 4);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing    = 25.0f;
    style.ScrollbarSize    = 15.0f;
    style.GrabMinSize      = 12.0f;

    style.WindowRounding    = 0.0f;
    style.FrameRounding     = 0.0f;
    style.GrabRounding      = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ChildRounding     = 0.0f;
  }

  void
  SetupAudioCallbacks()
  {
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

    int i, count = SDL_GetNumAudioDevices(0);
    for(i = 0; i < count; ++i)
    {
      SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
    }
  }

  void
  Start()
  {
    if(ok)
    {
      SDL_PauseAudio(0);
    }
  }

  ~AppBase()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
  };

  virtual void
  Draw() = 0;

  virtual float
  SynthSample(float time) = 0;

  void
  OnRender()
  {
    ImGuiIO&     io          = ImGui::GetIO();
    const ImVec4 clear_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    Draw();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  void
  AudioCallback(Uint8* stream, int bytes)
  {
    const int len    = bytes / 2;
    auto*     output = reinterpret_cast<Sint16*>(stream);

    const Sint16 max_amplitude = 32767;

    for(int i = 0; i < len; i += 1)
    {
      const float sample_time =
          audio_callback_time +
          (i + samples_consumed) / static_cast<float>(sample_frequency);
      auto sample = SynthSample(sample_time);
      if(sample > 1)
      {
        sample = 1;
      }
      if(sample < -1)
      {
        sample = -1;
      }
      output[i]       = static_cast<Sint16>(max_amplitude * sample);
      max_sample_time = sample_time;
    }

    samples_consumed += len;
    while(samples_consumed >= sample_frequency)
    {
      samples_consumed -= sample_frequency;
      audio_callback_time += 1;
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

 protected:
  float max_sample_time     = 0;
  int   sample_frequency    = 44100;
  float audio_callback_time = 0;
  int   samples_consumed    = 0;

 public:
  SDL_Window*   window;
  SDL_GLContext gl_context;
};

// https://pages.mtu.edu/~suits/notefreqs.html
namespace base_frequencies
{
  constexpr float c0           = 16.35f;
  constexpr float a4           = 440.0f;
  constexpr float boston_a4    = 441.0f;
  constexpr float new_york_a4  = 442.0f;
  constexpr float europe_a4    = 443.0f;
  constexpr float french_a4    = 435.0f;
  constexpr float baroque_a4   = 415.0f;
  constexpr float chorton_a4   = 466.0f;  // 460-470
  constexpr float classical_a4 = 430.0f;  // 460-470
}

enum class Tuning
{
  A4,
  Boston,
  NewYork,
  Europe,
  French,
  Baroque,
  Chorton,
  Classical,
  Max
};

std::string
ToString(Tuning t)
{
  switch(t)
  {
    case Tuning::A4:
      return "A4 (ISO 16) 440 Hz";
    case Tuning::Boston:
      return "Boston 441 Hz";
    case Tuning::NewYork:
      return "New York 442 Hz";
    case Tuning::Europe:
      return "Europe 443 Hz";
    case Tuning::French:
      return "French 435 Hz";
    case Tuning::Baroque:
      return "Baroque 415 hz";
    case Tuning::Chorton:
      return "Chorton 466 Hz";
    case Tuning::Classical:
      return "Classical 430 Hz";
    default:
      return "???";
  }
}

float
TuningToBaseFrequency(Tuning t)
{
  switch(t)
  {
    case Tuning::A4:
      return base_frequencies::a4;
    case Tuning::Boston:
      return base_frequencies::boston_a4;
    case Tuning::NewYork:
      return base_frequencies::new_york_a4;
    case Tuning::Europe:
      return base_frequencies::europe_a4;
    case Tuning::French:
      return base_frequencies::french_a4;
    case Tuning::Baroque:
      return base_frequencies::baroque_a4;
    case Tuning::Chorton:
      return base_frequencies::chorton_a4;
    case Tuning::Classical:
      return base_frequencies::classical_a4;
    default:
      return base_frequencies::a4;
  }
}

template <int StepsPerOctave>
struct ToneToFrequencyConverter
{
  constexpr ToneToFrequencyConverter()
  {
    const float base =
        std::pow(2.0f, 1.0f / static_cast<float>(StepsPerOctave));
    for(int i = 0; i < StepsPerOctave; i += 1)
    {
      step_data[i] = std::pow(base, static_cast<float>(i));
    }
  }

  constexpr float
  GetFrequency(int halfstep, float octave_base_frequency) const
  {
    float freq = octave_base_frequency;

    int step = halfstep;

    while(step >= StepsPerOctave)
    {
      freq = freq * 2;
      step -= StepsPerOctave;
    }

    while(step < 0)
    {
      freq = freq / 2;
      step += StepsPerOctave;
    }

    assert(step >= 0 && step < StepsPerOctave);
    const auto rf = freq * step_data[step];
    return rf;
  }

 private:
  float step_data[StepsPerOctave] = {
      0.0f,
  };
};


template <int TonesPerOctave>
float
ToneToFrequency(int tone, float base_frequency)
{
  const static auto converter = ToneToFrequencyConverter<TonesPerOctave>();
  return converter.GetFrequency(tone, base_frequency);
}

float
CrappyCrap7h(int tone, float base_frequency)
{
  // switch from A to C
  int b12 = tone + 9;
  int oct = 0;

  while(b12 >= 12)
  {
    oct += 1;
    b12 -= 12;
  }
  while(b12 < 0)
  {
    oct -= 1;
    b12 += 12;
  }

  int b7 = 0;
  switch(b12)
  {
    case 1:
    case 3:
    case 6:
    case 8:
    case 10:
      // black keys are silent
      return 0;
    case 2:
      b7 = 1;
      break;
    case 4:
      b7 = 2;
      break;
    case 5:
      b7 = 3;
      break;
    case 7:
      b7 = 4;
      break;
    case 9:
      b7 = 5;
      break;
    case 11:
      b7 = 6;
      break;
    default:
      b7 = 0;
      break;
  }

  const auto t = b7 + oct * 7;
  // todo: switch base_frequency to C, as the base freq with A is wrong
  return ToneToFrequency<7>(t, base_frequency);
}

void
ValidateToneFrequencies()
{
  auto Validate = [](float c, float v, const char* cs, const char* vs) {
    if(fabs(c - v) > 0.01)
    {
      std::cerr << cs << " not equal to " << vs << ": " << c << " " << v
                << "\n";
    }
  };
#define VALIDATE(CALL, VALUE) Validate(CALL, VALUE, #CALL, #VALUE)
  VALIDATE(ToneToFrequency<12>(0, base_frequencies::c0), base_frequencies::c0);
  VALIDATE(ToneToFrequency<12>(0, base_frequencies::a4), base_frequencies::a4);
  VALIDATE(ToneToFrequency<12>(-9, base_frequencies::a4), 261.63f);
  VALIDATE(ToneToFrequency<12>(-12, base_frequencies::a4), 220.0f);
  VALIDATE(ToneToFrequency<12>(-12 - 9, base_frequencies::a4), 130.81f);
#undef VALIDATE
}

std::string
NameAndOctave(const std::string& name, int octave)
{
  std::stringstream ss;
  ss << name << octave;
  return ss.str();
}

struct Node
{
  virtual void
  Update(float dt, float current_time)
  {
  }

  virtual ~Node() = default;
};

struct ToneTaker
{
  virtual ~ToneTaker() = default;

  virtual void
  OnTone(int tone, bool down, float time) = 0;
};

struct ToneSender
{
  ToneTaker* NextNode = nullptr;

  void
  SendTone(int tone, bool down, float time)
  {
    if(NextNode)
    {
      NextNode->OnTone(tone, down, time);
    }
  }
};

struct FrequencyTaker
{
  virtual ~FrequencyTaker() = default;

  virtual void
  OnFrequencyDown(int id, float freq, float time) = 0;

  virtual void
  OnFrequencyUp(int id, float frequency, float time) = 0;
};

struct WaveOut
{
  virtual ~WaveOut() = default;

  virtual float
  GetOutput(float time) = 0;
};

// Tone -> Frequency
struct ToneToFrequencyConverterNode : public ToneTaker, public Node
{
  bool   use_western_scale = true;
  Tuning tuning            = Tuning::A4;

  FrequencyTaker* next = nullptr;

  void
  OnTone(int tone, bool down, float time) override
  {
    if(next == nullptr)
    {
      return;
    }

    if(down)
    {
      next->OnFrequencyDown(tone, CalculateFrequency(tone), time);
    }
    else
    {
      next->OnFrequencyUp(tone, CalculateFrequency(tone), time);
    }
  }

  float
  CalculateFrequency(int semitone)
  {
    const float base_freq = TuningToBaseFrequency(tuning);
    const int   tone      = semitone - 9;

    const float freq = use_western_scale ? ToneToFrequency<12>(tone, base_freq)
                                         : CrappyCrap7h(tone, base_freq);

    return freq;
  }
};

using Key = SDL_Keycode;

struct PianoKey
{
  PianoKey(int st, SDL_Keycode kc, const std::string& n, int octave)
      : semitone(st)
      , keycode(kc)
      , name(NameAndOctave(n, octave))
  {
  }

  int         semitone = 0;
  SDL_Keycode keycode  = 0;
  std::string name;

  bool octave_shift = false;
};

enum class ChordEmulation
{
  None,
  Major,
  Minor,
  Diminished,
  Augmented,
  Major7,
  Dominant7,
  Augmented7,
  AugmentedMajor7,
  MinorMajor7,
  Max
};

std::string
ToString(ChordEmulation em)
{
  switch(em)
  {
    case ChordEmulation::None:
      return "None";
    case ChordEmulation::Major:
      return "Major triad";
    case ChordEmulation::Minor:
      return "Minor Triad";
    case ChordEmulation::Diminished:
      return "Diminished triad";
    case ChordEmulation::Augmented:
      return "Augmented triad";
    case ChordEmulation::Major7:
      return "Major 7th";
    case ChordEmulation::Dominant7:
      return "Dominant 7th";
    case ChordEmulation::Augmented7:
      return "Augmented 7th";
    case ChordEmulation::AugmentedMajor7:
      return "Augmented Major 7th";
    case ChordEmulation::MinorMajor7:
      return "Minor major 7th";
    default:
      return "???";
  }
}

// Node handles input from keyboard. Input -> Tones
struct KeyboardInputNode : public virtual Node
{
  std::vector<PianoKey> keys;
  bool                  octave_shift = false;

  ToneTaker* tones = nullptr;

  ChordEmulation chords_emulation = ChordEmulation::None;

  void
  OnChord(int base, bool was_pressed, float time, int first, int second)
  {
    tones->OnTone(base, was_pressed, time);
    tones->OnTone(base + first, was_pressed, time);
    tones->OnTone(base + first + second, was_pressed, time);
  }

  void
  OnChord(
      int base, bool was_pressed, float time, std::vector<int> integer_notation)
  {
    for(auto i : integer_notation)
    {
      tones->OnTone(base + i, was_pressed, time);
    }
  }

  void
  OnInput(SDL_Keycode input, Uint16, bool was_pressed, float time)
  {
    if(tones == nullptr)
    {
      return;
    }

    for(auto& key : keys)
    {
      if(key.keycode != 0 && input == key.keycode)
      {
        if(was_pressed)
        {
          key.octave_shift = octave_shift;
        }
        const auto octave_shift_semitones = key.octave_shift ? 24 : 0;
        const int  tone = key.semitone + octave_shift_semitones;

        const auto minor3rd = 3;
        const auto major3rd = 4;
        switch(chords_emulation)
        {
          case ChordEmulation::None:
            tones->OnTone(tone, was_pressed, time);
            break;
          case ChordEmulation::Major:
            OnChord(tone, was_pressed, time, major3rd, minor3rd);
            break;
          case ChordEmulation::Minor:
            OnChord(tone, was_pressed, time, minor3rd, major3rd);
            break;
          case ChordEmulation::Diminished:
            OnChord(tone, was_pressed, time, minor3rd, minor3rd);
            break;
          case ChordEmulation::Augmented:
            OnChord(tone, was_pressed, time, major3rd, major3rd);
            break;
          case ChordEmulation::Major7:
            OnChord(tone, was_pressed, time, {0, 4, 7, 11});
            break;
          case ChordEmulation::Dominant7:
            OnChord(tone, was_pressed, time, {0, 4, 7, 10});
            break;
          case ChordEmulation::Augmented7:
            OnChord(tone, was_pressed, time, {0, 4, 8, 10});
            break;
          case ChordEmulation::AugmentedMajor7:
            OnChord(tone, was_pressed, time, {0, 4, 8, 11});
            break;
          case ChordEmulation::MinorMajor7:
            OnChord(tone, was_pressed, time, {0, 3, 7, 11});
            break;
          default:
            break;
        }
      }
    }

    if(input == SDLK_LSHIFT || input == SDLK_RSHIFT)
    {
      octave_shift = was_pressed;
    }
  }
};

// Single Tone node, Tone->Tone.
struct SingleToneNode : public ToneTaker, public ToneSender, public Node
{
  std::map<int, float> down_tones;

  void
  OnTone(int tone, bool down, float time) override
  {
    if(down)
    {
      if(!down_tones.empty())
      {
        const auto current_tone = GetCurrentTone();
        SendTone(current_tone, false, time);
      }
      // this will override the old tone and this is "wrong",
      // but we need a better way to handle multiples anyway
      down_tones[tone] = time;

      SendTone(tone, down, time);
    }
    else
    {
      const auto playing_tone = GetCurrentTone();
      down_tones.erase(tone);

      if(playing_tone == tone)
      {
        // this means the best match stopped playing

        SendTone(tone, false, time);
        if(!down_tones.empty())
        {
          const auto new_tone = GetCurrentTone();
          SendTone(new_tone, true, time);
        }
      }
    }
  }

  int
  GetCurrentTone() const
  {
    bool  has_tone = false;
    int   tone     = 0;
    float time     = 0;
    for(const auto k : down_tones)
    {
      if(has_tone)
      {
        if(k.second > time)
        {
          time = k.second;
          tone = k.first;
        }
      }
      else
      {
        has_tone = true;
        time     = k.second;
        tone     = k.first;
      }
    }

    assert(has_tone);
    return tone;
  }
};

enum class ArpMode
{
  Up,
  Down,
  UpDownInclusive,
  UpDownExclusive,
  Random,
  RandomNoRepeat,
  MAX
};

std::string
ToString(ArpMode mode)
{
  switch(mode)
  {
    case ArpMode::Up:
      return "Up";
    case ArpMode::Down:
      return "Down";
    case ArpMode::UpDownInclusive:
      return "Up/Down (inclusive)";
    case ArpMode::UpDownExclusive:
      return "Up/Down (exclusive)";
    case ArpMode::Random:
      return "Random";
    case ArpMode::RandomNoRepeat:
      return "Random (no repeat)";
    default:
      return "???";
  }
}

struct ArpegiatorNode : public ToneTaker, public ToneSender, public Node
{
  std::map<int, float> down_tones;
  float t = 0;

  int              index = 0;
  std::vector<int> tones;

  ArpMode mode        = ArpMode::Up;
  int     octaves     = 3;
  float   update_time = 1.0f;
  float   tone_time   = 0.3f;

  std::map<int, float> active_tones;

  void
  Update(float dt, float current_time) override
  {
    t += dt;
    while(t > update_time)
    {
      t -= update_time;
      if(!tones.empty())
      {
        const int size = tones.size();

        if(mode == ArpMode::Random || mode == ArpMode::RandomNoRepeat)
        {
          const int last_index = index;
          index                = rand() % size;
          if(mode == ArpMode::RandomNoRepeat && tones.size() > 1)
          {
            while(index == last_index)
            {
              index = rand() % size;
            }
          }
        }
        else
        {
          index = (index + 1) % size;
        }
        SendTone(tones[index], true, current_time);
        active_tones[tones[index]] = current_time + tone_time;
      }
    }

    std::vector<int> erase;
    for(auto& t : active_tones)
    {
      if(t.second < current_time)
      {
        SendTone(t.first, false, t.second);
        erase.emplace_back(t.first);
      }
    }
    for(auto t : erase)
    {
      active_tones.erase(t);
    }
  }

  void
  OnTone(int tone, bool down, float time) override
  {
    if(down)
    {
      down_tones[tone] = time;
    }
    else
    {
      down_tones.erase(tone);
    }

    tones.resize(0);
    if(!down_tones.empty())
    {
      std::set<int> tt;
      for(const auto& t : down_tones)
      {
        for(int i = 0; i < octaves; i += 1)
        {
          tt.insert(t.first + i * 12);
        }
      }
      switch(mode)
      {
        case ArpMode::Up:
          tones.insert(tones.begin(), tt.begin(), tt.end());
          break;
        case ArpMode::Down:
          tones.insert(tones.begin(), tt.rbegin(), tt.rend());
          break;
        case ArpMode::UpDownInclusive:
          tones.insert(tones.begin(), tt.begin(), tt.end());
          tones.insert(tones.begin(), tt.rbegin(), tt.rend());
          break;
        case ArpMode::UpDownExclusive:
          tones.insert(tones.begin(), tt.begin(), tt.end());
          if(tt.size() > 2)
          {
            tt.erase(tt.begin());
            std::set<int>::iterator it = tt.end();
            --it;
            tt.erase(it);
            tones.insert(tones.begin(), tt.rbegin(), tt.rend());
          }
          break;
        default:
          tones.insert(tones.begin(), tt.begin(), tt.end());
          break;
      }
      index = index % tones.size();
    }
  }
};

template <typename T>
void
Insert(std::vector<T>* to, const std::vector<T>& from)
{
  for(const auto& t : from)
  {
    to->push_back(t);
  }
}

enum class OscilatorType
{
  Sine,
  Square,
  Triangle,
  Sawtooth,
  Noise,
  Max
};

std::string
ToString(OscilatorType osc)
{
  switch(osc)
  {
    case OscilatorType::Sine:
      return "Sine";
    case OscilatorType::Square:
      return "Square";
    case OscilatorType::Triangle:
      return "Triangle";
    case OscilatorType::Sawtooth:
      return "Sawtooth";
    case OscilatorType::Noise:
      return "Noise";
    default:
      return "?";
  }
}

float
RunOscilator(float frequency, float time, OscilatorType osc)
{
  const float sine = sin(frequency * 2.0f * pi * time);
  switch(osc)
  {
    case OscilatorType::Sine:
      return sine;
    case OscilatorType::Square:
      return sine > 0 ? 1 : -1;
    case OscilatorType::Triangle:
      return asin(sine) * (2 / pi);
    case OscilatorType::Sawtooth:
      return (2 / pi) * (frequency * pi * fmodf(time, 1 / frequency) - pi / 2);
    case OscilatorType::Noise:
      // todo: use the improved c++ random library
      // todo: and also add perlin noise?
      return 2 * (static_cast<float>(rand()) / RAND_MAX) - 1;
    default:
      return 0;
  }
}

struct LiveFrequency
{
  float frequency;
  float time_start;
};

struct DeadFrequency
{
  bool  alive;
  float frequency;
  float time_end;
  float scale;
};

float
to01(float lower_bound, float value, float upper_bound)
{
  return (value - lower_bound) / (upper_bound - lower_bound);
}

struct Envelope
{
  float time_to_start = 0.01f;
  float time_to_end   = 0.01f;

  float
  GetLive(float wave, float start_time, float current_time)
  {
    if(current_time < start_time)
      return 0;
    if(current_time > (start_time + time_to_start))
      return wave;
    return wave * to01(start_time, current_time, start_time + time_to_start);
  }

  float
  GetDead(float wave, float end_time, float current_time)
  {
    if(current_time < end_time)
      return wave;
    if(current_time > (end_time + time_to_end))
      return 0;
    return wave * (1 - to01(end_time, current_time, end_time + time_to_end));
  }
};

/// Node represents a single Oscilator. Frequency -> WaveOutput
struct OscilatorNode : public virtual WaveOut,
                       public virtual FrequencyTaker,
                       public Node
{
  std::map<int, LiveFrequency> live;
  std::vector<DeadFrequency> dead;

  int
  GetTotalTones() const
  {
    return live.size() + dead.size();
  }

  int
  GetAliveTones() const
  {
    return live.size();
  }

  int
  GetDeadTones() const
  {
    return dead.size();
  }

  void
  Update(float dt, float current_time) override
  {
    dead.erase(
        std::remove_if(
            dead.begin(),
            dead.end(),
            [=](const DeadFrequency& df) -> bool {
              return df.time_end + envelope.time_to_end > current_time;
            }),
        dead.end());
  }

  void
  OnFrequencyDown(int id, float freq, float time) override
  {
    live[id] = LiveFrequency{freq, time};
  }

  void
  OnFrequencyUp(int id, float frequency, float time) override
  {
    const auto scale = envelope.GetLive(1.0f, live[id].time_start, time);
    live.erase(id);
    dead.emplace_back(DeadFrequency{true, frequency, time, scale});
  }

  OscilatorType oscilator = OscilatorType::Sawtooth;

  Envelope envelope;

  float
  GetOutput(float time) override
  {
    float value = 0;

    for(const auto& li : live)
    {
      const auto& f = li.second;
      value += envelope.GetLive(
          RunOscilator(f.frequency, time, oscilator), f.time_start, time);
    }

    for(const auto& d : dead)
    {
      value +=
          d.scale *
          envelope.GetDead(
              RunOscilator(d.frequency, time, oscilator), d.time_end, time);
    }

    return value;
  }
};

struct Effect : public WaveOut
{
  float
  GetOutput(float time) override
  {
    if(in == nullptr)
      return 0;

    return OnWave(in->GetOutput(time));
  }

  virtual float
  OnWave(float wave) = 0;

  WaveOut* in = nullptr;
};

struct VolumeNode : public Effect, public Node
{
  float
  OnWave(float wave) override
  {
    return volume * wave;
  }

  float volume = 0.5f;
};

struct ScalerEffect : public Effect, public Node
{
  float
  OnWave(float wave) override
  {
    float      w        = fabs(wave);
    const auto negative = wave < 0;
    for(int i = 0; i < times; i += 1)
    {
      w = w * w;
    }
    return negative ? w * -1 : w;
  }

  int times = 0;
};

std::vector<PianoKey>
OneOctaveOfPianoKeys(
    int octave,
    int semitone_offset,
    Key c,
    Key d,
    Key e,
    Key f,
    Key g,
    Key a,
    Key b,
    Key c_sharp,
    Key d_sharp,
    Key f_sharp,
    Key g_sharp,
    Key a_sharp)
{
  return {
      PianoKey(semitone_offset + 0, c, "C", octave),
      PianoKey(semitone_offset + 1, c_sharp, "C#", octave),
      PianoKey(semitone_offset + 2, d, "D", octave),
      PianoKey(semitone_offset + 3, d_sharp, "D#", octave),
      PianoKey(semitone_offset + 4, e, "E", octave),

      PianoKey(semitone_offset + 5, f, "F", octave),
      PianoKey(semitone_offset + 6, f_sharp, "F#", octave),
      PianoKey(semitone_offset + 7, g, "G", octave),
      PianoKey(semitone_offset + 8, g_sharp, "G#", octave),
      PianoKey(semitone_offset + 9, a, "A", octave),
      PianoKey(semitone_offset + 10, a_sharp, "A#", octave),
      PianoKey(semitone_offset + 11, b, "B", octave),

  };
}

using KeyboardLayout = std::vector<std::vector<SDL_Keycode>>;

std::string
KeyToString(SDL_Keycode k)
{
  if((k >= 'a' && k <= 'z') || (k >= '0' && k <= '9'))
  {
    const auto c = static_cast<char>(k);
    const auto s = std::string(1, toupper(c));
    return s;
  }

  return "";
}

const KeyboardLayout&
KeyboardLayoutQwerty()
{
  static const KeyboardLayout k = {
      {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
      {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
      {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'},
      {'z', 'x', 'c', 'v', 'b', 'n', 'm'},
  };

  return k;
}

void
SetupOneOctaveLayout(
    std::vector<PianoKey>* keys,
    int                    base_octave,
    int                    octave,
    const KeyboardLayout&  k,
    int                    start_row,
    int                    start_col)
{
  const auto K = [&](int x, int y) -> SDL_Keycode {
    const auto wy = start_row - y + 1;
    if(wy < 0 || wy > k.size())
      return 0;
    const auto& r  = k[wy];
    const auto  wx = start_col + x;
    if(wx < 0 || wx > r.size())
      return 0;
    return r[wx];
  };
  const auto W = K;
  const auto B = K;

  Insert(
      keys,
      OneOctaveOfPianoKeys(
          base_octave + octave,
          octave * 12,
          // first 3 white
          W(0, 0),
          W(1, 0),
          W(2, 0),
          // second 4
          W(3, 0),
          W(4, 0),
          W(5, 0),
          W(6, 0),
          // first 2 black
          B(1, 1),
          B(2, 1),
          // second 3
          B(4, 1),
          B(5, 1),
          B(6, 1)));
}

void
DrawKeys(
    const std::vector<PianoKey>& piano,
    const KeyboardLayout&        layout,
    float                        start_x,
    float                        start_y,
    float                        width,
    float                        height,
    float                        spacing)
{
  ImGuiStyle& style     = ImGui::GetStyle();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  float y        = start_y;
  float x_offset = 0;
  for(const auto& row : layout)
  {
    float x = start_x + x_offset;

    for(const auto& key : row)
    {
      const ImVec2 pos{x, y};
      const ImVec2 tone_offset{6, 8};

      const auto found = std::find_if(
          piano.begin(), piano.end(), [=](const PianoKey& p) -> bool {
            return p.keycode == key;
          });

      const auto t =
          ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TextDisabled]);
      const auto c =
          ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBg]);
      const auto tt =
          ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

      const auto key_text = KeyToString(key);

      draw_list->AddRectFilled(pos, pos + ImVec2(width, height), c);
      draw_list->AddText(pos, t, key_text.c_str());

      if(found != piano.end())
      {
        draw_list->AddText(pos + tone_offset, tt, found->name.c_str());
      }

      x += width + spacing;
    }

    x_offset += (width + spacing) / 2;
    y += height + spacing;
  }
}

void
SetupQwertyTwoOctaveLayout(
    std::vector<PianoKey>* keys, int base_octave, int octave_offset)
{
  SetupOneOctaveLayout(
      keys, base_octave, 0 + octave_offset, KeyboardLayoutQwerty(), 0, 3);
  SetupOneOctaveLayout(
      keys, base_octave, 1 + octave_offset, KeyboardLayoutQwerty(), 2, 0);
}

template <typename T>
void
CustomDropdown(const char* name, T* current, T max)
{
  if(ImGui::BeginCombo(name, ToString(*current).c_str()))
  {
    for(int i = 0; i < static_cast<int>(max); i += 1)
    {
      const auto o = static_cast<T>(i);
      if(ImGui::Selectable(ToString(o).c_str(), *current == o))
      {
        *current = o;
      }
    }
    ImGui::EndCombo();
  }
}

template <typename T>
struct History
{
  explicit History(int s)
      : max(s)
  {
  }

  void
  Push(const T& t)
  {
    if(d.size() == max)
    {
      d.erase(d.begin());
    }
    d.push_back(t);
  }

  const T*
  data() const
  {
    assert(!d.empty());
    return &d[0];
  }

  int
  size() const
  {
    return d.size();
  }

  std::vector<T> d;
  int            max;
};

class App : public AppBase
{
 public:
  KeyboardInputNode            piano;
  SingleToneNode               single_tone;
  ArpegiatorNode               arp;
  ToneToFrequencyConverterNode ttf;
  OscilatorNode                oscilator;
  ScalerEffect                 scaler;
  VolumeNode                   master;

  std::vector<Node*> nodes;

  float time = 0;

  App()
  {
    SetupQwertyTwoOctaveLayout(&piano.keys, 4, -2);

    piano.tones          = &arp;
    arp.NextNode         = &ttf;
    single_tone.NextNode = &ttf;
    ttf.next             = &oscilator;
    scaler.in            = &oscilator;
    master.in            = &scaler;

    nodes.emplace_back(&piano);
    nodes.emplace_back(&single_tone);
    nodes.emplace_back(&arp);
    nodes.emplace_back(&ttf);
    nodes.emplace_back(&oscilator);
    nodes.emplace_back(&scaler);
    nodes.emplace_back(&master);
  }

  void
  Draw() override
  {
    // imgui: demo window
    ImGui::ShowDemoWindow();

    // imgui: style window
    if(ImGui::Begin("ImGui Style"))
    {
      ImGui::ShowStyleEditor();
    }
    ImGui::End();

    // musik maskin ui
    if(ImGui::Begin("UI"))
    {
      if(ImGui::Button("Light"))
      {
        ImGui::StyleColorsLight();
      }
      ImGui::SameLine();
      if(ImGui::Button("Dark"))
      {
        ImGui::StyleColorsDark();
      }

      ImGui::Text("Solarized");
      if(ImGui::Button("Light##solarized"))
      {
        SetupSolarized(true);
      }
      ImGui::SameLine();
      if(ImGui::Button("Dark##solarized"))
      {
        SetupSolarized(false);
      }
    }
    ImGui::End();

    // keyboard input
    if(ImGui::Begin("Keyboard"))
    {
      if(imgui::CanvasBegin(ImVec4(0, 0, 0, 0.5f), "canvas_piano"))
      {
        const auto      p       = ImGui::GetCursorScreenPos();
        constexpr float keysize = 30;
        DrawKeys(
            piano.keys,
            KeyboardLayoutQwerty(),
            p.x + 10,
            p.y + 10,
            keysize,
            keysize,
            3);
      }
      imgui::CanvasEnd();
    }
    ImGui::End();

    // musik maskin main window
    if(ImGui::Begin("Main"))
    {
      static History<float> time_history(100);
      static float          max_diff = 0;
      ImGui::Text(
          "Tones: %d, %d alive and %d dead",
          oscilator.GetTotalTones(),
          oscilator.GetAliveTones(),
          oscilator.GetDeadTones());
      ImGui::Text("Time: %.1f", time);
      ImGui::Text("Sample time: %.1f", max_sample_time);
      ImGui::Text("Max diff: %.1f", max_diff);
      const auto f = time - audio_callback_time;
      time_history.Push(f);
      if(f > max_diff)
      {
        max_diff = f;
      }
      ImGui::PlotLines(
          "Callback diff", time_history.data(), time_history.size());
      // ImGui::Text("Callback time: %.1f", time - audio_callback_time);

      // horizontal line?

      ImGui::SliderFloat("master", &master.volume, 0.0f, 1.0f);

      imgui::Knob("Master", &master.volume, 0.0f, 1.0f);

      ImGui::Checkbox("Western", &ttf.use_western_scale);

      CustomDropdown("Tuning", &ttf.tuning, Tuning::Max);

      ImGui::DragFloat(
          "Time to start",
          &oscilator.envelope.time_to_start,
          0.01f,
          0.0f,
          1.0f);
      ImGui::DragFloat(
          "Time to end", &oscilator.envelope.time_to_end, 0.01f, 0.0f, 1.0f);

      CustomDropdown("Oscilator", &oscilator.oscilator, OscilatorType::Max);

      CustomDropdown(
          "Chord emulation", &piano.chords_emulation, ChordEmulation::Max);

      ImGui::InputInt("Times", &scaler.times, 1, 5);

      ImGui::InputInt("Arp octaves", &arp.octaves);
      CustomDropdown("Arp mode", &arp.mode, ArpMode::MAX);
      imgui::Knob("Update time", &arp.update_time, 0, 1);
      ImGui::SameLine();
      imgui::Knob("Tone time", &arp.tone_time, 0, 1);

      {
        ImGui::BeginChild("audio devices", ImVec2(0, 0), true);
        int i, count = SDL_GetNumAudioDevices(0);

        for(i = 0; i < count; ++i)
        {
          ImGui::Text("%s", SDL_GetAudioDeviceName(i, 0));
        }
        ImGui::EndChild();
      }
    }

    ImGui::End();
  }

  float
  SynthSample(float time) override
  {
    return master.GetOutput(time);
  }

  void
  Update(float dt, float current_time)
  {
    time = current_time;
    for(auto* node : nodes)
    {
      node->Update(dt, current_time);
    }
  }

  void
  OnKey(SDL_Keycode key, Uint16 mod, bool down, float time)
  {
    piano.OnInput(key, mod, down, time);
  }
};

int
main(int argc, char* argv[])
{
  ValidateToneFrequencies();

  App app;
  if(!app.ok)
  {
    return -1;
  }

  SDL_Event event;

  Uint64 current_time = SDL_GetPerformanceCounter();
  Uint64 last_time    = 0;

  app.Start();

  float time = 0;

  bool run = true;
  while(run)
  {
    last_time    = current_time;
    current_time = SDL_GetPerformanceCounter();

    const auto dt =
        (static_cast<float>(current_time - last_time) /
         SDL_GetPerformanceFrequency());

    time += dt;

    app.Update(dt, time);

    while(SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type)
      {
        case SDL_QUIT:
          run = false;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if(!ImGui::GetIO().WantCaptureKeyboard)
          {
            if(event.key.repeat == 0)
            {
              app.OnKey(
                  event.key.keysym.sym,
                  event.key.keysym.mod,
                  event.type == SDL_KEYDOWN,
                  time);
            }
          }
          break;
        default:
          break;
      }

      if(event.type == SDL_WINDOWEVENT &&
         event.window.event == SDL_WINDOWEVENT_CLOSE &&
         event.window.windowID == SDL_GetWindowID(app.window))
      {
        run = false;
      }
    }

    app.OnRender();
  }

  SDL_Quit();

  return 0;
}
