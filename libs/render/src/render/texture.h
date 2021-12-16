#pragma once

#include "core/image.h"
#include "render/gltypes.h"
#include "core/noncopyable.h"
#include "core/vfs.h"

namespace euphoria::core
{
    struct Image;
}

namespace euphoria::render
{
    enum class TextureWrap
    {
        repeat,
        mirrored_repeat,
        clamp_to_edge
    };

    enum class FilterMagnification
    {
        nearest,
        linear
    };

    enum class FilterMinification
    {
        nearest,
        linear
        // todo(Gustav): add mipmap
    };

    struct Texture2dLoadData
    {
        Texture2dLoadData();

        Texture2dLoadData&
        set_wrap(TextureWrap v);

        Texture2dLoadData&
        set_filter_magnification(FilterMagnification v);

        Texture2dLoadData&
        set_filter_minification(FilterMinification v);

        TextureWrap wrap;
        FilterMinification minification;
        FilterMagnification magnification;
    };

    struct TextureId
    {
    public:
        TextureId();
        ~TextureId();

        NONCOPYABLE(TextureId);

        [[nodiscard]] bool
        is_currently_bound() const;

        [[nodiscard]] OpenglUint
        get_id() const;

    private:
        OpenglUint id;
    };

    void
    use(const TextureId* texture);

    struct Texture2 : public TextureId
    {
    public:
        Texture2();
        ~Texture2() = default;

        NONCOPYABLE(Texture2);

        void
        load_from_pixels
        (
            int width,
            int height,
            const unsigned char* pixel_data,
            OpenglInt internal_format,
            OpenglUint image_format,
            const Texture2dLoadData& data
        );

        void
        load_from_image
        (
            const core::Image& image,
            core::AlphaLoad alpha,
            const Texture2dLoadData& data
        );

        void
        load_from_file
        (
            core::vfs::FileSystem* fs,
            const core::vfs::FilePath& path,
            core::AlphaLoad alpha,
            const Texture2dLoadData& data
        );

        int width;
        int height;
    };

}
