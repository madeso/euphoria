#include "core/image.h"
#include "core/filesystem.h"
#include "core/base64.h"
#include "core/rgb.h"
#include "tests/testbase.h"

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

  SECTION("load-white")
  {
    const auto pixel = loaded.image.GetPixel(0, 0);
    const auto white = Rgba{1.0f, 1.0f, 1.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }

  SECTION("load-red")
  {
    const auto pixel = loaded.image.GetPixel(1, 0);
    const auto white = Rgba{1.0f, 0.0f, 0.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }

  SECTION("load-green")
  {
    const auto pixel = loaded.image.GetPixel(0, 1);
    const auto white = Rgba{0.0f, 1.0f, 0.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }

  SECTION("load-blue")
  {
    const auto pixel = loaded.image.GetPixel(1, 1);
    const auto white = Rgba{0.0f, 0.0f, 1.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }
}
