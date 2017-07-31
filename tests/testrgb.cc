#include "gtest/gtest.h"
#include "core/rgb.h"

#define GTEST(X) TEST(rgb, X)

const unsigned int DARKSLATE_GREY = 0x2f4f4f;

const unsigned int PHTHALO_GREEN = 0x123524;

const unsigned int BRITISH_RACING_GREEN = 0x004225;

const unsigned int RED = 0xff0000;
const unsigned int GREEN = 0x00ff00;
const unsigned int BLUE = 0x0000ff;


GTEST(test_basic) {
  EXPECT_EQ(255, colorutil::GetRed(RED));
  EXPECT_EQ(0, colorutil::GetGreen(RED));
  EXPECT_EQ(0, colorutil::GetBlue(RED));

  EXPECT_EQ(0, colorutil::GetRed(GREEN));
  EXPECT_EQ(255, colorutil::GetGreen(GREEN));
  EXPECT_EQ(0, colorutil::GetBlue(GREEN));

  EXPECT_EQ(0, colorutil::GetRed(BLUE));
  EXPECT_EQ(0, colorutil::GetGreen(BLUE));
  EXPECT_EQ(255, colorutil::GetBlue(BLUE));
}

GTEST(get_component_works) {
  EXPECT_EQ(47, colorutil::GetRed(DARKSLATE_GREY));
  EXPECT_EQ(79, colorutil::GetGreen(DARKSLATE_GREY));
  EXPECT_EQ(79, colorutil::GetBlue(DARKSLATE_GREY));

  EXPECT_EQ(18, colorutil::GetRed(PHTHALO_GREEN));
  EXPECT_EQ(53, colorutil::GetGreen(PHTHALO_GREEN));
  EXPECT_EQ(36, colorutil::GetBlue(PHTHALO_GREEN));

  EXPECT_EQ(0, colorutil::GetRed(BRITISH_RACING_GREEN));
  EXPECT_EQ(66, colorutil::GetGreen(BRITISH_RACING_GREEN));
  EXPECT_EQ(37, colorutil::GetBlue(BRITISH_RACING_GREEN));
}
