#pragma once

#include "core/image.h"
#include "render/gltypes.h"
#include "core/vfs.h"


namespace eu::core
{
    struct Image;
}

namespace eu::render
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

        TextureId(const TextureId& other) = delete;
        void operator=(const TextureId&) = delete;
        TextureId(TextureId&& other) = delete;
        void operator=(TextureId&&) = delete;

        [[nodiscard]] bool
        is_currently_bound() const;

        [[nodiscard]] gl::Uint
        get_id() const;

    private:
        gl::Uint id;
    };

    void
    use(const TextureId* texture);

    struct Texture2 : public TextureId
    {
    public:
        Texture2();
        ~Texture2() = default;

        Texture2(const Texture2& other) = delete;
        void operator=(const Texture2&) = delete;
        Texture2(Texture2&& other) = delete;
        void operator=(Texture2&&) = delete;

        void
        load_from_pixels
        (
            int width,
            int height,
            const unsigned char* pixel_data,
            gl::Int internal_format,
            gl::Uint image_format,
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
