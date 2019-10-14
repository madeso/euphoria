#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <set>
#include <memory>

#include "core/log.h"

// #include <glad/glad.h>
#include <imgui/imgui.h>
#include "RtMidi.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include <SDL.h>

#include "core/history.h"

#include "window/imgui_extra.h"
#include "window/timgui.h"
#include "window/solarized.h"

#include "minsynth/synth.h"

const float pi = 3.14159f;

#ifdef max
#undef max
#endif

using namespace euphoria::core;
using namespace euphoria::window;
using namespace euphoria::minsynth;

LOG_SPECIFY_DEFAULT_LOGGER("musikmaskin")

class AppBase
{
public:
    AppBase() : ok(true)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            ok = false;
            return;
        }

        SetupWindow("musik maskin");
        SetupAudioCallbacks();
    }

    void
    SetupWindow(const std::string&)
    {
        // Setup style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

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
    };

    virtual void
    Draw() = 0;

    virtual float
    SynthSample(float time)
            = 0;

    void
    OnRender()
    {
        // ImGuiIO&     io          = ImGui::GetIO();
        // const ImVec4 clear_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

        // Start the Dear ImGui frame
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        Draw();

        // Rendering
        ImGui::Render();
        SDL_GL_MakeCurrent(window, gl_context);
        // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // glClear(GL_COLOR_BUFFER_BIT);
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
            const float sample_time
                    = audio_callback_time
                      + (i + samples_consumed)
                                / static_cast<float>(sample_frequency);
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


using Key = SDL_Keycode;


struct MidiInputNode : public euphoria::minsynth::MidiInNode
{
    ToneTaker* tones = nullptr;

    std::unique_ptr<RtMidiIn> midi;

    static void
    StaticCallback(
            double                      deltatime,
            std::vector<unsigned char>* message,
            void*                       user_data)
    {
        auto* self = static_cast<MidiInNode*>(user_data);
        if(message)
        {
            self->Callback(deltatime, *message);
            // self->DebugCallback(deltatime, *message);
        }
    }

    void
    Setup()
    {
        try
        {
            midi.reset(new RtMidiIn());
            midi->setCallback(&StaticCallback, this);

            if(open_virtual_port)
            {
                midi->openVirtualPort();
            }

            for(unsigned int i = 0; i < midi->getPortCount(); i += 1)
            {
                const std::string name = midi->getPortName(i);
                LOG_INFO("Midi port " << i << ": " << name);
            }

            midi->openPort(port_number);
            LOG_INFO("Using port " << midi->getPortName(port_number));
        }
        catch(const RtMidiError& error)
        {
            error.printMessage();
        }
    }
};

////////////////////////////////////////////////////////////////

/*
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
*/


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
    for(const auto& row: layout)
    {
        float x = start_x + x_offset;

        for(const auto& key: row)
        {
            const ImVec2 pos {x, y};
            const ImVec2 tone_offset {6, 8};

            const auto found = std::find_if(
                    piano.begin(), piano.end(), [=](const PianoKey& p) -> bool {
                        return p.keycode == key;
                    });

            const auto t = ImGui::ColorConvertFloat4ToU32(
                    style.Colors[ImGuiCol_TextDisabled]);
            const auto c = ImGui::ColorConvertFloat4ToU32(
                    style.Colors[ImGuiCol_WindowBg]);
            const auto tt = ImGui::ColorConvertFloat4ToU32(
                    style.Colors[ImGuiCol_Text]);

            const auto key_text = ToString(key);

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




class App : public AppBase
{
public:
    MidiInputNode                   midi;
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

        midi.tones           = &ttf;
        piano.tones          = &ttf;
        arp.NextNode         = &ttf;
        single_tone.NextNode = &ttf;
        ttf.next             = &oscilator;
        scaler.in            = &oscilator;
        master.in            = &scaler;

        midi.Setup();

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

            CustomDropdown("Tuning", &ttf.tuning, Tuning::Max, [](auto t){return ToString(t);});

            ImGui::DragFloat(
                    "Time to start",
                    &oscilator.envelope.time_to_start,
                    0.01f,
                    0.0f,
                    1.0f);
            ImGui::DragFloat(
                    "Time to end",
                    &oscilator.envelope.time_to_end,
                    0.01f,
                    0.0f,
                    1.0f);

            CustomDropdown(
                    "Oscilator", &oscilator.oscilator, OscilatorType::Max, [](auto t){return ToString(t);});

            CustomDropdown(
                    "Chord emulation",
                    &piano.chords_emulation,
                    ChordEmulation::Max, [](auto t){return ToString(t);});

            ImGui::InputInt("Times", &scaler.times, 1, 5);

            ImGui::InputInt("Arp octaves", &arp.octaves);
            CustomDropdown("Arp mode", &arp.mode, ArpMode::MAX, [](auto t){return ToString(t);});
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
        for(auto* node: nodes)
        {
            node->Update(dt, current_time);
        }
    }

    void
    OnKey(SDL_Keycode key, bool down, float time)
    {
        piano.OnInput(key, down, time);
    }
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

        const auto dt
                = (static_cast<float>(current_time - last_time)
                   / SDL_GetPerformanceFrequency());

        time += dt;

        app.Update(dt, time);

        while(SDL_PollEvent(&event))
        {
            // ImGui_ImplSDL2_ProcessEvent(&event);
            switch(event.type)
            {
            case SDL_QUIT: run = false; break;
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
            default: break;
            }

            if(event.type == SDL_WINDOWEVENT
               && event.window.event == SDL_WINDOWEVENT_CLOSE
               && event.window.windowID == SDL_GetWindowID(app.window))
            {
                run = false;
            }
        }

        app.OnRender();
    }

    SDL_Quit();

    return 0;
}
