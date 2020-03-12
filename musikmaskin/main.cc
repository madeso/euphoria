#include "core/mat4.h"
#include "core/random.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_path.h"
#include "core/os.h"
#include "core/range.h"
#include "core/camera.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"

#include "render/init.h"
#include "render/debuggl.h"
#include "render/materialshader.h"
#include "render/compiledmesh.h"
#include "render/texturecache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/materialshadercache.h"
#include "render/defaultfiles.h"


#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_ext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"
#include "window/canvas.h"
#include "window/key.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>


#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "core/history.h"

#include "window/imgui_extra.h"
#include "window/timgui.h"

#include "minsynth/synth.h"

#ifdef max
#undef max
#endif

#include "euph_generated_config.h"

#include "RtMidi.h"

using namespace euphoria::core;
using namespace euphoria::window;
using namespace euphoria::minsynth;

LOG_SPECIFY_DEFAULT_LOGGER("musikmaskin")

class AppBase
{
public:
    AppBase() : ok(true)
    {
        SetupWindow("musik maskin");
        SetupAudioCallbacks();
    }

    void
    SetupWindow(const std::string&)
    {
        // Setup style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();
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
            LOG_ERROR("Failed to setup audio: {0}", SDL_GetError());
            ok = false;
        }

        audio_opened = true;

        int i, count = SDL_GetNumAudioDevices(0);
        for(i = 0; i < count; ++i)
        {
            LOG_INFO
            (
                "Audio device {0}: {1}",
                i,
                SDL_GetAudioDeviceName(i, 0)
            );
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

    virtual ~AppBase()
    {
        if(audio_opened)
        {
            SDL_CloseAudio();
        }
    }

    virtual void
    Draw() = 0;

    virtual float
    SynthSample(float time)
            = 0;

    void
    OnRender()
    {
        Draw();
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
    bool audio_opened = false;
    float max_sample_time     = 0;
    int   sample_frequency    = 44100;
    float audio_callback_time = 0;
    int   samples_consumed    = 0;

public:
    SDL_Window*   window;
    SDL_GLContext gl_context;
};


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
                LOG_INFO("Midi port {0}: {1}", i, name);
            }

            midi->openPort(port_number);
            LOG_INFO("Using port {0}", midi->getPortName(port_number));
        }
        catch(const RtMidiError& error)
        {
            error.printMessage();
        }
    }
};


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
    MidiInputNode                midi;
    KeyboardInputNode            piano;
    SingleToneNode               single_tone;
    ArpegiatorNode               arp;
    ToneToFrequencyConverterNode ttf;
    OscilatorNode                oscilator;
    ScalerEffect                 scaler;
    VolumeNode                   master;

    std::vector<Node*> nodes;

    float current_time = 0;

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
            ImGui::Text("Time: %.1f", current_time);
            ImGui::Text("Sample time: %.1f", max_sample_time);
            ImGui::Text("Max diff: %.1f", max_diff);
            const auto f = current_time - audio_callback_time;
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

            CustomDropdown("Tuning", &ttf.tuning, Tuning::Max, [](auto t) {
                return ToString(t);
            });

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
                    "Oscilator",
                    &oscilator.oscilator,
                    OscilatorType::Max,
                    [](auto t) { return ToString(t); });

            CustomDropdown(
                    "Chord emulation",
                    &piano.chords_emulation,
                    ChordEmulation::Max,
                    [](auto t) { return ToString(t); });

            ImGui::InputInt("Times", &scaler.times, 1, 5);

            ImGui::InputInt("Arp octaves", &arp.octaves);
            CustomDropdown("Arp mode", &arp.mode, ArpMode::MAX, [](auto t) {
                return ToString(t);
            });
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
    Update(float dt)
    {
        for(auto* node: nodes)
        {
            node->Update(dt, current_time);
        }
    }

    void
    OnKey(Key key, bool down)
    {
        piano.OnInput(key, down, current_time);
    }
};

int
main(int argc, char** argv)
{
    Engine engine;

    if (const auto r = engine.Setup(argparse::Arguments::Extract(argc, argv)); r != 0)
    {
        return r;
    }

    int window_width  = 1280;
    int window_height = 720;

    if(!engine.CreateWindow("Musikmaskin", window_width, window_height, true))
    {
        return -1;
    }

    App app;
    app.Start();

    bool running = true;

    //////////////////////////////////////////////////////////////////////////////
    // main loop

    float  time         = 0;
    Uint64 current_time = SDL_GetPerformanceCounter();
    Uint64 last_time    = 0;

    while(running)
    {
        last_time    = current_time;
        current_time = SDL_GetPerformanceCounter();

        const auto dt
                = (static_cast<float>(current_time - last_time)
                   / SDL_GetPerformanceFrequency());

        time += dt;
        app.current_time = time;


        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            engine.imgui->ProcessEvents(&e);

            if(engine.HandleResize(e, &window_width, &window_height))
            {
            }

            // key handling
            switch(e.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if(!ImGui::GetIO().WantCaptureKeyboard)
                {
                    if(e.key.repeat == 0)
                    {
                        app.OnKey(ToKey(e.key.keysym), e.type == SDL_KEYDOWN);
                    }
                }
                break;
            default: break;
            }

            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default: break;
            }
        }

        app.Update(dt);

        engine.imgui->StartNewFrame();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", "Ctrl+Q"))
                {
                    running = false;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        app.OnRender();
        // ImGui::End();

        engine.init->ClearScreen(Color::LightGray);
        engine.imgui->Render();

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
