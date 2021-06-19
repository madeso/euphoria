#include "minsynth/synth.h"

#include "core/log.h"
#include "core/assert.h"
#include "core/numeric.h"
#include "core/cint.h"
#include "core/angle.h"

#include <sstream>
#include <cmath>
#include <set>
#include <algorithm>

/*
   Tone names:
   1  2  3  4  5  6  7
   C  D  E  F  G  A  B
   Do Re Mi Fa So La Si
*/

namespace euphoria::minsynth
{
    // todo(Gustav): get a better number here
    constexpr float pi = core::pi;


    std::string
    to_string(tuning t)
    {
        switch(t)
        {
        case tuning::a4: return "A4 (ISO 16) 440 Hz";
        case tuning::boston: return "Boston 441 Hz";
        case tuning::new_york: return "New York 442 Hz";
        case tuning::europe: return "europe 443 Hz";
        case tuning::french: return "French 435 Hz";
        case tuning::baroque: return "baroque 415 hz";
        case tuning::chorton: return "chorton 466 Hz";
        case tuning::classical: return "classical 430 Hz";
        default: return "???";
        }
    }

    std::string
    to_string(chord_emulation em)
    {
        switch(em)
        {
        case chord_emulation::none: return "None";
        case chord_emulation::major: return "Major triad";
        case chord_emulation::minor: return "Minor Triad";
        case chord_emulation::diminished: return "diminished triad";
        case chord_emulation::augmented: return "augmented triad";
        case chord_emulation::major7: return "Major 7th";
        case chord_emulation::dominant7: return "Dominant 7th";
        case chord_emulation::augmented7: return "augmented 7th";
        case chord_emulation::augmented_major7: return "augmented Major 7th";
        case chord_emulation::minor_major7: return "Minor major 7th";
        default: return "???";
        }
    }

    float
    tuning_to_base_frequency(tuning t)
    {
        switch(t)
        {
        case tuning::a4: return base_frequencies::a4;
        case tuning::boston: return base_frequencies::boston_a4;
        case tuning::new_york: return base_frequencies::new_york_a4;
        case tuning::europe: return base_frequencies::europe_a4;
        case tuning::french: return base_frequencies::french_a4;
        case tuning::baroque: return base_frequencies::baroque_a4;
        case tuning::chorton: return base_frequencies::chorton_a4;
        case tuning::classical: return base_frequencies::classical_a4;
        default: return base_frequencies::a4;
        }
    }


    std::string
    midi_event_to_string(midi_event e)
    {
        switch(e)
        {
        case midi_event::note_off: return "Off";
        case midi_event::note_on: return "On";
        case midi_event::aftertouch: return "aftertouch";
        case midi_event::control_change: return "CC";
        case midi_event::program_change: return "PC";
        case midi_event::channel_pressure: return "Channel Pressure";
        case midi_event::pitch_bend: return "pitch_bend";
        }
        return "???";
    }


    std::string
    to_string(arp_mode mode)
    {
        switch(mode)
        {
        case arp_mode::up: return "Up";
        case arp_mode::down: return "Down";
        case arp_mode::up_down_inclusive: return "Up/Down (inclusive)";
        case arp_mode::up_down_exclusive: return "Up/Down (exclusive)";
        case arp_mode::random: return "Random";
        case arp_mode::random_no_repeat: return "Random (no repeat)";
        default: return "???";
        }
    }


    std::string
    to_string(oscilator_type osc)
    {
        switch(osc)
        {
        case oscilator_type::sine: return "Sine";
        case oscilator_type::square: return "Square";
        case oscilator_type::triangle: return "Triangle";
        case oscilator_type::sawtooth: return "sawtooth";
        case oscilator_type::noise: return "noise";
        default: return "?";
        }
    }

    template <int StepsPerOctave>
    struct tone_to_frequency_converter
    {
        constexpr tone_to_frequency_converter()
        {
            const float base
                    = std::pow(2.0f, 1.0f / static_cast<float>(StepsPerOctave));
            for(int i = 0; i < StepsPerOctave; i += 1)
            {
                step_data[i] = std::pow(base, static_cast<float>(i));
            }
        }

        [[nodiscard]] constexpr float
        get_frequency(int halfstep, float octave_base_frequency) const
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

