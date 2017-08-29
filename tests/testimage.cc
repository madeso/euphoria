#include "core/image.h"
#include "core/filesystem.h"
#include "core/base64.h"
#include "core/rgb.h"
#include "tests/testbase.h"

#include "catch.hpp"

const char* const WHITE_IMAGE =
    "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAIAAAACUFjqAAAABmJLR0QA/wD/"
    "AP+"
    "gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4QgdFRwdHFP2GwAAABVJREFUGNNj/"
    "P//PwNuwMSAF4xUaQCl4wMR/9A5uQAAAABJRU5ErkJggg==";

TEST_CASE("image-load", "[img]")
{
  FileSystem fs;
  auto       catalog = FileSystemRootCatalog::AddRoot(&fs);
  catalog->RegisterFileData("white", base64::Decode(WHITE_IMAGE));

  SECTION("load-white")
  {
    auto loaded = LoadImage(&fs, "white", AlphaLoad::Remove);
    REQUIRE(loaded.error == "");
    REQUIRE_FALSE(loaded.image.HasAlpha());

    const auto pixel = loaded.image.GetPixel(0, 0);
    const auto white = Rgba{1.0f, 1.0f, 1.0f, 1.0f};
    REQUIRE(pixel == approx(white));
  }
}
