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
    const auto white = Rgbai{Rgbi{255, 255, 255}, 255};
    REQUIRE(pixel == white);
  }

  // upper right
  SECTION("load-red")
  {
    const auto pixel = loaded.image.GetPixel(1, 1);
    const auto red   = Rgbai{Rgbi{255, 0, 0}, 255};
    REQUIRE(pixel == red);
  }

  // lower left
  SECTION("load-green")
  {
    const auto pixel = loaded.image.GetPixel(0, 0);
    const auto green = Rgbai{Rgbi{0, 255, 0}, 255};
    REQUIRE(pixel == green);
  }

  // lower right
  SECTION("load-blue")
  {
    const auto pixel = loaded.image.GetPixel(1, 0);
    const auto blue  = Rgbai{Rgbi{0, 0, 255}, 255};
    REQUIRE(pixel == blue);
  }
}

TEST_CASE("image solid", "[img]")
{
  Image img;
  img.SetupNoAlphaSupport(3, 3);

  SECTION("default-is-black")
  {
    REQUIRE(img.GetPixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
    REQUIRE(img.GetPixel(1, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
  }

  SECTION("can set and get color")
  {
    REQUIRE(img.GetPixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
    Rgbai color{Rgbi{255, 255, 255}, 255};
    img.SetPixel(0, 0, color);
    REQUIRE(img.GetPixel(0, 0) == color);
  }
}

TEST_CASE("image transparent", "[img]")
{
  Image img;
  img.SetupWithAlphaSupport(4, 4);

  SECTION("default-is-black")
  {
    REQUIRE(img.GetPixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 0));
    REQUIRE(img.GetPixel(1, 1) == Rgbai(Rgbi{0, 0, 0}, 0));
  }

  SECTION("can set and get color")
  {
    REQUIRE(img.GetPixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 0));
    Rgbai color{Rgbi{255, 255, 255}, 255};
    img.SetPixel(0, 0, color);
    REQUIRE(img.GetPixel(0, 0) == color);
  }
}

// todo: add paint test
TEST_CASE("image draw", "[img]")
{
  Image     img;
  const int width  = 10;
  const int height = 12;
  img.SetupNoAlphaSupport(width, height);
  Draw draw{&img};

  SECTION("draw size is image size")
  {
    const auto size = draw.WholeImage();
    CHECK(size.bottom == 0);
    CHECK(size.left == 0);
    CHECK(size.GetWidth() == width);
    CHECK(size.GetHeight() == height);
  }

  Rgbi  color{255, 255, 0};
  Rgbai colora{color, 255};

  SECTION("fill")
  {
    CHECK_FALSE(img.GetPixel(0, 0) == colora);
    CHECK_FALSE(img.GetPixel(3, 3) == colora);
    CHECK_FALSE(img.GetPixel(width - 1, height - 1) == colora);

    draw.Square(color, draw.WholeImage());
    REQUIRE(img.GetPixel(0, 0) == colora);
    REQUIRE(img.GetPixel(3, 3) == colora);
    REQUIRE(img.GetPixel(width - 1, height - 1) == colora);
  }

  SECTION("circle")
  {
    CHECK_FALSE(img.GetPixel(5, 5) == colora);
    draw.Circle(rgb(color), point2i{5, 5}, 4);
    CHECK_FALSE(img.GetPixel(0, 0) == colora);
    REQUIRE(img.GetPixel(5, 5) == colora);
  }

  SECTION("circle with hole")
  {
    CHECK_FALSE(img.GetPixel(0, 0) == colora);
    CHECK_FALSE(img.GetPixel(5, 5) == colora);
    draw.Circle(rgb(color), point2i{5, 5}, 20, 0, 3);
    CHECK_FALSE(img.GetPixel(5, 5) == colora);
    REQUIRE(img.GetPixel(0, 0) == colora);
  }
}
