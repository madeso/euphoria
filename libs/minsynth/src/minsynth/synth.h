#pragma once

#include <string>
#include <vector>
#include <map>

#include "core/key.h"
#include "core/noncopyable.h"


namespace euphoria::minsynth
{
    enum class tuning
    {
        a4,
        boston,
        new_york,
        europe,
        french,
        baroque,
        chorton,
        classical,
        Max
    };


    enum class chord_emulation
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
        Max
    };


    enum class midi_event
    {
        note_off = 0b000,
        note_on = 0b001,
        aftertouch = 0b010,
        control_change = 0b011,
        program_change = 0b100,
        channel_pressure = 0b101,
        pitch_bend = 0b110
    };


    enum class arp_mode
    {
        up,
        down,
        up_down_inclusive,
        up_down_exclusive,
        random,
        random_no_repeat,
        MAX
    };


    enum class oscilator_type
    {
        sine,
        square,
        triangle,
        sawtooth,
        noise,
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
    to_string(tuning t);

    std::string
    to_string(chord_emulation em);


    std::string
    MidiEventToString(midi_event e);

    std::string
    to_string(arp_mode mode);


    std::string
    to_string(oscilator_type osc);

    float
    tuning_to_base_frequency(tuning t);

    float
    tone_to_frequency(int tone, float base_frequency);

    // nodes


    struct node
    {
        node() = default;
        virtual ~node() = default;

        NONCOPYABLE(node);

        virtual void
        update(float dt, float current_time);
    };

    struct tone_taker
    {
        tone_taker() = default;
        virtual ~tone_taker() = default;

        NONCOPYABLE(tone_taker);

        virtual void
        on_tone(int tone, bool down, float time) = 0;
    };

    struct tone_sender
    {
        tone_taker* next_node = nullptr;

        void
        send_tone(int tone, bool down, float time) const;
    };

    struct frequency_taker
    {
        frequency_taker() = default;
        virtual ~frequency_taker() = default;

        NONCOPYABLE(frequency_taker);

        virtual void
        on_frequency_down(int id, float freq, float time) = 0;

        virtual void
        on_frequency_up(int id, float frequency, float time) = 0;
    };

    struct wave_out
    {
        wave_out() = default;
        virtual ~wave_out() = default;

        NONCOPYABLE(wave_out);

        virtual float
        get_output(float time) = 0;
    };

    // Tone -> Frequency
    struct tone_to_frequency_converter_node
        : public tone_taker
        , public node
    {
        tuning tuning = tuning::a4;

        frequency_taker* next = nullptr;

        void
        on_tone(int tone, bool down, float time) override;

        [[nodiscard]] float
        calculate_frequency(int semitone) const;
    };

    struct piano_key
    {
        piano_key(int st, core::key kc, const std::string& n, int octave);

        int semitone;
        core::key keycode;
        std::string name;

        bool octave_shift;
    };

    struct midi_in_node : public virtual node
    {
        tone_taker* tones = nullptr;
        float last_time = 0;
        bool open_virtual_port = false;
        unsigned int port_number = 1;

        static bool
        is_status_message(unsigned char b);

        static void
        debug_callback(double dt, const std::vector<unsigned char>& bytes);

        void
        callback(float dt, const std::vector<unsigned char>& bytes);
    };


    // Node handles input from keyboard. Input -> Tones
    struct keyboard_input_node : public virtual node
    {
        std::vector<piano_key> keys;
        bool octave_shift = false;

        tone_taker* tones = nullptr;

        chord_emulation chords_emulation = chord_emulation::none;

        void
        on_chord
        (
            int base,
            bool was_pressed,
            float time,
            int first,
            int second
        ) const;

        void
        on_chord
        (
            int base,
            bool was_pressed,
            float time,
            const std::vector<int>& integer_notation
        ) const;

        void
        on_input(core::key input, bool was_pressed, float time);
    };


    // Single Tone node, Tone->Tone.
    struct single_tone_node
        : public tone_taker
        , public tone_sender
        , public node
    {
        std::map<int, float> down_tones;

        void
        on_tone(int tone, bool down, float time) override;

        [[nodiscard]] int
        get_current_tone() const;
    };


    struct arpegiator_node
        : public tone_taker
        , public tone_sender
        , public node
    {
        std::map<int, float> down_tones;
        float current_time_in_interval = 0;

        int index = 0;
        std::vector<int> tones;

        arp_mode mode = arp_mode::up;
        int octaves = 3;
        float update_time = 1.0f;
        float tone_time = 0.3f;

        std::map<int, float> active_tones;

        void
        update(float dt, float current_time) override;

        void
        on_tone(int tone, bool down, float time) override;
    };


    float
    run_oscilator(float frequency, float time, oscilator_type osc);


    struct live_frequency
    {
        float frequency;
        float time_start;
    };

    struct dead_frequency
    {
        bool alive;
        float frequency;
        float time_end;
        float scale;
    };


    float
    to01(float lower_bound, float value, float upper_bound);

    struct envelope
    {
        float time_to_start = 0.01f;
        float time_to_end = 0.01f;

        [[nodiscard]] float
        get_live(float wave, float start_time, float current_time) const;

        [[nodiscard]] float
        get_dead(float wave, float end_time, float current_time) const;
    };


    /// Node represents a single Oscilator. Frequency -> WaveOutput
    struct oscilator_node
        : public virtual wave_out
        , public virtual frequency_taker
        , public node
    {
        std::map<int, live_frequency> live;
        std::vector<dead_frequency> dead;
        oscilator_type oscilator = oscilator_type::sawtooth;
        envelope envelope;

        [[nodiscard]] int
        get_total_tones() const;

        [[nodiscard]] int
        get_alive_tones() const;

        [[nodiscard]] int
        get_dead_tones() const;

        void
        update(float dt, float current_time) override;

        void
        on_frequency_down(int id, float freq, float time) override;

        void
        on_frequency_up(int id, float frequency, float time) override;


        float
        get_output(float time) override;
    };


    struct effect : public wave_out
    {
        float
        get_output(float time) override;

        virtual float
        on_wave(float wave) = 0;

        wave_out* in = nullptr;
    };

    struct volume_node
        : public effect
        , public node
    {
        float
        on_wave(float wave) override;

        float volume = 0.5f;
    };

    struct scaler_effect
        : public effect
        , public node
    {
        float
        on_wave(float wave) override;

        int times = 0;
    };


    using keyboard_layout = std::vector<std::vector<core::key>>;


    const keyboard_layout&
    create_qwerty_keyboard_layout();

    void
    setup_qwerty_two_octave_layout
    (
        std::vector<piano_key>* keys,
        int base_octave,
        int octave_offset
    );


}