            ASSERT(step >= 0 && step < StepsPerOctave);
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
    template_tone_to_frequency(int tone, float base_frequency)
    {
        const static auto converter = tone_to_frequency_converter<TonesPerOctave>();
        return converter.get_frequency(tone, base_frequency);
    }

    float
    tone_to_frequency(int tone, float base_frequency)
    {
        return template_tone_to_frequency<12>(tone, base_frequency);
    }

    void
    node::update(float /*dt*/, float /*current_time*/)
    {
    }

    void
    tone_sender::send_tone(int tone, bool down, float time) const
    {
        if(next_node != nullptr)
        {
            next_node->on_tone(tone, down, time);
        }
    }


    void
    tone_to_frequency_converter_node::on_tone(int tone, bool down, float time)
    {
        if(next == nullptr)
        {
            return;
        }

        if(down)
        {
            next->on_frequency_down(tone, calculate_frequency(tone), time);
        }
        else
        {
            next->on_frequency_up(tone, calculate_frequency(tone), time);
        }
    }

    float
    tone_to_frequency_converter_node::calculate_frequency(int semitone) const
    {
        const float base_freq = tuning_to_base_frequency(tuning);
        const int tone = semitone - 9; // hrm.... why?

        const float freq = tone_to_frequency(tone, base_freq);

        return freq;
    }

    namespace
    {
        std::string
        name_and_octave(const std::string& name, int octave)
        {
            std::stringstream ss;
            ss << name << octave;
            return ss.str();
        }
    }

    piano_key::piano_key(int st, core::key kc, const std::string& n, int octave)
        : semitone(st)
        , keycode(kc)
        , name(name_and_octave(n, octave))
        , octave_shift(false)
    {
    }


    bool
    midi_in_node::is_status_message(unsigned char b)
    {
        const auto is_status = ((b >> 7) & 0x1) == 1;
        return is_status;
    }

    void
    midi_in_node::debug_callback
    (
        double dt,
        const std::vector<unsigned char>& bytes
    )
    {
        using byte = unsigned char;

        const byte message_mask = 0x7;
        const byte channel_mask = 0xF;

        for(auto b: bytes)
        {
            const auto is_status = midi_in_node::is_status_message(b);
            if(is_status)
            {
                const byte channel = (b >> 0) & channel_mask;
                const byte message = (b >> 4) & message_mask;

                LOG_INFO
                (
                "STAT ({0}) {1}",
                static_cast<unsigned int>(channel),
                midi_event_to_string(static_cast<midi_event>(message))
                );
            }
            else
            {
                LOG_INFO("{0}", static_cast<unsigned int>(b));
            }
        }
        if(!bytes.empty())
        {
            LOG_INFO("stamp = {0}", dt);
        }
    }

    void
    midi_in_node::callback(float dt, const std::vector<unsigned char>& bytes)
    {
        using byte = unsigned char;

        if(bytes.empty())
        {
            return;
        }

        if(!midi_in_node::is_status_message(bytes[0]))
        {
            LOG_ERROR("todo: need to handle data message without status.");
            return;
        }

        const auto bytes_size = bytes.size();

        const byte message_mask = 0x7;
        // const byte channel_mask = 0xF;
        // const byte channel = (bytes[0] >> 0) & channel_mask;
        const byte message = (bytes[0] >> 4) & message_mask;

        const auto event = static_cast<midi_event>(message);
        switch(event)
        {
        case midi_event::note_on:
        case midi_event::note_off: {
            const bool on = event == midi_event::note_on;
            if(bytes_size == 3)
            {
                const auto note = bytes[1];
                const auto velocity = bytes[2];

                if(midi_in_node::is_status_message(note) || midi_in_node::is_status_message(velocity))
                {
                    LOG_ERROR("Unexpected midi command in note on/off data.");
                    return;
                }

                // c3 == midi #60
                if(tones != nullptr)
                {
                    const int tone = note;
                    float time = dt;
                    tones->on_tone(tone - 60, on, time + last_time);
                    LOG_INFO("Time: {0}", time + last_time);
                    last_time += time;
                }
            }
            else
            {
                LOG_ERROR("todo: need to handle note on/off with other sizes.");
                return;
            }
            break;
        }

        default: LOG_ERROR("todo: handle {0}", midi_event_to_string(event)); return;
        }
    }


