#include "core/collage.h"

#include <range/v3/all.hpp>

#include "core/image.h"
#include "base/rgb.h"
#include "core/image_draw.h"
#include "core/pack.h"
#include "base/cint.h"
#include "base/numeric.h"


namespace eu::core
{
    std::vector<size2i> collect_sizes
    (
        const std::vector<Image>& images,
        int padding
    )
    {
        auto sizes = std::vector<size2i>{};

        for(const auto& img: images)
        {
            sizes.emplace_back(size2i::create_from_width_height
            (
                img.width + padding,
                img.height + padding
            ));
        }

        return sizes;
    }


    Image
    draw_image
    (
        const std::vector<vec2i>& positions,
        const std::vector<Image>& images,
        const size2i& size,
        const Rgbi& background_color
    )
    {
        auto composed_image = Image{};
        composed_image.setup_with_alpha_support
        (
            size.width,
            size.height
        );
        clear(&composed_image, background_color);

        for(const auto& [position, image]: ranges::views::zip(positions, images))
        {
            paste_image
            (
                &composed_image,
                position,
                image,
                BlendMode::normal,
                PixelsOutside::discard
            );
        }

        return composed_image;
    }


    size2i
    pack_tight
    (
        const size2i& default_size,
        std::vector<vec2i>* positions,
        const std::vector<Image>& images,
        int padding
    )
    {
        std::optional<Recti> bb = std::nullopt;

        for(const auto& [position, img]: ranges::views::zip(*positions, images))
        {
            const auto image_width = img.width;
            const auto image_height = img.height;
            const auto& rect = Recti::from_bottom_left_width_height
            (
                vec2i(position.x, position.y),
                image_width + padding,
                image_height + padding
            );
            if(bb.has_value())
            { bb->include(rect); }
            else
            { bb = rect; }
        }
        if(!bb) { return default_size; }

        const auto size = bb->get_size();
        const auto dx = -bb->left;
        const auto dy = -bb->bottom;

        for(auto& position: *positions)
        {
            position += vec2i(dx, dy);
        }

        return size2i::create_from_width_height(size.width + padding, size.height + padding);
    }

    ////////////////////////////////////////////////////


    std::vector<vec2i>
    pack_image
    (
        const size2i& image_size,
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        int padding
    )
    {
        const auto image_sizes = collect_sizes(images, padding);
        const auto packed = pack(image_size, image_sizes);

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
                ret.emplace_back(rect->get_bottom_left());
            }
        }

        return ret;
    }


    std::optional<Image>
    pack_image
    (
        const std::vector<Image>& images,
        const std::vector<std::string>& files,
        const size2i& requested_size,
        int padding,
        Rgbi background_color,
        bool should_pack_image
    )
    {
        if( requested_size.width < padding ||
            requested_size.height < padding)
        {
            return std::nullopt;
        }

        const auto image_size = size2i::create_from_width_height
        (
            requested_size.width - padding,
            requested_size.height - padding
        );

        // pack images
        auto packed = pack_image(image_size, images, files, padding);

        if(packed.empty())
        {
            return std::nullopt;
        }

        // optionally reduce image size
        const auto size = should_pack_image
            ? pack_tight(requested_size, &packed, images, padding)
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
        auto composed_image = draw_image(packed, images, size, background_color);

        return composed_image;
    }


    ////////////////////////////////////////////////////////////


    std::pair<std::vector<vec2i>, size2i>
    lay_out_in_a_grid
    (
        const std::vector<Image>& images,
        int padding,
        bool top_to_bottom
    )
    {
        const auto images_per_row = c_float_to_int
        (
            ceil
            (
                sqrt
                (
                    c_sizet_to_float
                    (
                        images.size()
                    )
                )
            )
        );

        auto ret = std::vector<vec2i>{};

        int x = padding;
        int y = padding;
        int current_height = 0;
        int column = 0;
        int max_x = 0;

        auto add_row = [&]
        {
            max_x = max(max_x, x);
            column = 0;
            x = padding;
            y += current_height + padding;
            current_height = 0;
        };

        for(const auto& src: images)
        {
            const auto width = src.width;
            const auto height = src.height;

            ret.emplace_back
            (
                x, y
            );

            x += width + padding;
            current_height = max(current_height, height);
            column += 1;

            if(column >= images_per_row)
            {
                add_row();
            }
        }

        if(column != 0)
        {
            add_row();
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
                position->y = image_height - (position->y + img.height);
            }
        }

        return {ret, size2i::create_from_width_height(image_width, image_height)};
    }


    Image
    lay_out_in_a_grid
    (
        const std::vector<Image>& images,
        int padding,
        Rgbi background_color,
        bool top_to_bottom
    )
    {
        // layout images in grid and calculate image size from grid
        const auto [image_grid, size] = lay_out_in_a_grid(images, padding, top_to_bottom);

        // draw new image
        auto composed_image = draw_image(image_grid, images, size, background_color);

        return composed_image;
    }

}

