#pragma once



#include <memory>

#include "core/rgb.h"
#include "core/memorychunk.h"
#include "core/rect.h"

namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::core
{
    enum class ImageWriteFormat
    {
        png,
        bmp,
        tga,
        jpeg
    };


    struct Image
    {
        // todo(Gustav): replace with a array instead of a vector
        std::vector<unsigned char> components;
        int width = 0;
        int height = 0;
        bool has_alpha = false;

        // todo(Gustav): Add create function that will create empty image and run setup
        

        void make_invalid();

        void setup(int image_width, int image_height, bool alpha, int default_value);

        /// if default value is negative, default value is ignored, otherwise its the default value for both R, G, B, and A.
        void setup_with_alpha_support
        (
            int image_width,
            int image_height,
            int default_value = 0
        );

        void setup_no_alpha_support
        (
            int image_width,
            int image_height,
            int default_value = 0
        );

        void set_pixel(int x, int y, const rgbai& color);

        void set_pixel
        (
            int x,
            int y,
            unsigned char r,
            unsigned char g,
            unsigned char b,
            unsigned char a
        );

        template <typename TFunc>
        void filter(TFunc f)
        {
            for(int y = 0; y < height; y += 1)
            {
                for(int x = 0; x < width; x += 1)
                {
                    set_pixel(x, y, f(get_pixel(x, y)));
                }
            }
        }

        template <typename TFunc>
        void for_all_top_bottom(TFunc f)
        {
            for(int y = height; y > 0; y -= 1)
            {
                for(int x = 0; x < width; x += 1)
                {
                    f(x, y - 1, get_pixel(x, y - 1));
                }
            }
        }

        template <typename TFunc>
        void set_all_top_bottom(TFunc f)
        {
            for(int y = height; y > 0; y -= 1)
            {
                for(int x = 0; x < width; x += 1)
                {
                    set_pixel(x, y - 1, f(x, y - 1));
                }
            }
        }

        template <typename TFunc>
        void set_all_bottom_top(TFunc f)
        {
            for(int y = 0; y < height; y += 1)
            {
                for(int x = 0; x < width; x += 1)
                {
                    set_pixel(x, y, f(x, y));
                }
            }
        }

        [[nodiscard]] rgbai get_pixel(int x, int y) const;
        [[nodiscard]] std::shared_ptr<MemoryChunk> write(ImageWriteFormat format, int jpeg_quality = 100) const;
        [[nodiscard]] int get_pixel_index(int x, int y) const;

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] Recti get_indices() const;
        [[nodiscard]] const unsigned char* get_pixel_data() const;
        [[nodiscard]] int get_pixel_byte_size() const;

        
    };


    struct ImageLoadResult
    {
        core::Image image;
        std::string error;
    };


    enum class AlphaLoad
    {
        remove,
        keep
    };


    // todo(Gustav): move image loading to a io library instead
    ImageLoadResult
    load_image(vfs::FileSystem* fs, const vfs::FilePath& path, AlphaLoad alpha);


    ImageLoadResult
    load_image
    (
        std::shared_ptr<MemoryChunk> channel,
        const std::string& path,
        AlphaLoad alpha
    );


    ImageLoadResult
    load_image
    (
        void* compressed_data,
        int compressed_size,
        const std::string& path,
        AlphaLoad alpha
    );
}