    void
    keyboard_input_node::on_chord
    (
        int base,
        bool was_pressed,
        float time,
        int first,
        int second
    ) const
    {
        tones->on_tone(base, was_pressed, time);
        tones->on_tone(base + first, was_pressed, time);
        tones->on_tone(base + first + second, was_pressed, time);
    }

    void
    keyboard_input_node::on_chord
    (
        int base,
        bool was_pressed,
        float time,
        const std::vector<int>& integer_notation
    ) const
    {
        for(auto i: integer_notation)
        {
            tones->on_tone(base + i, was_pressed, time);
        }
    }

    void
    keyboard_input_node::on_input(core::key input, bool was_pressed, float time)
    {
        if(tones == nullptr)
        {
            return;
        }

        for(auto& key: keys)
        {
            if(key.keycode != core::key::unbound && input == key.keycode)
            {
                if(was_pressed)
                {
                    key.octave_shift = octave_shift;
                }
                const auto octave_shift_semitones = key.octave_shift ? 24 : 0;
                const int tone = key.semitone + octave_shift_semitones;

                const auto minor3rd = 3;
                const auto major3rd = 4;
                switch(chords_emulation)
                {
                case chord_emulation::none:
                    tones->on_tone(tone, was_pressed, time);
                    break;
                case chord_emulation::major:
                    on_chord(tone, was_pressed, time, major3rd, minor3rd);
                    break;
                case chord_emulation::minor:
                    on_chord(tone, was_pressed, time, minor3rd, major3rd);
                    break;
                case chord_emulation::diminished:
                    on_chord(tone, was_pressed, time, minor3rd, minor3rd);
                    break;
                case chord_emulation::augmented:
                    on_chord(tone, was_pressed, time, major3rd, major3rd);
                    break;
                case chord_emulation::major7:
                    on_chord(tone, was_pressed, time, {0, 4, 7, 11});
                    break;
                case chord_emulation::dominant7:
                    on_chord(tone, was_pressed, time, {0, 4, 7, 10});
                    break;
                case chord_emulation::augmented7:
                    on_chord(tone, was_pressed, time, {0, 4, 8, 10});
                    break;
                case chord_emulation::augmented_major7:
                    on_chord(tone, was_pressed, time, {0, 4, 8, 11});
                    break;
                case chord_emulation::minor_major7:
                    on_chord(tone, was_pressed, time, {0, 3, 7, 11});
                    break;
                default: break;
                }
            }
        }

        if(input == core::key::shift_left || input == core::key::shift_right)
        {
            octave_shift = was_pressed;
        }
    }


    void
    single_tone_node::on_tone(int tone, bool down, float time)
    {
        if(down)
        {
            if(!down_tones.empty())
            {
                const auto current_tone = get_current_tone();
                send_tone(current_tone, false, time);
            }
            // this will override the old tone and this is "wrong",
            // but we need a better way to handle multiples anyway
            down_tones[tone] = time;

            send_tone(tone, down, time);
        }
        else
        {
            const auto playing_tone = get_current_tone();
            down_tones.erase(tone);

            if(playing_tone == tone)
            {
                // this means the best match stopped playing

                send_tone(tone, false, time);
                if(!down_tones.empty())
                {
                    const auto new_tone = get_current_tone();
                    send_tone(new_tone, true, time);
                }
            }
        }
    }

    int
    single_tone_node::get_current_tone() const
    {
        bool has_tone = false;
        int tone = 0;
        float time = 0;
        for(const auto k: down_tones)
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
                time = k.second;
                tone = k.first;
            }
        }

