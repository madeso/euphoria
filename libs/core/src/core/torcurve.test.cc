#include "tests/approx_equal.h"

#include "core/torcurve.h"

#include "catch2/catch_all.hpp"


namespace core = eu::core;
using namespace eu::tests;
using namespace eu::convert;

TEST_CASE("torcurve-test", "[torcurve]")
{
    using P = std::pair<float, float>;
    // Sampled data from the javascript implementation
    const auto sample = GENERATE
    (
        P{0.0f, 0.0f},
        P{0.1f, 0.09999800003999922f},
        P{0.2f, 0.19999600007999843f},
        P{0.3f, 0.29999400011999766f},
        P{0.4f, 0.39999200015999686f},
        P{0.5f, 0.5000099998000039f},
        P{0.6f, 0.6000079998400032f},
        P{0.7f, 0.7000059998800024f},
        P{0.8f, 0.8000039999200016f},
        P{0.9f, 0.9000019999600009f},
        P{1.0f, 1.0f}
    );
    CHECK(sample.second == core::calc_tor_curve(sample.first, 0.0f, 0.5f, 0.0f));
}
