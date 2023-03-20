#pragma once



#include <map>

#include "core/key.h"


namespace euphoria::minsynth
{
    enum class Tuning
    {
        a4,
        boston,
        new_york,
        europe,
        french,
        baroque,
        chorton,
        classical,
        max_value
    };


    enum class ChordEmulation
    {
        none,
        major,
        minor,
        diminished,
        augmented,
        major7,
        dominant7,
        augmented7,
        augmented_major7,
        minor_major7,
        max_value
    };


    enum class MidiEvent
    {
        note_off = 0b000,
        note_on = 0b001,
        aftertouch = 0b010,
        control_change = 0b011,
        program_change = 0b100,
        channel_pressure = 0b101,
        pitch_bend = 0b110
    };


    enum class ArpMode
    {
        up,
        down,
        up_down_inclusive,
        up_down_exclusive,
        random,
        random_no_repeat,
        max_value
    };


    enum class OscilatorType
    {
        sine,
        square,
        triangle,
        sawtooth,
        noise,
        max_value
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

    std::string to_string(Tuning t);
    std::string to_string(ChordEmulation em);
    std::string to_string(ArpMode mode);
    std::string to_string(OscilatorType osc);

    std::string from_midi_event_to_string(MidiEvent e);

    float from_tuning_to_base_frequency(Tuning t);

    float from_tone_to_frequency(int tone, float base_frequency);

    // nodes


    struct Node
    {
        Node() = default;
        virtual ~Node() = default;

        Node(const Node&) = delete;
        Node(Node&&) = delete;
        void operator=(const Node&) = delete;
        void operator=(Node&&) = delete;

        virtual void update(float dt, float current_time);
    };

    struct ToneTaker
    {
        ToneTaker() = default;
        virtual ~ToneTaker() = default;

        ToneTaker(const ToneTaker&) = delete;
        ToneTaker(ToneTaker&&) = delete;
        void operator=(const ToneTaker&) = delete;
        void operator=(ToneTaker&&) = delete;

        virtual void on_tone(int tone, bool down, float time) = 0;
    };

    struct ToneSender
    {
        ToneTaker* next_node = nullptr;

        void send_tone(int tone, bool down, float time) const;
    };

    struct FrequencyTaker
    {
        FrequencyTaker() = default;
        virtual ~FrequencyTaker() = default;

        FrequencyTaker(const FrequencyTaker&) = delete;
        FrequencyTaker(FrequencyTaker&&) = delete;
        void operator=(const FrequencyTaker&) = delete;
        void operator=(FrequencyTaker&&) = delete;

        virtual void on_frequency_down(int id, float freq, float time) = 0;
        virtual void on_frequency_up(int id, float frequency, float time) = 0;
    };

    struct WaveOut
    {
        WaveOut() = default;
        virtual ~WaveOut() = default;

        WaveOut(const WaveOut&) = delete;
        WaveOut(WaveOut&&) = delete;
        void operator=(const WaveOut&) = delete;
        void operator=(WaveOut&&) = delete;

        virtual float get_output(float time) = 0;
    };

    // Tone -> Frequency
    struct ToneToFrequencyConverterNode
        : public ToneTaker
        , public Node
    {
        minsynth::Tuning tuning = minsynth::Tuning::a4;
        FrequencyTaker* next = nullptr;

        void on_tone(int tone, bool down, float time) override;

        [[nodiscard]] float calc_frequency(int semitone) const;
    };

    struct PianoKey
    {
        int semitone;
        core::Key keycode;
        std::string name;
        bool octave_shift;

        PianoKey(int st, core::Key kc, const std::string& n, int octave);
    };

    struct MidiInNode : public virtual Node
    {
        ToneTaker* tones = nullptr;
        float last_time = 0;
        bool open_virtual_port = false;
        unsigned int port_number = 1;

        void callback(float dt, const std::vector<unsigned char>& bytes);

        static bool is_status_message(unsigned char b);
        static void debug_callback(double dt, const std::vector<unsigned char>& bytes);
    };


    // Node handles input from keyboard. Input -> Tones
    struct KeyboardInputNode : public virtual Node
    {
        std::vector<PianoKey> keys;
        bool octave_shift = false;
        ToneTaker* tones = nullptr;
        ChordEmulation chords_emulation = ChordEmulation::none;

        void on_input(core::Key input, bool was_pressed, float time);

        void on_chord
        (
            int base,
            bool was_pressed,
            float time,
            int first,
            int second
        ) const;

        void on_chord
        (
            int base,
            bool was_pressed,
            float time,
            const std::vector<int>& integer_notation
        ) const;
    };


    // Single Tone node, Tone->Tone.
    struct SingleToneNode
        : public ToneTaker
        , public ToneSender
        , public Node
    {
        std::map<int, float> down_tones;

        void on_tone(int tone, bool down, float time) override;

        [[nodiscard]] int get_current_tone() const;
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
        ArpMode mode = ArpMode::up;
        int octaves = 3;
        float update_time = 1.0f;
        float tone_time = 0.3f;
        std::map<int, float> active_tones;

        void update(float dt, float current_time) override;
        void on_tone(int tone, bool down, float time) override;
    };


    float run_oscilator(float frequency, float time, OscilatorType osc);


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


    float to01(float lower_bound, float value, float upper_bound);

    struct Envelope
    {
        float time_to_start = 0.01f;
        float time_to_end = 0.01f;

        [[nodiscard]] float get_live(float wave, float start_time, float current_time) const;
        [[nodiscard]] float get_dead(float wave, float end_time, float current_time) const;
    };


    /// Node represents a single Oscilator. Frequency -> WaveOutput
    struct OscilatorNode
        : public virtual WaveOut
        , public virtual FrequencyTaker
        , public Node
    {
        std::map<int, LiveFrequency> live;
        std::vector<DeadFrequency> dead;
        OscilatorType oscilator = OscilatorType::sawtooth;
        minsynth::Envelope envelope;

        [[nodiscard]] int get_total_tones() const;
        [[nodiscard]] int get_alive_tones() const;
        [[nodiscard]] int get_dead_tones() const;

        void update(float dt, float current_time) override;
        void on_frequency_down(int id, float freq, float time) override;
        void on_frequency_up(int id, float frequency, float time) override;
        float get_output(float time) override;
    };


    struct Effect : public WaveOut
    {
        WaveOut* in = nullptr;

        float get_output(float time) override;

        virtual float on_wave(float wave) = 0;
    };

    struct VolumeNode
        : public Effect
        , public Node
    {
        float volume = 0.5f;

        float on_wave(float wave) override;
    };

    struct ScalerEffect
        : public Effect
        , public Node
    {
        int times = 0;

        float on_wave(float wave) override;
    };


    using KeyboardLayout = std::vector<std::vector<core::Key>>;


    const KeyboardLayout& create_qwerty_keyboard_layout();

    void setup_qwerty_two_octave_layout
    (
        std::vector<PianoKey>* keys,
        int base_octave,
        int octave_offset
    );


}
