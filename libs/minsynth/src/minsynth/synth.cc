#include "minsynth/synth.h"

#include <cmath>
#include <set>
#include <algorithm>

#include <fmt/core.h>
#include <fmt/format.h>

#include "log/log.h"
#include "assert/assert.h"
#include "core/numeric.h"
#include "core/cint.h"
#include "core/angle.h"


using namespace fmt::literals;


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
    to_string(Tuning t)
    {
        switch(t)
        {
        case Tuning::a4: return "A4 (ISO 16) 440 Hz";
        case Tuning::boston: return "Boston 441 Hz";
        case Tuning::new_york: return "New York 442 Hz";
        case Tuning::europe: return "europe 443 Hz";
        case Tuning::french: return "French 435 Hz";
        case Tuning::baroque: return "baroque 415 hz";
        case Tuning::chorton: return "chorton 466 Hz";
        case Tuning::classical: return "classical 430 Hz";
        default: return "???";
        }
    }

    std::string
    to_string(ChordEmulation em)
    {
        switch(em)
        {
        case ChordEmulation::none: return "None";
        case ChordEmulation::major: return "Major triad";
        case ChordEmulation::minor: return "Minor Triad";
        case ChordEmulation::diminished: return "diminished triad";
        case ChordEmulation::augmented: return "augmented triad";
        case ChordEmulation::major7: return "Major 7th";
        case ChordEmulation::dominant7: return "Dominant 7th";
        case ChordEmulation::augmented7: return "augmented 7th";
        case ChordEmulation::augmented_major7: return "augmented Major 7th";
        case ChordEmulation::minor_major7: return "Minor major 7th";
        default: return "???";
        }
    }

    float
    from_tuning_to_base_frequency(Tuning t)
    {
        switch(t)
        {
        case Tuning::a4: return base_frequencies::a4;
        case Tuning::boston: return base_frequencies::boston_a4;
        case Tuning::new_york: return base_frequencies::new_york_a4;
        case Tuning::europe: return base_frequencies::europe_a4;
        case Tuning::french: return base_frequencies::french_a4;
        case Tuning::baroque: return base_frequencies::baroque_a4;
        case Tuning::chorton: return base_frequencies::chorton_a4;
        case Tuning::classical: return base_frequencies::classical_a4;
        default: return base_frequencies::a4;
        }
    }


    std::string
    from_midi_event_to_string(MidiEvent e)
    {
        switch(e)
        {
        case MidiEvent::note_off: return "Off";
        case MidiEvent::note_on: return "On";
        case MidiEvent::aftertouch: return "aftertouch";
        case MidiEvent::control_change: return "CC";
        case MidiEvent::program_change: return "PC";
        case MidiEvent::channel_pressure: return "Channel Pressure";
        case MidiEvent::pitch_bend: return "pitch_bend";
        }
        return "???";
    }


    std::string
    to_string(ArpMode mode)
    {
        switch(mode)
        {
        case ArpMode::up: return "Up";
        case ArpMode::down: return "Down";
        case ArpMode::up_down_inclusive: return "Up/Down (inclusive)";
        case ArpMode::up_down_exclusive: return "Up/Down (exclusive)";
        case ArpMode::random: return "Random";
        case ArpMode::random_no_repeat: return "Random (no repeat)";
        default: return "???";
        }
    }


    std::string
    to_string(OscilatorType osc)
    {
        switch(osc)
        {
        case OscilatorType::sine: return "Sine";
        case OscilatorType::square: return "Square";
        case OscilatorType::triangle: return "Triangle";
        case OscilatorType::sawtooth: return "sawtooth";
        case OscilatorType::noise: return "noise";
        default: return "?";
        }
    }

    template <int steps_per_octave>
    struct ToneToFrequencyConverter
    {
        constexpr ToneToFrequencyConverter()
        {
            const float base
                    = std::pow(2.0f, 1.0f / static_cast<float>(steps_per_octave));
            for(int i = 0; i < steps_per_octave; i += 1)
            {
                step_data[i] = std::pow(base, static_cast<float>(i));
            }
        }

        [[nodiscard]] constexpr float
        get_frequency(int halfstep, float octave_base_frequency) const
        {
            float freq = octave_base_frequency;

            int step = halfstep;

            while(step >= steps_per_octave)
            {
                freq = freq * 2;
                step -= steps_per_octave;
            }

            while(step < 0)
            {
                freq = freq / 2;
                step += steps_per_octave;
            }

            ASSERT(step >= 0 && step < steps_per_octave);
            const auto rf = freq * step_data[step];
            return rf;
        }

    private:
        float step_data[steps_per_octave] = {
                0.0f,
        };
    };


    template <int tones_per_octave>
    float
    from_tone_to_frequency_impl(int tone, float base_frequency)
    {
        const static auto converter = ToneToFrequencyConverter<tones_per_octave>();
        return converter.get_frequency(tone, base_frequency);
    }

    float
    from_tone_to_frequency(int tone, float base_frequency)
    {
        return from_tone_to_frequency_impl<12>(tone, base_frequency);
    }

    void
    Node::update(float /*dt*/, float /*current_time*/)
    {
    }

    void
    ToneSender::send_tone(int tone, bool down, float time) const
    {
        if(next_node != nullptr)
        {
            next_node->on_tone(tone, down, time);
        }
    }


    void
    ToneToFrequencyConverterNode::on_tone(int tone, bool down, float time)
    {
        if(next == nullptr)
        {
            return;
        }

        if(down)
        {
            next->on_frequency_down(tone, calc_frequency(tone), time);
        }
        else
        {
            next->on_frequency_up(tone, calc_frequency(tone), time);
        }
    }

    float
    ToneToFrequencyConverterNode::calc_frequency(int semitone) const
    {
        const float base_freq = from_tuning_to_base_frequency(tuning);
        const int tone = semitone - 9; // hrm.... why?

        const float freq = from_tone_to_frequency(tone, base_freq);

        return freq;
    }

    namespace
    {
        std::string
        name_and_octave(const std::string& name, int octave)
        {
            return "{}{}"_format(name, octave);
        }
    }

    PianoKey::PianoKey(int st, core::Key kc, const std::string& n, int octave)
        : semitone(st)
        , keycode(kc)
        , name(name_and_octave(n, octave))
        , octave_shift(false)
    {
    }


    bool
    MidiInNode::is_status_message(unsigned char b)
    {
        const auto is_status = ((b >> 7) & 0x1) == 1;
        return is_status;
    }

    void
    MidiInNode::debug_callback
    (
        double dt,
        const std::vector<unsigned char>& bytes
    )
    {
        using Byte = unsigned char;

        const Byte message_mask = 0x7;
        const Byte channel_mask = 0xF;

        for(auto b: bytes)
        {
            const auto is_status = MidiInNode::is_status_message(b);
            if(is_status)
            {
                const Byte channel = (b >> 0) & channel_mask;
                const Byte message = (b >> 4) & message_mask;

                LOG_INFO
                (
                "STAT ({0}) {1}",
                static_cast<unsigned int>(channel),
                from_midi_event_to_string(static_cast<MidiEvent>(message))
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
    MidiInNode::callback(float dt, const std::vector<unsigned char>& bytes)
    {
        using Byte = unsigned char;

        if(bytes.empty())
        {
            return;
        }

        if(!MidiInNode::is_status_message(bytes[0]))
        {
            LOG_ERROR("todo: need to handle data message without status.");
            return;
        }

        const auto bytes_size = bytes.size();

        const Byte message_mask = 0x7;
        // const byte channel_mask = 0xF;
        // const byte channel = (bytes[0] >> 0) & channel_mask;
        const Byte message = (bytes[0] >> 4) & message_mask;

        const auto event = static_cast<MidiEvent>(message);
        switch(event)
        {
        case MidiEvent::note_on:
        case MidiEvent::note_off: {
            const bool on = event == MidiEvent::note_on;
            if(bytes_size == 3)
            {
                const auto note = bytes[1];
                const auto velocity = bytes[2];

                if(MidiInNode::is_status_message(note) || MidiInNode::is_status_message(velocity))
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

        default: LOG_ERROR("todo: handle {0}", from_midi_event_to_string(event)); return;
        }
    }


    void
    KeyboardInputNode::on_chord
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
    KeyboardInputNode::on_chord
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
    KeyboardInputNode::on_input(core::Key input, bool was_pressed, float time)
    {
        if(tones == nullptr)
        {
            return;
        }

        for(auto& key: keys)
        {
            if(key.keycode != core::Key::unbound && input == key.keycode)
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
                case ChordEmulation::none:
                    tones->on_tone(tone, was_pressed, time);
                    break;
                case ChordEmulation::major:
                    on_chord(tone, was_pressed, time, major3rd, minor3rd);
                    break;
                case ChordEmulation::minor:
                    on_chord(tone, was_pressed, time, minor3rd, major3rd);
                    break;
                case ChordEmulation::diminished:
                    on_chord(tone, was_pressed, time, minor3rd, minor3rd);
                    break;
                case ChordEmulation::augmented:
                    on_chord(tone, was_pressed, time, major3rd, major3rd);
                    break;
                case ChordEmulation::major7:
                    on_chord(tone, was_pressed, time, {0, 4, 7, 11});
                    break;
                case ChordEmulation::dominant7:
                    on_chord(tone, was_pressed, time, {0, 4, 7, 10});
                    break;
                case ChordEmulation::augmented7:
                    on_chord(tone, was_pressed, time, {0, 4, 8, 10});
                    break;
                case ChordEmulation::augmented_major7:
                    on_chord(tone, was_pressed, time, {0, 4, 8, 11});
                    break;
                case ChordEmulation::minor_major7:
                    on_chord(tone, was_pressed, time, {0, 3, 7, 11});
                    break;
                default: break;
                }
            }
        }

        if(input == core::Key::shift_left || input == core::Key::shift_right)
        {
            octave_shift = was_pressed;
        }
    }


    void
    SingleToneNode::on_tone(int tone, bool down, float time)
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
    SingleToneNode::get_current_tone() const
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
    ArpegiatorNode::update(float dt, float current_time)
    {
        int count = 0;
        current_time_in_interval += dt;
        while(current_time_in_interval > update_time && count < 10)
        {
            count += 1;
            current_time_in_interval -= update_time;
            if(!tones.empty())
            {
                const int size = core::c_sizet_to_int(tones.size());

                if(mode == ArpMode::random || mode == ArpMode::random_no_repeat)
                {
                    const int last_index = index;
                    index = rand() % size;
                    if(mode == ArpMode::random_no_repeat && tones.size() > 1)
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
    ArpegiatorNode::on_tone(int tone, bool down, float time)
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
            case ArpMode::up:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                break;
            case ArpMode::down:
                tones.insert(tones.begin(), tt.rbegin(), tt.rend());
                break;
            case ArpMode::up_down_inclusive:
                tones.insert(tones.begin(), tt.begin(), tt.end());
                tones.insert(tones.begin(), tt.rbegin(), tt.rend());
                break;
            case ArpMode::up_down_exclusive:
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
    run_oscilator(float frequency, float time, OscilatorType osc)
    {
        const float sine = core::sin( core::angle::from_percent_of_360(frequency) * time);
        switch(osc)
        {
        case OscilatorType::sine: return sine;
        case OscilatorType::square: return sine > 0.0f ? 1.0f : -1.0f;
        case OscilatorType::triangle: return core::asin(sine).as_radians() * (2.0f / pi);
        case OscilatorType::sawtooth:
            return (2 / pi) * (frequency * pi * fmodf(time, 1 / frequency) - pi / 2);
        case OscilatorType::noise:
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
    Envelope::get_live(float wave, float start_time, float current_time) const
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
    Envelope::get_dead(float wave, float end_time, float current_time) const
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
    OscilatorNode::get_total_tones() const
    {
        return core::c_sizet_to_int(live.size() + dead.size());
    }

    int
    OscilatorNode::get_alive_tones() const
    {
        return core::c_sizet_to_int(live.size());
    }

    int
    OscilatorNode::get_dead_tones() const
    {
        return core::c_sizet_to_int(dead.size());
    }

    void
    OscilatorNode::update(float, float current_time)
    {
        dead.erase
        (
            std::remove_if
            (
                dead.begin(),
                dead.end(),
                [=](const DeadFrequency& df) -> bool
                {
                    return df.time_end + envelope.time_to_end < current_time;
                }
            ),
            dead.end()
        );
    }

    void
    OscilatorNode::on_frequency_down(int id, float freq, float time)
    {
        live[id] = LiveFrequency {freq, time};
    }

    void
    OscilatorNode::on_frequency_up(int id, float frequency, float time)
    {
        const auto scale = envelope.get_live(1.0f, live[id].time_start, time);
        live.erase(id);
        dead.emplace_back(DeadFrequency {true, frequency, time, scale});
    }

    float
    OscilatorNode::get_output(float time)
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
    Effect::get_output(float time)
    {
        if(in == nullptr)
        {
            return 0;
        }

        return on_wave(in->get_output(time));
    }


    float
    VolumeNode::on_wave(float wave)
    {
        return volume * wave;
    }


    float
    ScalerEffect::on_wave(float wave)
    {
        float w = core::abs(wave);
        const auto negative = wave < 0;
        for(int i = 0; i < times; i += 1)
        {
            w = w * w;
        }
        return negative ? w * -1 : w;
    }


    std::vector<PianoKey>
    build_one_cctave_of_piano_keys
    (
        int octave,
        int semitone_offset,
        core::Key c,
        core::Key d,
        core::Key e,
        core::Key f,
        core::Key g,
        core::Key a,
        core::Key b,
        core::Key c_sharp,
        core::Key d_sharp,
        core::Key f_sharp,
        core::Key g_sharp,
        core::Key a_sharp
    )
    {
        return
        {
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


    const KeyboardLayout&
    create_qwerty_keyboard_layout()
    {
        using K = euphoria::core::Key;

        static const KeyboardLayout k =
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
        std::vector<PianoKey>* keys,
        int base_octave,
        int octave,
        const KeyboardLayout& k,
        int start_row,
        int start_col
    )
    {
        const auto key = [&](int x, int y) -> core::Key
        {
            const auto wy = start_row - y + 1;
            if(wy < 0 || wy > core::c_sizet_to_int(k.size()))
            {
                return core::Key::unbound;
            }
            const auto& r = k[wy];
            const auto wx = start_col + x;
            if(wx < 0 || wx > core::c_sizet_to_int(r.size()))
            {
                return core::Key::unbound;
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
        std::vector<PianoKey>* keys,
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
