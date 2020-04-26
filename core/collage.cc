#include "core/collage.h"

#include <range/v3/all.hpp>

#include "core/image.h"
#include "core/rgb.h"
#include "core/image_draw.h"
#include "core/pack.h"


namespace euphoria::core
{
    std::vector<Sizei> CollectSizes
    (
        const std::vector<Image>& images,
        int padding
    )
    {
        auto sizes = std::vector<Sizei>{};

        for(const auto& img: images)
        {
            sizes.emplace_back(Sizei::FromWidthHeight
            (
                img.GetWidth() + padding,
                img.GetHeight() + padding
            ));
        }

        return sizes;
    }


    Image
    DrawImage
    (
        const std::vector<vec2i>& positions,
        const std::vector<Image>& images,
        const Sizei& size,
        const Rgbi& background_color
    )
    {
        auto composed_image = Image{};
        composed_image.SetupWithAlphaSupport
        (
            size.width,
            size.height
        );
        Clear(&composed_image, background_color);

        for(const auto& [position, image]: ranges::views::zip(positions, images))
        {
            PasteImage
            (
                &composed_image,
                position,
                image,
                BlendMode::Normal,
                PixelsOutside::Discard
            );
        }

        return composed_image;
    }


    Sizei
    PackTight
    (
        const Sizei& default_size,
        std::vector<vec2i>* positions,
        const std::vector<Image>& images,
        int padding
    )
    {
        std::optional<Recti> bb = std::nullopt;

        for(const auto& [position, img]: ranges::views::zip(*positions, images))
        {
            const auto image_width = img.GetWidth();
            const auto image_height = img.GetHeight();
            const auto& rect = Recti::FromBottomLeftWidthHeight
            (
                vec2i(position.x, position.y),
                image_width + padding,
                image_height + padding
            );
            if(bb.has_value())
            { bb->Include(rect); }
            else
            { bb = rect; }
        }
        if(!bb) { return default_size; }

        const auto size = bb->GetSize();
        const auto dx = -bb->left;
        const auto dy = -bb->bottom;

        for(auto& position: *positions)
        {
            position += vec2i(dx, dy);
        }

        return Sizei::FromWidthHeight(size.width + padding, size.height+padding);
    }

    ////////////////////////////////////////////////////


    std::vector<vec2i>
    PackImage
    (
        const Sizei& image_size,
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        int padding
    )
    {
        const auto image_sizes = CollectSizes(images, padding);
        const auto packed = Pack(image_size, image_sizes);

        auto ret = std::vector<vec2i>{};

        int i = 0;
        for(const auto& rect: packed)
        {
            const auto& file = files[i];
            i += 1;

            if(rect.has_value() == false)
            {
                std::cerr << "failed to pack " << file << "\n";
            }
            else
            {
                ret.emplace_back(rect->BottomLeft());
            }
        }

        return ret;
    }


    std::optional<Image>
    PackImage
    (
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        const Sizei& requested_size,
        int padding,
        Rgbi background_color,
        bool pack_image
    )
    {
        if( requested_size.width < padding ||
            requested_size.height < padding)
        {
            return std::nullopt;
        }

        const auto image_size = Sizei::FromWidthHeight
        (
            requested_size.width - padding,
            requested_size.height - padding
        );

        // pack images
        auto packed = PackImage(image_size, images, files, padding);

        if(packed.empty())
        {
            return std::nullopt;
        }

        // optionally reduce image size
        const auto size = pack_image
            ? PackTight(requested_size, &packed, images, padding)
            : requested_size
            ;

        // border-theory:
        // reuqested_size is decreased with padding
        // the tighlty packed size is increased with padding
        // the packed image-sizes is increased with padding
        // and finally all the images are offseted by padding
        // all to keep padding-sized border between the images
        for(auto& position: packed)
        {
            position += vec2i(padding, padding);
        }

        // draw new image
        auto composed_image = DrawImage(packed, images, size, background_color);

        return composed_image;
    }


    ////////////////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, Sizei>
    GridLayout
    (
        const std::vector<Image>& images,
        int padding,
        bool top_to_bottom
    )
    {
        const auto images_per_row = Ceil(Sqrt(images.size()));

        auto ret = std::vector<vec2i>{};

        int x = padding;
        int y = padding;
        int current_height = 0;
        int column = 0;
        int max_x = 0;

        auto new_row = [&]
        {
            max_x = Max(max_x, x);
            column = 0;
            x = padding;
            y += current_height + padding;
            current_height = 0;
        };

        for(const auto& src: images)
        {
            const auto width = src.GetWidth();
            const auto height = src.GetHeight();

            ret.emplace_back
            (
                x, y
            );

            x += width + padding;
            current_height = Max(current_height, height);
            column += 1;

            if(column >= images_per_row)
            {
                new_row();
            }
        }

        if(column != 0)
        {
            new_row();
        }

        const auto image_width = max_x;
        const auto image_height = y;

        // we lay out left->right, bottom->top since that is easer
        // but top->bottom is ieasier to read, so we provide a option
        // to switch to that, and we do that by inverting it at the end
        if(top_to_bottom)
        {
            const auto into_pointer = [](vec2i& v) -> vec2i* { return &v; };
            using namespace ranges::views;
            for
            (
                const auto& [position, img]:
                    zip(ret|transform(into_pointer), images)
            )
            {
                position->y = image_height - (position->y + img.GetHeight());
            }
        }

        return {ret, Sizei::FromWidthHeight(image_width, image_height)};
    }


    Image
    GridLayout
    (
        const std::vector<Image>& images,
        int padding,
        Rgbi background_color,
        bool top_to_bottom
    )
    {
        // layout images in grid and calculate image size from grid
        const auto [image_grid, size] = GridLayout(images, padding, top_to_bottom);

        // draw new image
        auto composed_image = DrawImage(image_grid, images, size, background_color);

        return composed_image;
    }

}

