#include <iostream>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include "imgui.h"
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
      , sample_frequency(44100)
      , time(0)
      , samples_consumed(0)
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

 public:
  SDL_Window*   window;
  SDL_GLContext gl_context;
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

struct PianoKey
{
  void
  OnInput(SDL_Keycode input, bool was_pressed, float time)
  {
    if(keycode != 0 && input == keycode)
    {
      is_down   = was_pressed;
      time_down = time;
    }
  }

  int semitone = 0;

  float time_down = 0;

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

std::vector<PianoKey>
CreatePianoKeysStartingAtC(int key_offset, int semitone_offset)
{
  auto BlackKey = [](int semitone, SDL_Keycode keycode) -> PianoKey {
    PianoKey k;
    k.semitone = semitone;
    k.keycode  = keycode;
    return k;
  };

  auto WhiteKey = [](int semitone, SDL_Keycode keycode) -> PianoKey {
    PianoKey k;
    k.semitone = semitone;
    k.keycode  = keycode;
    return k;
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

  return {
      WhiteKey(semitone_offset + 0, KeyIndex(0)),
      WhiteKey(semitone_offset + 2, KeyIndex(2)),
      WhiteKey(semitone_offset + 4, KeyIndex(4)),

      WhiteKey(semitone_offset + 5, KeyIndex(6)),
      WhiteKey(semitone_offset + 7, KeyIndex(8)),
      WhiteKey(semitone_offset + 9, KeyIndex(10)),
      WhiteKey(semitone_offset + 11, KeyIndex(12)),

      // specify black keys after white, since black keys are drawn on top
      BlackKey(semitone_offset + 1, KeyIndex(1)),
      BlackKey(semitone_offset + 3, KeyIndex(3)),
      // no black key here
      BlackKey(semitone_offset + 6, KeyIndex(7)),
      BlackKey(semitone_offset + 8, KeyIndex(9)),
      BlackKey(semitone_offset + 10, KeyIndex(11)),
  };
}

struct PianoInput
{
  std::vector<PianoKey>        keys;
  bool                         octave_shift = false;
  ToneToFrequencyConverter<12> converter;

  void
  OnInput(SDL_Keycode input, Uint16, bool down, float time)
  {
    for(auto& key : keys)
    {
      key.OnInput(input, down, time);
    }

    if(input == SDLK_LSHIFT)
    {
      octave_shift = down;
    }
  }

  PianoOutput
  GetAudioOutput()
  {
    bool down     = false;
    int  semitone = 0;

    float time = -1;

    for(auto& key : keys)
    {
      if(key.is_down)
      {
        // only get the latest key
        if(time < key.time_down)
        {
          time     = key.time_down;
          semitone = key.semitone;
          down     = true;
        }
      }
    }

    const int octave_shift_semitones = octave_shift ? 12 : 0;

    // 3 - because our converter starts at A, and the piano starts at C
    // and C is 3 semitones up from A, we could calculate a new base frequency
    // but I'm too lazy for that.
    return PianoOutput{
        converter.GetFrequency(semitone + 3 + octave_shift_semitones),
        down ? 1.0f : 0.0f};
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

class App : public AppBase
{
 public:
  PianoInput piano;

  App()
  {
    for(int i = -1; i < 2; i += 1)
    {
      Insert(&piano.keys, CreatePianoKeysStartingAtC(1 + 14 * i, 12 * i));
    }
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

    // musik maskin main window
    if(ImGui::Begin("Main"))
    {
      ImGui::SliderFloat("master", &master, 0.0f, 1.0f);

      imgui::Knob("Master", &master, 0.0f, 1.0f);

      if(ImGui::BeginCombo("Oscilator", ToString(osc).c_str()))
      {
        for(int i = 0; i < static_cast<int>(OscilatorType::Max); i += 1)
        {
          const auto o = static_cast<OscilatorType>(i);
          if(ImGui::Selectable(ToString(o).c_str(), osc == o))
          {
            osc = o;
          }
        }
        ImGui::EndCombo();
      }

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
    return master * amplitude * RunOscilator(frequency, time, osc);
  }

  void
  OnKey(SDL_Keycode key, Uint16 mod, bool down, float time)
  {
    if(key == SDLK_TAB && !down)
    {
      light_ui = !light_ui;
      SetupSolarized(light_ui);
    }

    piano.OnInput(key, mod, down, time);
    const auto out = piano.GetAudioOutput();
    frequency      = out.frequency;
    amplitude      = out.amplitude;
  }

 private:
  bool          light_ui  = true;
  float         frequency = 0;
  float         amplitude = 0;
  float         master    = 0.5f;
  OscilatorType osc       = OscilatorType::Sine;
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
        ((current_time - last_time) * 1000.0f / SDL_GetPerformanceFrequency());

    time += dt;

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
            app.OnKey(
                event.key.keysym.sym,
                event.key.keysym.mod,
                event.type == SDL_KEYDOWN,
                time);
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
