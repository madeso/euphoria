#include "core/image.h"

#include "io/vfs.h"
#include "io/vfs_path.h"
#include "base/rgb.h"
#include "core/loadedfont.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch2/catch_all.hpp"

using namespace eu;
using namespace eu::io;
using namespace eu::tests;


// 4x4 image without transperency
// white / red
// green / blue
const char* const base64_encoded_test_image
        = "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAABmJLR0QA/wD/"
          "AP+"
          "gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4QgdFSAdb6CP5AAAABVJREFUCNcFw"
          "QEBAAAAgBD/TxeqIDQz2gX7fv3PJgAAAABJRU5ErkJggg==";


TEST_CASE("image-load", "[img]")
{
    FileSystem fs;
    auto catalog = ReadRootCatalog::create_and_add(&fs);
    // catalog->register_file_data
    // (
    //     FilePath{"~/white"},
    //     core::base64::decode(base64_encoded_test_image)
    // );

    auto loaded = core::load_image
    (
        &fs,
        FilePath{"~/white"},
        core::AlphaLoad::remove
    );
    REQUIRE(loaded.error.empty());
    REQUIRE_FALSE(loaded.image.has_alpha);

    REQUIRE(loaded.image.height == 2);
    REQUIRE(loaded.image.width == 2);

    // upper left
    SECTION("load-white")
    {
        const auto pixel = loaded.image.get_pixel(0, 1);
        const auto white = Rgbai{Rgbi{255, 255, 255}, 255};
        REQUIRE(pixel == white);
    }

    // upper right
    SECTION("load-red")
    {
        const auto pixel = loaded.image.get_pixel(1, 1);
        const auto red = Rgbai{Rgbi{255, 0, 0}, 255};
        REQUIRE(pixel == red);
    }

    // lower left
    SECTION("load-green")
    {
        const auto pixel = loaded.image.get_pixel(0, 0);
        const auto green = Rgbai{Rgbi{0, 255, 0}, 255};
        REQUIRE(pixel == green);
    }

    // lower right
    SECTION("load-blue")
    {
        const auto pixel = loaded.image.get_pixel(1, 0);
        const auto blue = Rgbai{Rgbi{0, 0, 255}, 255};
        REQUIRE(pixel == blue);
    }
}

TEST_CASE("image solid", "[img]")
{
    core::Image img;
    img.setup_no_alpha_support(3, 3);

    SECTION("default-is-black")
    {
        REQUIRE(img.get_pixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
        REQUIRE(img.get_pixel(1, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.get_pixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 255));
        Rgbai color {Rgbi {255, 255, 255}, 255};
        img.set_pixel(0, 0, color);
        REQUIRE(img.get_pixel(0, 0) == color);
    }
}


TEST_CASE("image transparent", "[img]")
{
    core::Image img;
    img.setup_with_alpha_support(4, 4);

    SECTION("default-is-black")
    {
        REQUIRE(img.get_pixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 0));
        REQUIRE(img.get_pixel(1, 1) == Rgbai(Rgbi{0, 0, 0}, 0));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.get_pixel(0, 0) == Rgbai(Rgbi{0, 0, 0}, 0));
        Rgbai color {Rgbi {255, 255, 255}, 255};
        img.set_pixel(0, 0, color);
        REQUIRE(img.get_pixel(0, 0) == color);
    }
}
