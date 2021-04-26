#include "core/image.h"

#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/log.h"
#include "core/cint.h"
#include "core/decompress.h"
#include "core/vfs.h"
#include "core/vfs_path.h"


namespace euphoria::core
{
    void
    image::make_invalid()
    {
        components.resize(0);
        width = 0;
        height = 0;
        has_alpha = false;

        ASSERT(!is_valid());
    }


    int
    image::get_pixel_byte_size() const
    {
        return has_alpha ? 4 : 3;
    }


    void
    image::setup_with_alpha_support
    (
        int image_width,
        int image_height,
        int default_value
    )
    {
        setup(image_width, image_height, true, default_value);
    }


    void
    image::setup_no_alpha_support
    (
        int image_width,
        int image_height,
        int default_value
    )
    {
        setup(image_width, image_height, false, default_value);
    }

    void
    image::setup
    (
        int image_width,
        int image_height,
        bool alpha,
        int default_value
    )
    {
        ASSERTX(image_width > 0, image_width);
        ASSERTX(image_height > 0, image_height);

        width = image_width;
        height = image_height;
        has_alpha = alpha;

        components.resize(0);  // clear all pixels
        const unsigned int size = width * height * get_pixel_byte_size();
        if(default_value < 0)
        {
            components.resize(size);
        }
        else
        {
            ASSERT(default_value <= 255);
            components.resize(size, static_cast<unsigned char>(default_value));
        }

        ASSERT(is_valid());
    }


    int
    image::get_pixel_index(int x, int y) const
    {
        ASSERT(x >= 0 && x < width);
        ASSERT(y >= 0 && y < height);

        return (y * width + x) * get_pixel_byte_size();
    }


    void
    image::set_pixel(int x, int y, const rgbai& color)
    {
        set_pixel(x, y, color.r, color.g, color.b, color.a);
    }


    void
    image::set_pixel
    (
        int x,
        int y,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a
    )
    {
        ASSERTX(IsWithinInclusivei(0, x, width - 1), x, width - 1);
        ASSERTX(IsWithinInclusivei(0, y, height - 1), y, height - 1);

        const auto base_index = get_pixel_index(x, y);
        components[base_index + 0] = r;
        components[base_index + 1] = g;
        components[base_index + 2] = b;

        if(has_alpha)
        {
            components[base_index + 3] = a;
        }
    }


    rgbai
    image::get_pixel(int x, int y) const
    {
        ASSERTX(IsWithinInclusivei(0, x, width - 1), x, width);
        ASSERTX(IsWithinInclusivei(0, y, height - 1), y, height);

        const auto base_index = get_pixel_index(x, y);

        const auto red = components[base_index + 0];
        const auto green = components[base_index + 1];
        const auto blue = components[base_index + 2];

        if(has_alpha)
        {
            const auto alpha = components[base_index + 3];
            return rgbai {rgbi {red, green, blue}, alpha};
        }
        else
        {
            return rgbi {red, green, blue};
        }
    }


    bool
    image::is_valid() const
    {
        return width > 0 && height > 0;
    }


    recti
    image::get_indices() const
    {
        return recti::from_width_height
        (
            width - 1,
            height - 1
        );
    }

    const unsigned char*
    image::get_pixel_data() const
    {
        return &components[0];
    }

    namespace // local
    {
        void
        DetermineImageSize(void* context, void* /*unused*/, int size)
        {
            ASSERT(size >= 0);
            auto* total_size = static_cast<int*>(context);
            *total_size += size;
        }

        void
        WriteToMemoryChunkFile(void* context, void* data, int size)
        {
            ASSERT(size >= 0);
            auto* file = static_cast<MemoryChunkFile*>(context);
            file->Write(data, size);
        }
    }

    int
    WriteImageData
    (
        stbi_write_func* func,
        void* context,
        int w,
        int h,
        int comp,
        const void* data,
        image_write_format format,
        int jpeg_quality
    )
    {
        switch(format)
        {
        case image_write_format::png:
            return stbi_write_png_to_func(func, context, w, h, comp, data, 0);
        case image_write_format::bmp:
            return stbi_write_bmp_to_func(func, context, w, h, comp, data);
        case image_write_format::tga:
            return stbi_write_tga_to_func(func, context, w, h, comp, data);
        case image_write_format::jpeg:
            return stbi_write_jpg_to_func
            (
                func, context, w, h, comp, data, jpeg_quality
            );
        default: DIE("Unhandled case"); return 0;
        }
    }

