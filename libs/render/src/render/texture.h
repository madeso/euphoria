#pragma once

#include "core/image.h"
#include "render/gltypes.h"
#include "core/noncopyable.h"
#include "core/vfs.h"

namespace euphoria::core
{
    struct image;
}

namespace euphoria::render
{
    enum class texture_wrap
    {
        repeat,
        mirrored_repeat,
        clamp_to_edge
    };

    enum class filter_magnification
    {
        nearest,
        linear
    };

    enum class filter_minification
    {
        nearest,
        linear
        // todo(Gustav): add mipmap
    };

    struct texture2d_load_data
    {
        texture2d_load_data();

        texture2d_load_data&
        set_wrap(texture_wrap v);

        texture2d_load_data&
        set_filter_magnification(filter_magnification v);

        texture2d_load_data&
        set_filter_minification(filter_minification v);

        texture_wrap wrap;
        filter_minification minification;
        filter_magnification magnification;
    };

    struct texture_id
    {
    public:
        texture_id();
        ~texture_id();

        NONCOPYABLE(texture_id);

        [[nodiscard]] bool
        is_currently_bound() const;

        [[nodiscard]] gluint
        get_id() const;

    private:
        gluint id;
    };

    void
    use(const texture_id* texture);

    struct texture2d : public texture_id
    {
    public:
        texture2d();
        ~texture2d() = default;

        NONCOPYABLE(texture2d);

        void
        load_from_pixels
        (
            int width,
            int height,
            const unsigned char* pixel_data,
            glint internal_format,
            gluint image_format,
            const texture2d_load_data& data
        );

        void
        load_from_image
        (
            const core::image& image,
            core::alpha_load alpha,
            const texture2d_load_data& data
        );

        void
        load_from_file
        (
            core::vfs::file_system* fs,
            const core::vfs::file_path& path,
            core::alpha_load alpha,
            const texture2d_load_data& data
        );

        int width;
        int height;
    };

}
