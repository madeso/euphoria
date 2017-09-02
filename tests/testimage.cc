#include "core/image.h"
#include "core/filesystem.h"
#include "core/base64.h"
#include "core/rgb.h"
#include "tests/testbase.h"
#include "core/draw.h"

#include "catch.hpp"

// 4x4 image without transperency
// white  / red
// green / blue
const char* const TEST_IMAGE =
    "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAABmJLR0QA/wD/"
    "AP+"
    "gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4QgdFSAdb6CP5AAAABVJREFUCNcFw"
    "QEBAAAAgBD/TxeqIDQz2gX7fv3PJgAAAABJRU5ErkJggg==";

TEST_CASE("image-load", "[img]")
{
  FileSystem fs;
  auto       catalog = FileSystemRootCatalog::AddRoot(&fs);
  catalog->RegisterFileData("white", base64::Decode(TEST_IMAGE));

  auto loaded = LoadImage(&fs, "white", AlphaLoad::Remove);
  REQUIRE(loaded.error == "");
  REQUIRE_FALSE(loaded.image.HasAlpha());

  REQUIRE(loaded.image.GetHeight() == 2);
  REQUIRE(loaded.image.GetWidth() == 2);

  // upper left
  SECTION("load-white")
  {
    const auto pixel = loaded.image.GetPixel(0, 1);
    const auto white = Rgba{1.0f, 1.0f, 1.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }

  // upper right
  SECTION("load-red")
  {
    const auto pixel = loaded.image.GetPixel(1, 1);
    const auto red   = Rgba{1.0f, 0.0f, 0.0f, 1.0f};
    REQUIRE(pixel == approx(red));
  }

  // lower left
  SECTION("load-green")
  {
    const auto pixel = loaded.image.GetPixel(0, 0);
    const auto green = Rgba{0.0f, 1.0f, 0.0f, 1.0f};
    REQUIRE(pixel == approx(green));
  }

  // lower right
  SECTION("load-blue")
  {
    const auto pixel = loaded.image.GetPixel(1, 0);
    const auto blue  = Rgba{0.0f, 0.0f, 1.0f, 1.0f};
    REQUIRE(pixel == approx(blue));
  }
}

TEST_CASE("image solid", "[img]")
{
  Image img;
  img.Setup(3, 3, false);

  SECTION("default-is-black")
  {
    REQUIRE(img.GetPixel(0, 0) == approx(Rgba(0, 0, 0, 1)));
    REQUIRE(img.GetPixel(1, 0) == approx(Rgba(0, 0, 0, 1)));
  }

  SECTION("can set and get color")
  {
    REQUIRE(img.GetPixel(0, 0) == approx(Rgba(0, 0, 0, 1)));
    Rgba color{1, 1, 1, 1};
    img.SetPixel(0, 0, color);
    REQUIRE(img.GetPixel(0, 0) == approx(color));
  }
}

TEST_CASE("image transparent", "[img]")
{
  Image img;
  img.Setup(4, 4, true);

  SECTION("default-is-black")
  {
    REQUIRE(img.GetPixel(0, 0) == approx(Rgba(0, 0, 0, 0)));
    REQUIRE(img.GetPixel(1, 1) == approx(Rgba(0, 0, 0, 0)));
  }

  SECTION("can set and get color")
  {
    REQUIRE(img.GetPixel(0, 0) == approx(Rgba(0, 0, 0, 0)));
    Rgba color{1, 1, 1, 1};
    img.SetPixel(0, 0, color);
    REQUIRE(img.GetPixel(0, 0) == approx(color));
  }
}

// todo: add paint test
TEST_CASE("image draw", "[img]")
{
  Image     img;
  const int width  = 10;
  const int height = 12;
  img.Setup(width, height, false);
  Draw draw{&img};

  SECTION("draw size is image size")
  {
    const auto size = draw.WholeImage();
    CHECK(size.bottom == 0);
    CHECK(size.left == 0);
    CHECK(size.GetWidth() == width);
    CHECK(size.GetHeight() == height);
  }

  Rgb  color{1, 1, 0};
  Rgba colora{color, 1};

  SECTION("fill")
  {
    CHECK_FALSE(img.GetPixel(0, 0) == approx(colora));
    CHECK_FALSE(img.GetPixel(3, 3) == approx(colora));
    CHECK_FALSE(img.GetPixel(width - 1, height - 1) == approx(colora));

    draw.Square(color, draw.WholeImage());
    REQUIRE(img.GetPixel(0, 0) == approx(colora));
    REQUIRE(img.GetPixel(3, 3) == approx(colora));
    REQUIRE(img.GetPixel(width - 1, height - 1) == approx(colora));
  }

  SECTION("circle")
  {
    CHECK_FALSE(img.GetPixel(5, 5) == approx(colora));
    draw.Circle(color, vec2i{5, 5}, 4);
    CHECK_FALSE(img.GetPixel(0, 0) == approx(colora));
    REQUIRE(img.GetPixel(5, 5) == approx(colora));
  }

  SECTION("circle with hole")
  {
    CHECK_FALSE(img.GetPixel(0, 0) == approx(colora));
    CHECK_FALSE(img.GetPixel(5, 5) == approx(colora));
    draw.Circle(color, vec2i{5, 5}, 20, 0, 3);
    CHECK_FALSE(img.GetPixel(5, 5) == approx(colora));
    REQUIRE(img.GetPixel(0, 0) == approx(colora));
  }
}