    std::shared_ptr<MemoryChunk>
    image::write(image_write_format format, int jpeg_quality) const
    {
        const int number_of_components = has_alpha ? 4 : 3;

        std::vector<unsigned char> pixels(width * height * number_of_components, 0);
        for(int y = 0; y < height; y += 1)
        {
            const int iy = height - (y + 1);

            ASSERTX(IsWithinInclusivei(0, iy, height - 1), iy, y, height);
            for(int x = 0; x < width; x += 1)
            {
                const int target_index = (x + width * y) * number_of_components;
                const int source_index = (x + width * iy) * number_of_components;
                for(int component = 0; component < number_of_components; component += 1)
                {
                    pixels[target_index + component] = components[source_index + component];
                }
            }
        }

        int size = 0;
        int size_result = WriteImageData
        (
            DetermineImageSize,
            &size,
            width,
            height,
            number_of_components,
            &pixels[0],
            format,
            jpeg_quality
        );
        if(size_result == 0)
        {
            return MemoryChunk::Null();
        }

        ASSERT(size > 0);
        MemoryChunkFile file {MemoryChunk::Alloc(size)};
        int write_result = WriteImageData
        (
            WriteToMemoryChunkFile,
            &file,
            width,
            height,
            number_of_components,
            &pixels[0],
            format,
            jpeg_quality
        );
        if(write_result == 0)
        {
            return MemoryChunk::Null();
        }

        return file.data;
    }


    namespace
    {
        unsigned char
        Select
        (
            int ch,
            unsigned char a,
            unsigned char b,
            unsigned char c,
            unsigned char d
        )
        {
            switch(ch)
            {
            case 1: return a;  // grey
            case 2: return b;  // grey, alpha
            case 3: return c;  // red, green, blue
            case 4: return d;  // red, green, blue, alpha
            default:
                DIE("unhandled Select channel");
                return 0;
            }
        }
    }  // namespace

    image_load_result
    load_image(vfs::FileSystem* fs, const vfs::FilePath& path, alpha_load alpha)
    {
        auto file_memory = fs->ReadFile(path);
        if(file_memory == nullptr)
        {
            image_load_result result;
            result.error = "File doesnt exist";
            result.image.make_invalid();
            LOG_ERROR("Failed to open {0} File doesnt exist.", path);
            return result;
        }

        return load_image(file_memory, path.path, alpha);
    }

    image_load_result
    load_image
    (
        std::shared_ptr<MemoryChunk> file_memory,
        const std::string& path,
        alpha_load alpha
    )
    {
        int channels = 0;
        int image_width = 0;
        int image_height = 0;

        // signed to unsigned cast is probably ok since both are considered to be a chunk of memory
        // https://stackoverflow.com/questions/310451/should-i-use-static-cast-or-reinterpret-cast-when-casting-a-void-to-whatever
        unsigned char* data = stbi_load_from_memory
        (
            reinterpret_cast<unsigned char*>(file_memory->GetData()),  // NOLINT
            file_memory->GetSize(),
            &image_width,
            &image_height,
            &channels,
            0
        );

        if(data == nullptr)
        {
            image_load_result result;
            result.error = stbi_failure_reason();
            result.image.make_invalid();
            LOG_ERROR("Failed to read {0}: {1}", path, result.error);
            return result;
        }

        bool has_alpha = false;
        if(alpha == alpha_load::Keep)
        {
            has_alpha = channels == 2 || channels == 4;
        }

        if(false)
        {
            LOG_INFO
            (
                "Image: {0} {1}x{2} alpha {3} channels {4}",
                path,
                image_width,
                image_height,
                has_alpha,
                channels
            );
        }

        image_load_result result;
        if(has_alpha)
        {
            result.image.setup_with_alpha_support(image_width, image_height, -1);
        }
        else
        {
            result.image.setup_no_alpha_support(image_width, image_height, -1);
        }

        for(int y = 0; y < image_height; ++y)
        {
            for(int x = 0; x < image_width; ++x)
            {
                const int src_index = (y * image_width + x) * channels;

                // get component values
                const unsigned char zero = 0;
                const unsigned char c1 = data[src_index + 0];  // NOLINT no garbage value
                const unsigned char c2 = (channels <= 1) ? zero : data[src_index + 1];
                const unsigned char c3 = (channels <= 2) ? zero : data[src_index + 2];
                const unsigned char c4 = (channels <= 3) ? zero : data[src_index + 3];

                // Gray, Gray+alpha, RGB, RGB+alpha:     gr   gra  rgb  rgba
                const unsigned char r = c1;  //          c1   c1   c1    c1
                const unsigned char g = Select(channels, c1, c1, c2, c2);
                const unsigned char b = Select(channels, c1, c1, c3, c3);
                const unsigned char a = Select(channels, 255, c2, 255, c4);

                result.image.set_pixel(x, image_height - (y + 1), r, g, b, a);
            }
        }

        stbi_image_free(data);
        return result;
    }

    image_load_result
    load_image
    (
        void* compressed_data,
        int compressed_size,
        const std::string& path,
        alpha_load alpha
    )
    {
        auto dec = decompressor{};
        const unsigned int decompressed_size = decompressor::stb_decompress_length(static_cast<const unsigned char*>(compressed_data));
        auto decompressed = MemoryChunk::Alloc(Cunsigned_int_to_int(decompressed_size));
        const auto len = dec.stb_decompress
        (
            reinterpret_cast<unsigned char*>(decompressed->GetData()), // NOLINT
            reinterpret_cast<const unsigned char*>(compressed_data), // NOLINT
            static_cast<unsigned int>(compressed_size)
        );
        if(len == 0)
        {
            image_load_result result;
            result.error = "failed to decompress before loading image";
            result.image.make_invalid();
            return result;
        }

        return load_image(decompressed, path, alpha);
    }
}

