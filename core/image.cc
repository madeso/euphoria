#include "core/image.h"

#include <iostream>

#include "core/assert.h"
#include "core/numeric.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "core/log.h"
#include "core/decompress.h"

namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.image")

    void
    Image::MakeInvalid()
    {
        components.resize(0);
        width_     = 0;
        height_    = 0;
        has_alpha_ = false;

        ASSERT(!IsValid());
    }

    int
    Image::GetPixelByteSize() const
    {
        return has_alpha_ ? 4 : 3;
    }

    void
    Image::SetupWithAlphaSupport(
            int image_width,
            int image_height,
            int default_value)
    {
        Setup(image_width, image_height, true, default_value);
    }

    void
    Image::SetupNoAlphaSupport(
            int image_width,
            int image_height,
            int default_value)
    {
        Setup(image_width, image_height, false, default_value);
    }

    void
    Image::Setup(
            int  image_width,
            int  image_height,
            bool alpha,
            int  default_value)
    {
        ASSERTX(image_width > 0, image_width);
        ASSERTX(image_height > 0, image_height);

        width_     = image_width;
        height_    = image_height;
        has_alpha_ = alpha;

        components.resize(0);  // clear all pixels
        const unsigned int size = width_ * height_ * GetPixelByteSize();
        if(default_value < 0)
        {
            components.resize(size);
        }
        else
        {
            ASSERT(default_value <= 255);
            components.resize(size, static_cast<unsigned char>(default_value));
        }

        ASSERT(IsValid());
    }

    int
    Image::GetPixelIndex(int x, int y) const
    {
        ASSERT(x >= 0 && x < width_);
        ASSERT(y >= 0 && y < height_);

        return (y * width_ + x) * GetPixelByteSize();
    }

    void
    Image::SetPixel(int x, int y, const Rgbai& color)
    {
        SetPixel(x, y, color.r, color.g, color.b, color.a);
    }

    void
    Image::SetPixel(
            int           x,
            int           y,
            unsigned char r,
            unsigned char g,
            unsigned char b,
            unsigned char a)
    {
        ASSERTX(IsWithinInclusivei(0, x, GetWidth() - 1), x, GetWidth() - 1);
        ASSERTX(IsWithinInclusivei(0, y, GetHeight() - 1), y, GetHeight() - 1);

        const auto base_index      = GetPixelIndex(x, y);
        components[base_index + 0] = r;
        components[base_index + 1] = g;
        components[base_index + 2] = b;

        if(has_alpha_)
        {
            components[base_index + 3] = a;
        }
    }

    Rgbai
    Image::GetPixel(int x, int y) const
    {
        ASSERTX(IsWithinInclusivei(0, x, GetWidth() - 1), x, GetWidth());
        ASSERTX(IsWithinInclusivei(0, y, GetHeight() - 1), y, GetHeight());

        const auto base_index = GetPixelIndex(x, y);

        const auto red   = components[base_index + 0];
        const auto green = components[base_index + 1];
        const auto blue  = components[base_index + 2];

        if(has_alpha_)
        {
            const auto alpha = components[base_index + 3];
            return Rgbai {Rgbi {red, green, blue}, alpha};
        }
        else
        {
            return Rgbi {red, green, blue};
        }
    }

    bool
    Image::IsValid() const
    {
        return width_ > 0 && height_ > 0;
    }

    int
    Image::GetWidth() const
    {
        return width_;
    }

    int
    Image::GetHeight() const
    {
        return height_;
    }

    bool
    Image::HasAlpha() const
    {
        return has_alpha_;
    }

    const unsigned char*
    Image::GetPixelData() const
    {
        return &components[0];
    }

    namespace  // local
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
    }  // namespace

    int
    WriteImageData(
            stbi_write_func* func,
            void*            context,
            int              w,
            int              h,
            int              comp,
            const void*      data,
            ImageWriteFormat format,
            int              jpeg_quality)
    {
        switch(format)
        {
        case ImageWriteFormat::PNG:
            return stbi_write_png_to_func(func, context, w, h, comp, data, 0);
        case ImageWriteFormat::BMP:
            return stbi_write_bmp_to_func(func, context, w, h, comp, data);
        case ImageWriteFormat::TGA:
            return stbi_write_tga_to_func(func, context, w, h, comp, data);
        case ImageWriteFormat::JPEG:
            return stbi_write_jpg_to_func(
                    func, context, w, h, comp, data, jpeg_quality);
        default: DIE("Unhandled case"); return 0;
        }
    }

    std::shared_ptr<MemoryChunk>
    Image::Write(ImageWriteFormat format, int jpeg_quality) const
    {
        const int number_of_components = has_alpha_ ? 4 : 3;

        std::vector<unsigned char> pixels(
                width_ * height_ * number_of_components, 0);
        for(int y = 0; y < height_; y += 1)
        {
            const int iy = height_ - (y + 1);

            ASSERTX(IsWithinInclusivei(0, iy, height_ - 1), iy, y, height_);
            for(int x = 0; x < width_; x += 1)
            {
                const int target_index
                        = (x + width_ * y) * number_of_components;
                const int source_index
                        = (x + width_ * iy) * number_of_components;
                for(int component = 0; component < number_of_components;
                    component += 1)
                {
                    pixels[target_index + component]
                            = components[source_index + component];
                }
            }
        }

        int size        = 0;
        int size_result = WriteImageData(
                DetermineImageSize,
                &size,
                GetWidth(),
                GetHeight(),
                number_of_components,
                &pixels[0],
                format,
                jpeg_quality);
        if(size_result == 0)
        {
            return MemoryChunk::Null();
        }

        ASSERT(size > 0);
        MemoryChunkFile file {MemoryChunk::Alloc(size)};
        int             write_result = WriteImageData(
                WriteToMemoryChunkFile,
                &file,
                GetWidth(),
                GetHeight(),
                number_of_components,
                &pixels[0],
                format,
                jpeg_quality);
        if(write_result == 0)
        {
            return MemoryChunk::Null();
        }

        return file.data;
    }

    namespace
    {
        unsigned char
        Select(int           ch,
               unsigned char a,
               unsigned char b,
               unsigned char c,
               unsigned char d)
        {
            switch(ch)
            {
            case 1:  // grey
                return a;
            case 2:  // grey, alpha
                return b;
            case 3:  // red, green, blue
                return c;
            case 4:  // red, green, blue, alpha
                return d;
            default: DIE("unhandled Select channel"); return 0;
            }
        }
    }  // namespace

    ImageLoadResult
    LoadImage(vfs::FileSystem* fs, const std::string& path, AlphaLoad alpha)
    {
        auto file_memory = fs->ReadFile(path);
        if(file_memory == nullptr)
        {
            ImageLoadResult result;
            result.error = "File doesnt exist";
            result.image.MakeInvalid();
            LOG_ERROR("Failed to open " << path << ": File doesnt exist.");
            return result;
        }

        return LoadImage(file_memory, path, alpha);
    }

    ImageLoadResult
    LoadImage(
            std::shared_ptr<MemoryChunk> file_memory,
            const std::string&           path,
            AlphaLoad                    alpha)
    {
        int channels     = 0;
        int image_width  = 0;
        int image_height = 0;

        // signed to unsigned cast is probably ok since both are considered to be
        // a chuck of memory
        // https://stackoverflow.com/questions/310451/should-i-use-static-cast-or-reinterpret-cast-when-casting-a-void-to-whatever
        unsigned char* data = stbi_load_from_memory(
                reinterpret_cast<unsigned char*>(
                        file_memory->GetData()),  // NOLINT
                file_memory->GetSize(),
                &image_width,
                &image_height,
                &channels,
                0);

        if(data == nullptr)
        {
            ImageLoadResult result;
            result.error = stbi_failure_reason();
            result.image.MakeInvalid();
            LOG_ERROR("Failed to read " << path << ": " << result.error);
            return result;
        }

        bool has_alpha = false;
        if(alpha == AlphaLoad::Keep)
        {
            has_alpha = channels == 2 || channels == 4;
        }

        LOG_INFO(
                "Image: " << path << " " << image_width << "x" << image_height
                          << " alpha " << has_alpha << " channels "
                          << channels);

        ImageLoadResult result;
        if(has_alpha)
        {
            result.image.SetupWithAlphaSupport(image_width, image_height, -1);
        }
        else
        {
            result.image.SetupNoAlphaSupport(image_width, image_height, -1);
        }

        for(int y = 0; y < image_height; ++y)
        {
            for(int x = 0; x < image_width; ++x)
            {
                const int src_index = (y * image_width + x) * channels;

                // get component values
                const unsigned char zero = 0;
                const unsigned char c1
                        = data[src_index + 0];  // NOLINT no garbage value
                const unsigned char c2
                        = (channels <= 1) ? zero : data[src_index + 1];
                const unsigned char c3
                        = (channels <= 2) ? zero : data[src_index + 2];
                const unsigned char c4
                        = (channels <= 3) ? zero : data[src_index + 3];

                // Gray, Gray+alpha, RGB, RGB+alpha:     gr   gra  rgb  rgba
                const unsigned char r = c1;  //           c1   c1   c1    c1
                const unsigned char g = Select(channels, c1, c1, c2, c2);
                const unsigned char b = Select(channels, c1, c1, c3, c3);
                const unsigned char a = Select(channels, 255, c2, 255, c4);

                result.image.SetPixel(x, image_height - (y + 1), r, g, b, a);
            }
        }

        stbi_image_free(data);
        return result;
    }

    ImageLoadResult
    LoadImage(
            void* compressed_data,
            int compressed_size,
            const std::string&           path,
            AlphaLoad                    alpha)
    {
        Decompress dec;
        const unsigned int decompressed_size = Decompress::stb_decompress_length((const unsigned char*)compressed_data);
        auto decompressed = MemoryChunk::Alloc(decompressed_size);
        const auto len = dec.stb_decompress(reinterpret_cast<unsigned char*>(decompressed->GetData()),
            reinterpret_cast<const unsigned char*>(compressed_data),
            static_cast<unsigned int>(compressed_size));
        if(len == 0)
        {
            ImageLoadResult result;
            result.error = "failed to decompress before loading image";
            result.image.MakeInvalid();
            return result;
        }

        return LoadImage(decompressed, path, alpha);
    }

}  // namespace euphoria::core
