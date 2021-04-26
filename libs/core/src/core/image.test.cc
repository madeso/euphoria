#include "core/image.h"

#include "core/vfs.h"
#include "core/vfs_path.h"
#include "core/base64.h"
#include "core/rgb.h"
#include "core/image_draw.h"
#include "core/image_to_text.h"
#include "core/loadedfont.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"

namespace vfs  = euphoria::core::vfs;
namespace euco = euphoria::core;

using namespace euphoria::tests;


// 4x4 image without transperency
// white  / red
// green / blue
const char* const TEST_IMAGE
        = "iVBORw0KGgoAAAANSUhEUgAAAAIAAAACCAIAAAD91JpzAAAABmJLR0QA/wD/"
          "AP+"
          "gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH4QgdFSAdb6CP5AAAABVJREFUCNcFw"
          "QEBAAAAgBD/TxeqIDQz2gX7fv3PJgAAAABJRU5ErkJggg==";


TEST_CASE("image-load", "[img]")
{
    vfs::FileSystem fs;
    auto catalog = vfs::FileSystemRootCatalog::AddRoot(&fs);
    catalog->RegisterFileData
    (
        vfs::FilePath{"~/white"},
        euco::base64::decode(TEST_IMAGE)
    );

    auto loaded = euco::load_image
    (
        &fs,
        vfs::FilePath{"~/white"},
        euco::alpha_load::Remove
    );
    REQUIRE(loaded.error.empty());
    REQUIRE_FALSE(loaded.image.has_alpha);

    REQUIRE(loaded.image.height == 2);
    REQUIRE(loaded.image.width == 2);

    // upper left
    SECTION("load-white")
    {
        const auto pixel = loaded.image.get_pixel(0, 1);
        const auto white = euco::rgbai {euco::rgbi {255, 255, 255}, 255};
        REQUIRE(pixel == white);
    }

    // upper right
    SECTION("load-red")
    {
        const auto pixel = loaded.image.get_pixel(1, 1);
        const auto red = euco::rgbai {euco::rgbi {255, 0, 0}, 255};
        REQUIRE(pixel == red);
    }

    // lower left
    SECTION("load-green")
    {
        const auto pixel = loaded.image.get_pixel(0, 0);
        const auto green = euco::rgbai {euco::rgbi {0, 255, 0}, 255};
        REQUIRE(pixel == green);
    }

    // lower right
    SECTION("load-blue")
    {
        const auto pixel = loaded.image.get_pixel(1, 0);
        const auto blue  = euco::rgbai {euco::rgbi {0, 0, 255}, 255};
        REQUIRE(pixel == blue);
    }
}

TEST_CASE("image solid", "[img]")
{
    euco::image img;
    img.setup_no_alpha_support(3, 3);

    SECTION("default-is-black")
    {
        REQUIRE(img.get_pixel(0, 0) == euco::rgbai(euco::rgbi{0, 0, 0}, 255));
        REQUIRE(img.get_pixel(1, 0) == euco::rgbai(euco::rgbi{0, 0, 0}, 255));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.get_pixel(0, 0) == euco::rgbai(euco::rgbi{0, 0, 0}, 255));
        euco::rgbai color {euco::rgbi {255, 255, 255}, 255};
        img.set_pixel(0, 0, color);
        REQUIRE(img.get_pixel(0, 0) == color);
    }
}


TEST_CASE("image transparent", "[img]")
{
    euco::image img;
    img.setup_with_alpha_support(4, 4);

    SECTION("default-is-black")
    {
        REQUIRE(img.get_pixel(0, 0) == euco::rgbai(euco::rgbi{0, 0, 0}, 0));
        REQUIRE(img.get_pixel(1, 1) == euco::rgbai(euco::rgbi{0, 0, 0}, 0));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.get_pixel(0, 0) == euco::rgbai(euco::rgbi{0, 0, 0}, 0));
        euco::rgbai color {euco::rgbi {255, 255, 255}, 255};
        img.set_pixel(0, 0, color);
        REQUIRE(img.get_pixel(0, 0) == color);
    }
}