        ASSERT(has_tone);
        return tone;
    }


    void
    arpegiator_node::update(float dt, float current_time)
    {
        int count = 0;
        current_time_in_interval += dt;
        while(current_time_in_interval > update_time && count < 10)
        {
            count += 1;
            current_time_in_interval -= update_time;
            if(!tones.empty())
            {
                const int size = tones.size();

                if(mode == arp_mode::random || mode == arp_mode::random_no_repeat)
                {
                    const int last_index = index;
                    index = rand() % size;
                    if(mode == arp_mode::random_no_repeat && tones.size() > 1)
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
                send_tone(tones[index], true, current_time);
                active_tones[tones[index]] = current_time + tone_time;
            }
        }

        std::vector<int> erase;
        for(auto& tone: active_tones)
        {
            if(tone.second < current_time)
            {
                send_tone(tone.first, false, tone.second);
                erase.emplace_back(tone.first);
            }
        }
        for(auto tone: erase)
        {
            active_tones.erase(tone);
        }
    }

    void
    arpegiator_node::on_tone(int tone, bool down, float time)
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
            for(const auto& t: down_tones)
            {
                for(int i = 0; i < octaves; i += 1)
                {
                    tt.insert(t.first + i * 12);
                }
            }
            switch(mode)
            {
            case arp_mode::up:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                break;
            case arp_mode::down:
                tones.insert(tones.begin(), tt.rbegin(), tt.rend());
                break;
            case arp_mode::up_down_inclusive:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                tones.insert(tones.begin(), tt.rbegin(), tt.rend());
                break;
            case arp_mode::up_down_exclusive:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                if(tt.size() > 2)
                {
                    tt.erase(tt.begin());
                    auto it = tt.end();
                    --it;
                    tt.erase(it);
                    tones.insert(tones.begin(), tt.rbegin(), tt.rend());
                }
                break;
            default:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                break;
            }
            index = index % core::c_sizet_to_int(tones.size());
        }
    }


    float
    run_oscilator(float frequency, float time, oscilator_type osc)
    {
        const float sine = core::sin( core::angle::from_percent_of_360(frequency) * time);
        switch(osc)
        {
        case oscilator_type::sine: return sine;
        case oscilator_type::square: return sine > 0.0f ? 1.0f : -1.0f;
        case oscilator_type::triangle: return core::asin(sine).in_radians() * (2.0f / pi);
        case oscilator_type::sawtooth:
            return (2 / pi) * (frequency * pi * fmodf(time, 1 / frequency) - pi / 2);
        case oscilator_type::noise:
            // todo(Gustav): use the improved c++ random library
            // todo(Gustav): and also add perlin noise?
            return 2 * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 1;
        default: return 0;
        }
    }


    float
    to01(float lower_bound, float value, float upper_bound)
    {
        return (value - lower_bound) / (upper_bound - lower_bound);
    }


    float
    envelope::get_live(float wave, float start_time, float current_time) const
    {
        if(current_time < start_time)
        {
            return 0;
        }
        else if(current_time > (start_time + time_to_start))
        {
            return wave;
        }
        else
        {
            return wave * to01(start_time, current_time, start_time + time_to_start);
        }
    }

    float
    envelope::get_dead(float wave, float end_time, float current_time) const
    {
        if(current_time < end_time)
        {
            return wave;
        }
        else if(current_time > (end_time + time_to_end))
        {
            return 0;
        }
        else
        {
            return wave * (1 - to01(end_time, current_time, end_time + time_to_end));
        }
    }


    int
    oscilator_node::get_total_tones() const
    {
        return core::c_sizet_to_int(live.size() + dead.size());
    }

    int
    oscilator_node::get_alive_tones() const
    {
        return core::c_sizet_to_int(live.size());
    }

    int
    oscilator_node::get_dead_tones() const
    {
        return core::c_sizet_to_int(dead.size());
    }

    void
    oscilator_node::update(float, float current_time)
    {
        dead.erase
        (
            std::remove_if
            (
                dead.begin(),
                dead.end(),
                [=](const dead_frequency& df) -> bool
                {
                    return df.time_end + envelope.time_to_end < current_time;
                }
            ),
            dead.end()
        );
    }

    void
    oscilator_node::on_frequency_down(int id, float freq, float time)
    {
        live[id] = live_frequency {freq, time};
    }

    void
    oscilator_node::on_frequency_up(int id, float frequency, float time)
    {
        const auto scale = envelope.get_live(1.0f, live[id].time_start, time);
        live.erase(id);
        dead.emplace_back(dead_frequency {true, frequency, time, scale});
    }

    float
    oscilator_node::get_output(float time)
    {
        float value = 0;

        const int maxtones = 10;

        int tone = 0;
        for(const auto& li: live)
        {
            if(tone++ > maxtones)
            {
                break;
            }
            const auto& f = li.second;
            value += envelope.get_live
            (
                run_oscilator(f.frequency, time, oscilator),
                f.time_start,
                time
            );
        }

        tone = 0;
        for(const auto& d: dead)
        {
            if(tone++ > maxtones)
            {
                break;
            }
            value += d.scale * envelope.get_dead
            (
                run_oscilator(d.frequency, time, oscilator),
                d.time_end,
                time
            );
        }

        return value;
    }


    namespace
    {
        template <typename T>
        void
        insert(std::vector<T>* to, const std::vector<T>& from)
        {
            for(const auto& t: from)
            {
                to->push_back(t);
            }
        }
    }


    float
    effect::get_output(float time)
    {
        if(in == nullptr)
        {
            return 0;
        }

        return on_wave(in->get_output(time));
    }


    float
    volume_node::on_wave(float wave)
    {
        return volume * wave;
    }


    float
    scaler_effect::on_wave(float wave)
    {
        float w = core::abs(wave);
        const auto negative = wave < 0;
        for(int i = 0; i < times; i += 1)
        {
            w = w * w;
        }
        return negative ? w * -1 : w;
    }


    std::vector<piano_key>
    build_one_cctave_of_piano_keys
    (
        int octave,
        int semitone_offset,
        core::key c,
        core::key d,
        core::key e,
        core::key f,
        core::key g,
        core::key a,
        core::key b,
        core::key c_sharp,
        core::key d_sharp,
        core::key f_sharp,
        core::key g_sharp,
        core::key a_sharp
    )
    {
        return
        {
            piano_key(semitone_offset + 0, c, "C", octave),
            piano_key(semitone_offset + 1, c_sharp, "C#", octave),
            piano_key(semitone_offset + 2, d, "D", octave),
            piano_key(semitone_offset + 3, d_sharp, "D#", octave),
            piano_key(semitone_offset + 4, e, "E", octave),

            piano_key(semitone_offset + 5, f, "F", octave),
            piano_key(semitone_offset + 6, f_sharp, "F#", octave),
            piano_key(semitone_offset + 7, g, "G", octave),
            piano_key(semitone_offset + 8, g_sharp, "G#", octave),
            piano_key(semitone_offset + 9, a, "A", octave),
            piano_key(semitone_offset + 10, a_sharp, "A#", octave),
            piano_key(semitone_offset + 11, b, "B", octave),
        };
    }


    const keyboard_layout&
    create_qwerty_keyboard_layout()
    {
        using K = euphoria::core::key;

        static const keyboard_layout k =
        {
            {K::num_1, K::num_2, K::num_3, K::num_4, K::num_5, K::num_6, K::num_7, K::num_8, K::num_9, K::num_0},
            {K::q, K::w, K::e, K::r, K::t, K::y, K::u, K::i, K::o, K::p},
            {K::a, K::s, K::d, K::f, K::g, K::h, K::j, K::k, K::l},
            {K::z, K::x, K::c, K::v, K::b, K::n, K::m},
        };

        return k;
    }


    void
    setup_one_octave_layout
    (
        std::vector<piano_key>* keys,
        int base_octave,
        int octave,
        const keyboard_layout& k,
        int start_row,
        int start_col
    )
    {
        const auto key = [&](int x, int y) -> core::key
        {
            const auto wy = start_row - y + 1;
            if(wy < 0 || wy > core::c_sizet_to_int(k.size()))
            {
                return core::key::unbound;
            }
            const auto& r = k[wy];
            const auto wx = start_col + x;
            if(wx < 0 || wx > core::c_sizet_to_int(r.size()))
            {
                return core::key::unbound;
            }
            return r[wx];
        };
        const auto white_key = key;
        const auto black_key = key;

        insert
        (
            keys,
            build_one_cctave_of_piano_keys
            (
                base_octave + octave,
                octave * 12,
                // first 3 white
                white_key(0, 0), white_key(1, 0), white_key(2, 0),
                // second 4
                white_key(3, 0), white_key(4, 0), white_key(5, 0), white_key(6, 0),
                // first 2 black
                black_key(1, 1), black_key(2, 1),
                // second 3
                black_key(4, 1), black_key(5, 1), black_key(6, 1)
            )
        );
    }


    void
    setup_qwerty_two_octave_layout
    (
        std::vector<piano_key>* keys,
        int base_octave,
        int octave_offset
    )
    {
        setup_one_octave_layout
        (
                keys,
                base_octave,
            0 + octave_offset,
                create_qwerty_keyboard_layout(),
                0,
                3
        );
        setup_one_octave_layout
        (
                keys,
                base_octave,
            1 + octave_offset,
                create_qwerty_keyboard_layout(),
                2,
                0
        );
    }
}
