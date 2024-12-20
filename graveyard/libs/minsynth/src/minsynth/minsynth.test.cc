#include "minsynth/synth.h"

#include "catch2/catch_all.hpp"

namespace synth = eu::minsynth;

TEST_CASE("synth-verify", "[synth]")
{
    CHECK(synth::from_tone_to_frequency(0, synth::base_frequencies::c0) == Catch::Approx(synth::base_frequencies::c0));
    CHECK(synth::from_tone_to_frequency(0, synth::base_frequencies::a4) == Catch::Approx(synth::base_frequencies::a4));
    CHECK(synth::from_tone_to_frequency(-9, synth::base_frequencies::a4) == Catch::Approx(261.63f).margin(0.1));
    CHECK(synth::from_tone_to_frequency(-12, synth::base_frequencies::a4) == Catch::Approx(220.0f));
    CHECK(synth::from_tone_to_frequency(-12 - 9, synth::base_frequencies::a4) == Catch::Approx(130.81f).margin(0.1));
}
