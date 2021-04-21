#pragma once

#include <string>
#include <vector>
#include <map>

#include "core/key.h"
#include "core/noncopyable.h"


namespace euphoria::minsynth
{
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


    enum class MidiEvent
    {
        NoteOff = 0b000,
        NoteOn = 0b001,
        Aftertouch = 0b010,
        ControlChange = 0b011,
        ProgramChange = 0b100,
        ChannelPressure = 0b101,
        PitchBend = 0b110
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


    enum class OscilatorType
    {
        Sine,
        Square,
        Triangle,
        Sawtooth,
        Noise,
        Max
    };


    // https://pages.mtu.edu/~suits/notefreqs.html
    namespace base_frequencies
    {
        constexpr float c0 = 16.35f;
        constexpr float a4 = 440.0f;
        constexpr float boston_a4 = 441.0f;
        constexpr float new_york_a4 = 442.0f;
        constexpr float europe_a4 = 443.0f;
        constexpr float french_a4 = 435.0f;
        constexpr float baroque_a4 = 415.0f;
        constexpr float chorton_a4 = 466.0f; // 460-470
        constexpr float classical_a4 = 430.0f; // 460-470
    }

    std::string
    ToString(Tuning t);

    std::string
    ToString(ChordEmulation em);


    std::string
    MidiEventToString(MidiEvent e);

    std::string
    ToString(ArpMode mode);


    std::string
    ToString(OscilatorType osc);

    float
    TuningToBaseFrequency(Tuning t);

    float
    ToneToFrequency(int tone, float base_frequency);

    // nodes


    struct Node
    {
        Node() = default;
        virtual ~Node() = default;

        NONCOPYABLE(Node);

        virtual void
        Update(float dt, float current_time);
    };

    struct ToneTaker
    {
        ToneTaker() = default;
        virtual ~ToneTaker() = default;

        NONCOPYABLE(ToneTaker);

        virtual void
        OnTone(int tone, bool down, float time) = 0;
    };

    struct ToneSender
    {
        ToneTaker* NextNode = nullptr;

        void
        SendTone(int tone, bool down, float time) const;
    };

    struct FrequencyTaker
    {
        FrequencyTaker() = default;
        virtual ~FrequencyTaker() = default;

        NONCOPYABLE(FrequencyTaker);

        virtual void
        OnFrequencyDown(int id, float freq, float time) = 0;

        virtual void
        OnFrequencyUp(int id, float frequency, float time) = 0;
    };

    struct WaveOut
    {
        WaveOut() = default;
        virtual ~WaveOut() = default;

        NONCOPYABLE(WaveOut);

        virtual float
        GetOutput(float time) = 0;
    };

    // Tone -> Frequency
    struct ToneToFrequencyConverterNode
        : public ToneTaker
        , public Node
    {
        Tuning tuning = Tuning::A4;

        FrequencyTaker* next = nullptr;

        void
        OnTone(int tone, bool down, float time) override;

        [[nodiscard]] float
        CalculateFrequency(int semitone) const;
    };

    struct PianoKey
    {
        PianoKey(int st, core::Key kc, const std::string& n, int octave);

        int semitone;
        core::Key keycode;
        std::string name;

        bool octave_shift;
    };

    struct MidiInNode : public virtual Node
    {
        ToneTaker* tones = nullptr;
        float last_time = 0;
        bool open_virtual_port = false;
        unsigned int port_number = 1;

        static bool
        IsStatusMessage(unsigned char b);

        static void
        DebugCallback(double dt, const std::vector<unsigned char>& bytes);

        void
        Callback(float dt, const std::vector<unsigned char>& bytes);
    };


    // Node handles input from keyboard. Input -> Tones
    struct KeyboardInputNode : public virtual Node
    {
        std::vector<PianoKey> keys;
        bool octave_shift = false;

        ToneTaker* tones = nullptr;

        ChordEmulation chords_emulation = ChordEmulation::None;

        void
        OnChord
        (
            int base,
            bool was_pressed,
            float time,
            int first,
            int second
        ) const;

        void
        OnChord
        (
            int base,
            bool was_pressed,
            float time,
            const std::vector<int>& integer_notation
        ) const;

        void
        OnInput(core::Key input, bool was_pressed, float time);
    };


    // Single Tone node, Tone->Tone.
    struct SingleToneNode
        : public ToneTaker
        , public ToneSender
        , public Node
    {
        std::map<int, float> down_tones;

        void
        OnTone(int tone, bool down, float time) override;

        [[nodiscard]] int
        GetCurrentTone() const;
    };


    struct ArpegiatorNode
        : public ToneTaker
        , public ToneSender
        , public Node
    {
        std::map<int, float> down_tones;
        float current_time_in_interval = 0;

        int index = 0;
        std::vector<int> tones;

        ArpMode mode = ArpMode::Up;
        int octaves = 3;
        float update_time = 1.0f;
        float tone_time = 0.3f;

        std::map<int, float> active_tones;

        void
        Update(float dt, float current_time) override;

        void
        OnTone(int tone, bool down, float time) override;
    };


    float
    RunOscilator(float frequency, float time, OscilatorType osc);


    struct LiveFrequency
    {
        float frequency;
        float time_start;
    };

    struct DeadFrequency
    {
        bool alive;
        float frequency;
        float time_end;
        float scale;
    };


    float
    to01(float lower_bound, float value, float upper_bound);

    struct Envelope
    {
        float time_to_start = 0.01f;
        float time_to_end = 0.01f;

        [[nodiscard]] float
        GetLive(float wave, float start_time, float current_time) const;

        [[nodiscard]] float
        GetDead(float wave, float end_time, float current_time) const;
    };


    /// Node represents a single Oscilator. Frequency -> WaveOutput
    struct OscilatorNode
        : public virtual WaveOut
        , public virtual FrequencyTaker
        , public Node
    {
        std::map<int, LiveFrequency> live;
        std::vector<DeadFrequency> dead;
        OscilatorType oscilator = OscilatorType::Sawtooth;
        Envelope envelope;

        [[nodiscard]] int
        GetTotalTones() const;

        [[nodiscard]] int
        GetAliveTones() const;

        [[nodiscard]] int
        GetDeadTones() const;

        void
        Update(float dt, float current_time) override;

        void
        OnFrequencyDown(int id, float freq, float time) override;

        void
        OnFrequencyUp(int id, float frequency, float time) override;


        float
        GetOutput(float time) override;
    };


    struct Effect : public WaveOut
    {
        float
        GetOutput(float time) override;

        virtual float
        OnWave(float wave)
                = 0;

        WaveOut* in = nullptr;
    };

    struct VolumeNode
        : public Effect
        , public Node
    {
        float
        OnWave(float wave) override;

        float volume = 0.5f;
    };

    struct ScalerEffect
        : public Effect
        , public Node
    {
        float
        OnWave(float wave) override;

        int times = 0;
    };


    using KeyboardLayout = std::vector<std::vector<core::Key>>;


    const KeyboardLayout&
    KeyboardLayoutQwerty();

    void
    SetupQwertyTwoOctaveLayout(
            std::vector<PianoKey>* keys,
            int base_octave,
            int octave_offset);


}
