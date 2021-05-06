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
    constexpr float pi = core::Pi();


    LOG_SPECIFY_DEFAULT_LOGGER("minsynth.synth")

    std::string
    to_string(Tuning t)
    {
        switch(t)
        {
        case Tuning::A4: return "A4 (ISO 16) 440 Hz";
        case Tuning::Boston: return "Boston 441 Hz";
        case Tuning::NewYork: return "New York 442 Hz";
        case Tuning::Europe: return "Europe 443 Hz";
        case Tuning::French: return "French 435 Hz";
        case Tuning::Baroque: return "Baroque 415 hz";
        case Tuning::Chorton: return "Chorton 466 Hz";
        case Tuning::Classical: return "Classical 430 Hz";
        default: return "???";
        }
    }

    std::string
    to_string(ChordEmulation em)
    {
        switch(em)
        {
        case ChordEmulation::None: return "None";
        case ChordEmulation::Major: return "Major triad";
        case ChordEmulation::Minor: return "Minor Triad";
        case ChordEmulation::Diminished: return "Diminished triad";
        case ChordEmulation::Augmented: return "Augmented triad";
        case ChordEmulation::Major7: return "Major 7th";
        case ChordEmulation::Dominant7: return "Dominant 7th";
        case ChordEmulation::Augmented7: return "Augmented 7th";
        case ChordEmulation::AugmentedMajor7: return "Augmented Major 7th";
        case ChordEmulation::MinorMajor7: return "Minor major 7th";
        default: return "???";
        }
    }

    float
    TuningToBaseFrequency(Tuning t)
    {
        switch(t)
        {
        case Tuning::A4: return base_frequencies::a4;
        case Tuning::Boston: return base_frequencies::boston_a4;
        case Tuning::NewYork: return base_frequencies::new_york_a4;
        case Tuning::Europe: return base_frequencies::europe_a4;
        case Tuning::French: return base_frequencies::french_a4;
        case Tuning::Baroque: return base_frequencies::baroque_a4;
        case Tuning::Chorton: return base_frequencies::chorton_a4;
        case Tuning::Classical: return base_frequencies::classical_a4;
        default: return base_frequencies::a4;
        }
    }


    std::string
    MidiEventToString(MidiEvent e)
    {
        switch(e)
        {
        case MidiEvent::NoteOff: return "Off";
        case MidiEvent::NoteOn: return "On";
        case MidiEvent::Aftertouch: return "Aftertouch";
        case MidiEvent::ControlChange: return "CC";
        case MidiEvent::ProgramChange: return "PC";
        case MidiEvent::ChannelPressure: return "Channel Pressure";
        case MidiEvent::PitchBend: return "PitchBend";
        }
        return "???";
    }


    std::string
    to_string(ArpMode mode)
    {
        switch(mode)
        {
        case ArpMode::Up: return "Up";
        case ArpMode::Down: return "Down";
        case ArpMode::UpDownInclusive: return "Up/Down (inclusive)";
        case ArpMode::UpDownExclusive: return "Up/Down (exclusive)";
        case ArpMode::Random: return "Random";
        case ArpMode::RandomNoRepeat: return "Random (no repeat)";
        default: return "???";
        }
    }


    std::string
    to_string(OscilatorType osc)
    {
        switch(osc)
        {
        case OscilatorType::Sine: return "Sine";
        case OscilatorType::Square: return "Square";
        case OscilatorType::Triangle: return "Triangle";
        case OscilatorType::Sawtooth: return "Sawtooth";
        case OscilatorType::Noise: return "Noise";
        default: return "?";
        }
    }

    template <int StepsPerOctave>
    struct ToneToFrequencyConverter
    {
        constexpr ToneToFrequencyConverter()
        {
            const float base
                    = std::pow(2.0f, 1.0f / static_cast<float>(StepsPerOctave));
            for(int i = 0; i < StepsPerOctave; i += 1)
            {
                step_data[i] = std::pow(base, static_cast<float>(i));
            }
        }

        [[nodiscard]] constexpr float
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
    TemplateToneToFrequency(int tone, float base_frequency)
    {
        const static auto converter
                = ToneToFrequencyConverter<TonesPerOctave>();
        return converter.GetFrequency(tone, base_frequency);
    }

    float
    ToneToFrequency(int tone, float base_frequency)
    {
        return TemplateToneToFrequency<12>(tone, base_frequency);
    }

    // nodes


    void
    Node::Update(float /*dt*/, float /*current_time*/)
    {
    }

    void
    ToneSender::SendTone(int tone, bool down, float time) const
    {
        if(NextNode != nullptr)
        {
            NextNode->OnTone(tone, down, time);
        }
    }


    void
    ToneToFrequencyConverterNode::OnTone(int tone, bool down, float time)
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
    ToneToFrequencyConverterNode::CalculateFrequency(int semitone) const
    {
        const float base_freq = TuningToBaseFrequency(tuning);
        const int tone = semitone - 9; // hrm.... why?

        const float freq = ToneToFrequency(tone, base_freq);

        return freq;
    }

    namespace
    {
        std::string
        NameAndOctave(const std::string& name, int octave)
        {
            std::stringstream ss;
            ss << name << octave;
            return ss.str();
        }
    } // namespace

    PianoKey::PianoKey(int st, core::key kc, const std::string& n, int octave)
        : semitone(st)
        , keycode(kc)
        , name(NameAndOctave(n, octave))
        , octave_shift(false)
    {}


    bool
    MidiInNode::IsStatusMessage(unsigned char b)
    {
        const auto is_status = ((b >> 7) & 0x1) == 1;
        return is_status;
    }

    void
    MidiInNode::DebugCallback
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
            const auto is_status = MidiInNode::IsStatusMessage(b);
            if(is_status)
            {
                const byte channel = (b >> 0) & channel_mask;
                const byte message = (b >> 4) & message_mask;

                LOG_INFO
                (
                    "STAT ({0}) {1}",
                    static_cast<unsigned int>(channel),
                    MidiEventToString(static_cast<MidiEvent>(message))
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
    MidiInNode::Callback(float dt, const std::vector<unsigned char>& bytes)
    {
        using byte = unsigned char;

        if(bytes.empty())
        {
            return;
        }

        if(!MidiInNode::IsStatusMessage(bytes[0]))
        {
            LOG_ERROR("todo: need to handle data message without status.");
            return;
        }

        const auto bytes_size = bytes.size();

        const byte message_mask = 0x7;
        // const byte channel_mask = 0xF;
        // const byte channel = (bytes[0] >> 0) & channel_mask;
        const byte message = (bytes[0] >> 4) & message_mask;

        const auto event = static_cast<MidiEvent>(message);
        switch(event)
        {
        case MidiEvent::NoteOn:
        case MidiEvent::NoteOff: {
            const bool on = event == MidiEvent::NoteOn;
            if(bytes_size == 3)
            {
                const auto note = bytes[1];
                const auto velocity = bytes[2];

                if(MidiInNode::IsStatusMessage(note) || MidiInNode::IsStatusMessage(velocity))
                {
                    LOG_ERROR("Unexpected midi command in note on/off data.");
                    return;
                }

                // c3 == midi #60
                if(tones != nullptr)
                {
                    const int tone = note;
                    float time = dt;
                    tones->OnTone(tone - 60, on, time + last_time);
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

        default: LOG_ERROR("todo: handle {0}", MidiEventToString(event)); return;
        }
    }


    void
    KeyboardInputNode::OnChord
    (
        int base,
        bool was_pressed,
        float time,
        int first,
        int second
    ) const
    {
        tones->OnTone(base, was_pressed, time);
        tones->OnTone(base + first, was_pressed, time);
        tones->OnTone(base + first + second, was_pressed, time);
    }

    void
    KeyboardInputNode::OnChord
    (
        int base,
        bool was_pressed,
        float time,
        const std::vector<int>& integer_notation
    ) const
    {
        for(auto i: integer_notation)
        {
            tones->OnTone(base + i, was_pressed, time);
        }
    }

    void
    KeyboardInputNode::OnInput(core::key input, bool was_pressed, float time)
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
    SingleToneNode::OnTone(int tone, bool down, float time)
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
    SingleToneNode::GetCurrentTone() const
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
    ArpegiatorNode::Update(float dt, float current_time)
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

                if(mode == ArpMode::Random || mode == ArpMode::RandomNoRepeat)
                {
                    const int last_index = index;
                    index = rand() % size;
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
        for(auto& tone: active_tones)
        {
            if(tone.second < current_time)
            {
                SendTone(tone.first, false, tone.second);
                erase.emplace_back(tone.first);
            }
        }
        for(auto tone: erase)
        {
            active_tones.erase(tone);
        }
    }

    void
    ArpegiatorNode::OnTone(int tone, bool down, float time)
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
            index = index % core::Csizet_to_int(tones.size());
        }
    }


    float
    RunOscilator(float frequency, float time, OscilatorType osc)
    {
        const float sine = core::sin( core::angle::from_percent_of_360(frequency) * time);
        switch(osc)
        {
        case OscilatorType::Sine: return sine;
        case OscilatorType::Square: return sine > 0.0f ? 1.0f : -1.0f;
        case OscilatorType::Triangle: return core::asin(sine).in_radians() * (2.0f / pi);
        case OscilatorType::Sawtooth:
            return (2 / pi) * (frequency * pi * fmodf(time, 1 / frequency) - pi / 2);
        case OscilatorType::Noise:
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
    Envelope::GetLive(float wave, float start_time, float current_time) const
    {
        if(current_time < start_time) { return 0; }
        else if(current_time > (start_time + time_to_start)) { return wave; }
        else
        {
            return wave * to01(start_time, current_time, start_time + time_to_start);
        }
    }

    float
    Envelope::GetDead(float wave, float end_time, float current_time) const
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
    OscilatorNode::GetTotalTones() const
    {
        return core::Csizet_to_int(live.size() + dead.size());
    }

    int
    OscilatorNode::GetAliveTones() const
    {
        return live.size();
    }

    int
    OscilatorNode::GetDeadTones() const
    {
        return dead.size();
    }

    void
    OscilatorNode::Update(float, float current_time)
    {
        dead.erase(
                std::remove_if(
                        dead.begin(),
                        dead.end(),
                        [=](const DeadFrequency& df) -> bool {
                            return df.time_end + envelope.time_to_end
                                   < current_time;
                        }),
                dead.end());
    }

    void
    OscilatorNode::OnFrequencyDown(int id, float freq, float time)
    {
        live[id] = LiveFrequency {freq, time};
    }

    void
    OscilatorNode::OnFrequencyUp(int id, float frequency, float time)
    {
        const auto scale = envelope.GetLive(1.0f, live[id].time_start, time);
        live.erase(id);
        dead.emplace_back(DeadFrequency {true, frequency, time, scale});
    }

    float
    OscilatorNode::GetOutput(float time)
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
            value += envelope.GetLive
            (
                RunOscilator(f.frequency, time, oscilator),
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
            value += d.scale * envelope.GetDead
            (
                RunOscilator(d.frequency, time, oscilator),
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
        Insert(std::vector<T>* to, const std::vector<T>& from)
        {
            for(const auto& t: from)
            {
                to->push_back(t);
            }
        }
    } // namespace


    float
    Effect::GetOutput(float time)
    {
        if(in == nullptr)
        {
            return 0;
        }

        return OnWave(in->GetOutput(time));
    }


    float
    VolumeNode::OnWave(float wave)
    {
        return volume * wave;
    }


    float
    ScalerEffect::OnWave(float wave)
    {
        float w = core::Abs(wave);
        const auto negative = wave < 0;
        for(int i = 0; i < times; i += 1)
        {
            w = w * w;
        }
        return negative ? w * -1 : w;
    }


    std::vector<PianoKey>
    OneOctaveOfPianoKeys
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
    KeyboardLayoutQwerty()
    {
        using K = euphoria::core::key;

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
    SetupOneOctaveLayout
    (
        std::vector<PianoKey>* keys,
        int base_octave,
        int octave,
        const KeyboardLayout& k,
        int start_row,
        int start_col
    )
    {
        const auto K = [&](int x, int y) -> core::key
        {
            const auto wy = start_row - y + 1;
            if(wy < 0 || wy > core::Csizet_to_int(k.size()))
            {
                return core::key::unbound;
            }
            const auto& r = k[wy];
            const auto wx = start_col + x;
            if(wx < 0 || wx > core::Csizet_to_int(r.size()))
            {
                return core::key::unbound;
            }
            return r[wx];
        };
        const auto W = K;
        const auto B = K;

        Insert
        (
            keys,
            OneOctaveOfPianoKeys
            (
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
                B(6, 1)
            )
        );
    }


    void
    SetupQwertyTwoOctaveLayout
    (
        std::vector<PianoKey>* keys,
        int base_octave,
        int octave_offset
    )
    {
        SetupOneOctaveLayout
        (
            keys,
            base_octave,
            0 + octave_offset,
            KeyboardLayoutQwerty(),
            0,
            3
        );
        SetupOneOctaveLayout
        (
            keys,
            base_octave,
            1 + octave_offset,
            KeyboardLayoutQwerty(),
            2,
            0
        );
    }


}
