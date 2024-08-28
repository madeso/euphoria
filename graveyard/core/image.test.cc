#include "core/image.h"

#include "io/vfs.h"
#include "io/vfs_path.h"
#include "base/rgb.h"
#include "core/image_draw.h"
#include "core/image_to_text.h"
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


// todo(Gustav): add paint test
TEST_CASE("image draw", "[img]")
{
    core::Image img;
    const int width = 10;
    const int height = 12;
    img.setup_no_alpha_support(width, height);

    SECTION("draw size is image size")
    {
        const auto size = core::on_whole_image(img);
        CHECK(size.bottom == 0);
        CHECK(size.left == 0);
        CHECK(size.get_width() == width);
        CHECK(size.get_height() == height);
    }

    Rgbi color {255, 255, 0};
    Rgbai colora {color, 255};

    SECTION("fill")
    {
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(3, 3) == colora);
        CHECK_FALSE(img.get_pixel(width - 1, height - 1) == colora);

        core::draw_rect(&img, color, core::on_whole_image(img));
        REQUIRE(img.get_pixel(0, 0) == colora);
        REQUIRE(img.get_pixel(3, 3) == colora);
        REQUIRE(img.get_pixel(width - 1, height - 1) == colora);
    }

    SECTION("circle")
    {
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        core::draw_circle(&img, to_rgb(color), vec2i{5, 5}, 4);
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        REQUIRE(img.get_pixel(5, 5) == colora);
    }

    SECTION("circle with hole")
    {
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        core::draw_circle(&img, to_rgb(color), vec2i{5, 5}, 20, 0, 3);
        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        REQUIRE(img.get_pixel(0, 0) == colora);
    }

    SECTION("paste big with clip")
    {
        core::Image big;
        big.setup_no_alpha_support(width * 3, height * 3);
        core::clear(&big, color);

        CHECK_FALSE(img.get_pixel(5, 5) == colora);
        CHECK_FALSE(img.get_pixel(0, 0) == colora);
        CHECK_FALSE(img.get_pixel(0, height-1) == colora);
        CHECK_FALSE(img.get_pixel(width-1, 0) == colora);
        CHECK_FALSE(img.get_pixel(width-1, height-1) == colora);

        paste_image(&img, vec2i(-width, -height), big, core::BlendMode::normal, core::PixelsOutside::discard);

        CHECK(img.get_pixel(5, 5) == colora);
        CHECK(img.get_pixel(0, 0) == colora);
        CHECK(img.get_pixel(0, height-1) == colora);
        CHECK(img.get_pixel(width-1, 0) == colora);
        CHECK(img.get_pixel(width-1, height-1) == colora);
    }

    SECTION("paste small")
    {
        core::Image small;
        small.setup_no_alpha_support(2, 2);
        core::clear(&small, color);

        paste_image(&img, vec2i(5, 5), small);
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
        core::Image image;
        image.setup_no_alpha_support(width, height);
        core::draw_rect(&image, {NamedColor::white}, core::on_whole_image(image));
        core::draw_text
        (
            &image,
            vec2i(0,height-8),
            text,
            {NamedColor::black},
            core::load_characters_from_builtin8()
        );
        const auto table = core::from_image_to_string_table_exact
        (
            image,
            {
                {'#', NamedColor::black},
                {' ', NamedColor::white}
            },
            '?'
        );
        const auto strings = core::to_strings(table);
        return strings;
    };

    CHECK(is_string_equal(draw_text("dog !", 8 * 6, 8),
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

    CHECK(is_string_equal(draw_text("CAT", 8, 8 + 1),
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

    CHECK(is_string_equal(draw_text("c\na\nt", 8, 8 * 3),
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
