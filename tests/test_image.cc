#include "core/image.h"

#include "core/vfs.h"
#include "core/base64.h"
#include "core/rgb.h"
#include "core/image_draw.h"
#include "core/imageops.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"
#include "core/fonts.h"

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
    auto            catalog = vfs::FileSystemRootCatalog::AddRoot(&fs);
    catalog->RegisterFileData("white", euco::base64::Decode(TEST_IMAGE));

    auto loaded = euco::LoadImage(&fs, "white", euco::AlphaLoad::Remove);
    REQUIRE(loaded.error == "");
    REQUIRE_FALSE(loaded.image.HasAlpha());

    REQUIRE(loaded.image.GetHeight() == 2);
    REQUIRE(loaded.image.GetWidth() == 2);

    // upper left
    SECTION("load-white")
    {
        const auto pixel = loaded.image.GetPixel(0, 1);
        const auto white = euco::Rgbai {euco::Rgbi {255, 255, 255}, 255};
        REQUIRE(pixel == white);
    }

    // upper right
    SECTION("load-red")
    {
        const auto pixel = loaded.image.GetPixel(1, 1);
        const auto red   = euco::Rgbai {euco::Rgbi {255, 0, 0}, 255};
        REQUIRE(pixel == red);
    }

    // lower left
    SECTION("load-green")
    {
        const auto pixel = loaded.image.GetPixel(0, 0);
        const auto green = euco::Rgbai {euco::Rgbi {0, 255, 0}, 255};
        REQUIRE(pixel == green);
    }

    // lower right
    SECTION("load-blue")
    {
        const auto pixel = loaded.image.GetPixel(1, 0);
        const auto blue  = euco::Rgbai {euco::Rgbi {0, 0, 255}, 255};
        REQUIRE(pixel == blue);
    }
}

TEST_CASE("image solid", "[img]")
{
    euco::Image img;
    img.SetupNoAlphaSupport(3, 3);

    SECTION("default-is-black")
    {
        REQUIRE(img.GetPixel(0, 0) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 255));
        REQUIRE(img.GetPixel(1, 0) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 255));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.GetPixel(0, 0) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 255));
        euco::Rgbai color {euco::Rgbi {255, 255, 255}, 255};
        img.SetPixel(0, 0, color);
        REQUIRE(img.GetPixel(0, 0) == color);
    }
}

TEST_CASE("image transparent", "[img]")
{
    euco::Image img;
    img.SetupWithAlphaSupport(4, 4);

    SECTION("default-is-black")
    {
        REQUIRE(img.GetPixel(0, 0) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 0));
        REQUIRE(img.GetPixel(1, 1) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 0));
    }

    SECTION("can set and get color")
    {
        REQUIRE(img.GetPixel(0, 0) == euco::Rgbai(euco::Rgbi {0, 0, 0}, 0));
        euco::Rgbai color {euco::Rgbi {255, 255, 255}, 255};
        img.SetPixel(0, 0, color);
        REQUIRE(img.GetPixel(0, 0) == color);
    }
}

// todo: add paint test
TEST_CASE("image draw", "[img]")
{
    euco::Image img;
    const int   width  = 10;
    const int   height = 12;
    img.SetupNoAlphaSupport(width, height);

    SECTION("draw size is image size")
    {
        const auto size = euco::WholeImage(img);
        CHECK(size.bottom == 0);
        CHECK(size.left == 0);
        CHECK(size.GetWidth() == width);
        CHECK(size.GetHeight() == height);
    }

    euco::Rgbi  color {255, 255, 0};
    euco::Rgbai colora {color, 255};

    SECTION("fill")
    {
        CHECK_FALSE(img.GetPixel(0, 0) == colora);
        CHECK_FALSE(img.GetPixel(3, 3) == colora);
        CHECK_FALSE(img.GetPixel(width - 1, height - 1) == colora);

        euco::DrawRect(&img, color, euco::WholeImage(img));
        REQUIRE(img.GetPixel(0, 0) == colora);
        REQUIRE(img.GetPixel(3, 3) == colora);
        REQUIRE(img.GetPixel(width - 1, height - 1) == colora);
    }

    SECTION("circle")
    {
        CHECK_FALSE(img.GetPixel(5, 5) == colora);
        euco::DrawCircle(&img, rgb(color), euco::vec2i {5, 5}, 4);
        CHECK_FALSE(img.GetPixel(0, 0) == colora);
        REQUIRE(img.GetPixel(5, 5) == colora);
    }

    SECTION("circle with hole")
    {
        CHECK_FALSE(img.GetPixel(0, 0) == colora);
        CHECK_FALSE(img.GetPixel(5, 5) == colora);
        euco::DrawCircle(&img, rgb(color), euco::vec2i {5, 5}, 20, 0, 3);
        CHECK_FALSE(img.GetPixel(5, 5) == colora);
        REQUIRE(img.GetPixel(0, 0) == colora);
    }
}

TEST_CASE("image text")
{
    auto draw_text = [](const std::string& text, int width, int height) -> std::vector<std::string>
    {
        euco::Image image;
        image.SetupNoAlphaSupport(width, height);
        euco::DrawRect(&image, euco::Color::White, euco::WholeImage(image));
        euco::DrawText(&image, euco::vec2i(0,0), text, euco::Color::Black, euco::LoadCharactersFromBuiltin());
        const auto table = euco::ImageToStringTable(
            image,
            { {'#', euco::Color::Black}, {' ', euco::Color::White} }); 
        const auto strings = euco::ToStrings(table);
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
}
