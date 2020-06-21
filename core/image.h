#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include <string>
#include <vector>
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
        PNG,
        BMP,
        TGA,
        JPEG
    };

    struct Image
    {
        void
        MakeInvalid();

        bool
        IsValid() const;

        // todo(Gustav): Add create function that will create empty image and run setup

        // if default value is negative, default value is ignored, otherwise its the
        // default value for both R, G, B, and A.
        void
        SetupWithAlphaSupport(
                int image_width,
                int image_height,
                int default_value = 0);
        void
        SetupNoAlphaSupport(
                int image_width,
                int image_height,
                int default_value = 0);

        void
        SetPixel(int x, int y, const Rgbai& color);

        void
        SetPixel(
                int           x,
                int           y,
                unsigned char r,
                unsigned char g,
                unsigned char b,
                unsigned char a);

        Rgbai
        GetPixel(int x, int y) const;

        template <typename Func>
        void
        Filter(Func f)
        {
            for(int y = 0; y < GetHeight(); y += 1)
            for(int x = 0; x < GetWidth(); x += 1)
            {
                SetPixel(x, y, f(GetPixel(x, y)));
            }
        }

        template <typename Func>
        void
        ForAllTopBottom(Func f)
        {
            for(int y = GetHeight(); y > 0; y -= 1)
            for(int x = 0; x < GetWidth(); x += 1)
            {
                f(x, y - 1, GetPixel(x, y - 1));
            }
        }

        template <typename Func>
        void
        SetAllTopBottom(Func f)
        {
            for(int y = GetHeight(); y > 0; y -= 1)
            for(int x = 0; x < GetWidth(); x += 1)
            {
                SetPixel(x, y - 1, f(x, y - 1));
            }
        }

        template <typename Func>
        void
        SetAllBottomTop(Func f)
        {
            for(int y = 0; y < GetHeight(); y += 1)
            for(int x = 0; x < GetWidth(); x += 1)
            {
                SetPixel(x, y, f(x, y));
            }
        }

        Recti
        GetIndices() const;

        int
        GetWidth() const;

        int
        GetHeight() const;

        bool
        HasAlpha() const;

        const unsigned char*
        GetPixelData() const;

        std::shared_ptr<MemoryChunk>
        Write(ImageWriteFormat format, int jpeg_quality = 100) const;

    private:
        void
        Setup(int image_width, int image_height, bool alpha, int default_value);

        int
        GetPixelByteSize() const;

        int
        GetPixelIndex(int x, int y) const;

        // todo: replace with a array instead of a vector
        std::vector<unsigned char> components;
        int                        width_;
        int                        height_;
        bool                       has_alpha_;
    };

    struct ImageLoadResult
    {
        Image       image;
        std::string error;
    };

    enum class AlphaLoad
    {
        Remove,
        Keep
    };

    // todo: move image loading to a io library instead
    ImageLoadResult
    LoadImage(vfs::FileSystem* fs, const vfs::FilePath& path, AlphaLoad alpha);

    ImageLoadResult
    LoadImage(
            std::shared_ptr<MemoryChunk> memory,
            const std::string&           path,
            AlphaLoad                    alpha);

    ImageLoadResult
    LoadImage(
            void* compressed_data,
            int compressed_size,
            const std::string&           path,
            AlphaLoad                    alpha);

}  // namespace euphoria::core

#endif  // CORE_IMAGE_H
