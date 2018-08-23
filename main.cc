#include <iostream>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>

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

    SetupStyle(Solarized{true});
  }

  void
  SetupStyle(const Solarized& s, ImGuiStyle* dst = nullptr)
  {
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();

    auto C = [](Color color, float a = 1.0f) -> ImVec4 {
      const int r = (color >> 24) & 0xff;
      const int g = (color >> 16) & 0xff;
      const int b = (color >> 8) & 0xff;
      return ImVec4{r / 255.0f, g / 255.0f, b / 255.0f, a};
    };

    style->FrameBorderSize   = 1.0f;
    style->WindowPadding     = ImVec2(10, 10);
    style->FramePadding      = ImVec2(6, 2);
    style->ItemSpacing       = ImVec2(6, 4);
    style->ItemInnerSpacing  = ImVec2(8, 6);
    style->IndentSpacing     = 25.0f;
    style->ScrollbarSize     = 15.0f;
    style->GrabMinSize       = 12.0f;

    style->WindowRounding    = 0.0f;
    style->FrameRounding     = 0.0f;
    style->GrabRounding      = 0.0f;
    style->ScrollbarRounding = 0.0f;
    style->PopupRounding     = 0.0f;
    style->ChildRounding     = 0.0f;

    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                  = C(s.primary_content);
    colors[ImGuiCol_TextDisabled]          = C(s.comments);
    colors[ImGuiCol_WindowBg]              = C(s.background);
    colors[ImGuiCol_ChildWindowBg]         = C(s.background);
    colors[ImGuiCol_PopupBg]               = C(s.background);
    colors[ImGuiCol_Border]                = C(s.primary_content);
    colors[ImGuiCol_BorderShadow]          = C(s.background, 0);
    colors[ImGuiCol_FrameBg]               = C(s.background);
    colors[ImGuiCol_FrameBgHovered]        = C(s.background);
    colors[ImGuiCol_FrameBgActive]         = C(s.background);
    colors[ImGuiCol_TitleBg]               = C(s.background);
    colors[ImGuiCol_TitleBgActive]         = C(s.background_highlight);
    colors[ImGuiCol_TitleBgCollapsed]      = C(s.background);
    colors[ImGuiCol_MenuBarBg]             = C(s.background);
    colors[ImGuiCol_ScrollbarBg]           = C(s.background);
    colors[ImGuiCol_ScrollbarGrab]         = C(s.comments);
    colors[ImGuiCol_ScrollbarGrabHovered]  = C(s.blue);
    colors[ImGuiCol_ScrollbarGrabActive]   = C(s.primary_content);
    colors[ImGuiCol_CheckMark]             = C(s.primary_content);
    colors[ImGuiCol_SliderGrab]            = C(s.primary_content, 0.5f);
    colors[ImGuiCol_SliderGrabActive]      = C(s.primary_content, 0.3f);
    colors[ImGuiCol_Button]                = C(s.background);
    colors[ImGuiCol_ButtonHovered]         = C(s.background_highlight);
    colors[ImGuiCol_ButtonActive]          = C(s.background_highlight);
    colors[ImGuiCol_Header]                = C(s.background_highlight);
    colors[ImGuiCol_HeaderHovered]         = C(s.background_highlight);
    colors[ImGuiCol_HeaderActive]          = C(s.background_highlight);
    colors[ImGuiCol_Separator]             = C(s.comments);
    colors[ImGuiCol_SeparatorHovered]      = C(s.background_highlight);
    colors[ImGuiCol_SeparatorActive]       = C(s.primary_content);
    colors[ImGuiCol_ResizeGrip]            = C(s.comments);
    colors[ImGuiCol_ResizeGripHovered]     = C(s.blue);
    colors[ImGuiCol_ResizeGripActive]      = C(s.primary_content);
    colors[ImGuiCol_PlotLines]             = C(s.comments);
    colors[ImGuiCol_PlotLinesHovered]      = C(s.primary_content);
    colors[ImGuiCol_PlotHistogram]         = C(s.yellow, 0.9f);
    colors[ImGuiCol_PlotHistogramHovered]  = C(s.yellow);
    colors[ImGuiCol_TextSelectedBg]        = C(s.background_highlight);
    colors[ImGuiCol_DragDropTarget]        = C(s.blue);
    colors[ImGuiCol_NavHighlight]          = C(s.red);
    colors[ImGuiCol_NavWindowingHighlight] = C(s.background);
    colors[ImGuiCol_NavWindowingDimBg]     = C(s.background, 0.6f);
    colors[ImGuiCol_ModalWindowDimBg]      = C(s.background, 0.6f);
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

  // dummy implementations
  void
  Text(int, int, const std::string&, int, int)
  {
  }

  void
  ClearScreen(int)
  {
  }
  void
  FillRect(int, int, int, int, int)
  {
  }

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
  OnInput(SDL_Keycode input, bool was_pressed, float time)
  {
    if(keycode != 0 && input == keycode)
    {
      is_down   = was_pressed;
      time_down = time;
    }
  }

  bool is_black = false;

  int semitone = 0;

  int x = 10;
  int y = 10;
  int w = 10;
  int h = 10;

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
CreatePianoKeysStartingAtC(
    int white_key_width,
    int white_key_height,
    int black_key_width,
    int black_key_height,
    int x,
    int y,
    int spacing,
    int key_offset,
    int semitone_offset)
{
  auto BlackKey =
      [=](int semitone, int x, int y, SDL_Keycode keycode) -> PianoKey {
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

  auto WhiteKey =
      [=](int semitone, int x, int y, SDL_Keycode keycode) -> PianoKey {
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
    const int                      i      = index + key_offset;
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
      WhiteKey(semitone_offset + 0, CalcWhiteX(0), y, KeyIndex(0)),
      WhiteKey(semitone_offset + 2, CalcWhiteX(1), y, KeyIndex(2)),
      WhiteKey(semitone_offset + 4, CalcWhiteX(2), y, KeyIndex(4)),

      WhiteKey(semitone_offset + 5, CalcWhiteX(3), y, KeyIndex(6)),
      WhiteKey(semitone_offset + 7, CalcWhiteX(4), y, KeyIndex(8)),
      WhiteKey(semitone_offset + 9, CalcWhiteX(5), y, KeyIndex(10)),
      WhiteKey(semitone_offset + 11, CalcWhiteX(6), y, KeyIndex(12)),

      // specify black keys after white, since black keys are drawn on top
      BlackKey(semitone_offset + 1, CalcBlackX(0), y, KeyIndex(1)),
      BlackKey(semitone_offset + 3, CalcBlackX(1), y, KeyIndex(3)),
      // no black key here
      BlackKey(semitone_offset + 6, CalcBlackX(3), y, KeyIndex(7)),
      BlackKey(semitone_offset + 8, CalcBlackX(4), y, KeyIndex(9)),
      BlackKey(semitone_offset + 10, CalcBlackX(5), y, KeyIndex(11)),
  };
}

struct PianoInput
{
  std::vector<PianoKey>        keys;
  bool                         octave_shift = false;
  ToneToFrequencyConverter<12> converter;

  void
  Draw(AppBase* canvas, const PianoColorTheme& color_theme)
  {
    for(auto& key : keys)
    {
      key.Draw(canvas, color_theme);
    }
  }

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
    int whitew = 60;
    int whiteh = 200;
    int blackw = 20;
    int blackh = 100;

    int x = 10;
    int y = 40;

    int spacing = 3;

    for(int i = -1; i < 2; i += 1)
    {
      Insert(
          &piano.keys,
          CreatePianoKeysStartingAtC(
              whitew,
              whiteh,
              blackw,
              blackh,
              x + i * (7 * (whitew + spacing)),
              y,
              spacing,
              1 + 14 * i,
              12 * i));
    }
  }

  void
  Draw() override
  {
    const auto colors = Solarized{light_ui};

    ImGui::ShowDemoWindow();
    if(ImGui::Begin("ImGui Style"))
    {
      ImGui::ShowStyleSelector("Style");
      ImGui::ShowStyleEditor();
    }
    ImGui::End();

    piano.Draw(this, C(colors));

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
      SetupStyle(Solarized{light_ui});
    }

    if(key == SDLK_SPACE && !down)
    {
      osc = static_cast<OscilatorType>(
          (static_cast<int>(osc) + 1) % static_cast<int>(OscilatorType::Max));
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
