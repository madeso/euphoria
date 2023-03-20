#include "minsynth/synth.h"

#include "catch.hpp"

namespace synth = euphoria::minsynth;

TEST_CASE("synth-verify", "[synth]")
{
    CHECK(synth::from_tone_to_frequency(0, synth::base_frequencies::c0) == Approx(synth::base_frequencies::c0));
    CHECK(synth::from_tone_to_frequency(0, synth::base_frequencies::a4) == Approx(synth::base_frequencies::a4));
    CHECK(synth::from_tone_to_frequency(-9, synth::base_frequencies::a4) == Approx(261.63f).margin(0.1));
    CHECK(synth::from_tone_to_frequency(-12, synth::base_frequencies::a4) == Approx(220.0f));
    CHECK(synth::from_tone_to_frequency(-12 - 9, synth::base_frequencies::a4) == Approx(130.81f).margin(0.1));
}