// todo(Gustav): add paint test
TEST_CASE("image draw", "[img]")
{
    euco::image img;
    const int   width  = 10;
    const int   height = 12;
    img.setup_no_alpha_support(width, height);

    SECTION("draw size is image size")
    {
        const auto size = euco::whole_image(img);
        CHECK(size.bottom == 0);
        CHECK(size.left == 0);
        CHECK(size.get_width() == width);
        CHECK(size.get_height() == height);
    }

    euco::rgbi  color {255, 255, 0};
    euco::rgbai colora {color, 255};

    SECTION("fill")
    {
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(3, 3) == colora);
        CHECK_FALSE(img.get_pixel(width - 1, height - 1) == colora);

        euco::draw_rect(&img, color, euco::whole_image(img));
        REQUIRE(img.get_pixel(0, 0) == colora);
        REQUIRE(img.get_pixel(3, 3) == colora);
        REQUIRE(img.get_pixel(width - 1, height - 1) == colora);
    }

    SECTION("circle")
    {
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        euco::draw_circle(&img, crgb(color), euco::vec2i {5, 5}, 4);
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        REQUIRE(img.get_pixel(5, 5) == colora);
    }

    SECTION("circle with hole")
    {
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        euco::draw_circle(&img, crgb(color), euco::vec2i {5, 5}, 20, 0, 3);
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        REQUIRE(img.get_pixel(0, 0) == colora);
    }

    SECTION("paste big with clip")
    {
        euco::image big;
        big.setup_no_alpha_support(width * 3, height * 3);
        euco::clear(&big, color);

        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(0, height-1) == colora);
        CHECK_FALSE(img.get_pixel(width-1, 0) == colora);
        CHECK_FALSE(img.get_pixel(width-1, height-1) == colora);

        paste_image(&img, euco::vec2i(-width, -height), big, euco::blend_mode::normal, euco::pixels_outside::Discard);

        CHECK(img.get_pixel(5, 5) == colora);
        CHECK(img.get_pixel(0, 0) == colora);
        CHECK(img.get_pixel(0, height-1) == colora);
        CHECK(img.get_pixel(width-1, 0) == colora);
        CHECK(img.get_pixel(width-1, height-1) == colora);
    }

    SECTION("paste small")
    {
        euco::image small;
        small.setup_no_alpha_support(2, 2);
        euco::clear(&small, color);

        paste_image(&img, euco::vec2i(5, 5), small);
        CHECK(img.get_pixel(5, 5) == colora);
        CHECK(img.get_pixel(6, 5) == colora);
        CHECK(img.get_pixel(5, 6) == colora);
        CHECK(img.get_pixel(6, 6) == colora);

        CHECK_FALSE(img.get_pixel(4, 5) == colora);
        CHECK_FALSE(img.get_pixel(5, 4) == colora);
        CHECK_FALSE(img.get_pixel(4, 4) == colora);
    }
}


TEST_CASE("image text")
{
    auto draw_text = [](const std::string& text, int width, int height) -> std::vector<std::string>
    {
        euco::image image;
        image.setup_no_alpha_support(width, height);
        euco::draw_rect(&image, {euco::color::white}, euco::whole_image(image));
        euco::draw_text
        (
            &image,
            euco::vec2i(0,height-8),
            text,
            {euco::color::black},
            euco::LoadCharactersFromBuiltin8()
        );
        const auto table = euco::image_to_string_table_exact
        (
            image,
            {
                {'#', euco::color::black},
                {' ', euco::color::white}
            },
            '?'
        );
        const auto strings = euco::to_strings(table);
        return strings;
    };

    CHECK(StringEq(draw_text("dog !", 8*6, 8),
        {
    "   ###                             ##           ",
    "    ##                            ####          ",
    "    ##   ####    ### ##           ####          ",
    " #####  ##  ##  ##  ##             ##           ",
    "##  ##  ##  ##  ##  ##             ##           ",
    "##  ##  ##  ##   #####                          ",
    " ### ##  ####       ##             ##           ",
    "                #####                           "
        }));

    CHECK(StringEq(draw_text("CAT", 8, 8+1),
        {
            "  ####  ",
            " ##  ## ",
            "##      ",
            "##      ",
            "##      ",
            " ##  ## ",
            "  ####  ",
            "        ",
            "        "
        }));

    CHECK(StringEq(draw_text("c\na\nt", 8, 8*3),
        {
            "        ",
            "        ",
            " ####   ",
            "##  ##  ",
            "##      ",
            "##  ##  ",
            " ####   ",
            "        ",
            "        ",
            "        ",
            " ####   ",
            "    ##  ",
            " #####  ",
            "##  ##  ",
            " ### ## ",
            "        ",
            "   #    ",
            "  ##    ",
            " #####  ",
            "  ##    ",
            "  ##    ",
            "  ## #  ",
            "   ##   ",
            "        "
        }));
}
