
#include <iostream>

#include "core/image.h"
#include "core/assert.h"
#include "core/log.h"
#include "core/vfs_path.h"

#include "render/gl.h"
#include "render/texture.h"

#include "core/undef_windows.h"


namespace // local
{
    euphoria::render::gluint
    C(euphoria::render::texture_wrap v)
    {
        switch(v)
        {
        case euphoria::render::texture_wrap::repeat: return GL_REPEAT;
        case euphoria::render::texture_wrap::mirrored_repeat: return GL_MIRRORED_REPEAT;
        case euphoria::render::texture_wrap::clamp_to_edge: return GL_CLAMP_TO_EDGE;
        }

        DIE("Unhandled texture wrap value");
        return GL_REPEAT;
    }


    euphoria::render::gluint
    C(euphoria::render::filter_magnification v)
    {
        switch(v)
        {
        case euphoria::render::filter_magnification::nearest: return GL_NEAREST;
        case euphoria::render::filter_magnification::linear: return GL_LINEAR;
        }


        DIE("Unhandled filter magnification value");
        return GL_LINEAR;
    }


    euphoria::render::gluint
    C(euphoria::render::filter_minification v)
    {
        switch(v)
        {
        case euphoria::render::filter_minification::nearest: return GL_NEAREST;
        case euphoria::render::filter_minification::linear: return GL_LINEAR;
        }

        DIE("Unhandled filter minification value");
        return GL_LINEAR;
    }


    const euphoria::render::texture_id*&
    get_bound_texture()
    {
        static const euphoria::render::texture_id* bound_texture = nullptr;
        return bound_texture;
    }
}



namespace euphoria::render
{
    texture2d_load_data::texture2d_load_data()
        : wrap(texture_wrap::repeat)
        , minification(filter_minification::linear)
        , magnification(filter_magnification::linear)
    {
    }


    texture2d_load_data&
    texture2d_load_data::set_wrap(texture_wrap v)
    {
        wrap = v;
        return *this;
    }


    texture2d_load_data&
    texture2d_load_data::set_filter_magnification(filter_magnification v)
    {
        magnification = v;
        return *this;
    }


    texture2d_load_data&
    texture2d_load_data::set_filter_minification(filter_minification v)
    {
        minification = v;
        return *this;
    }


    texture_id::texture_id() : id_(0)
    {
        glGenTextures(1, &id_);
    }


    texture_id::~texture_id()
    {
        glDeleteTextures(1, &id_);
    }


    GLuint
    texture_id::get_id() const
    {
        return id_;
    }


    bool
    texture_id::is_currently_bound() const
    {
        return this == get_bound_texture();
    }


    void
    use(const texture_id* texture)
    {
        if(texture != nullptr)
        {
            glBindTexture(GL_TEXTURE_2D, texture->get_id());
        }
        get_bound_texture() = texture;
    }


    texture2d::texture2d()
        : width(0)
        , height(0)
    {
    }


    void
    texture2d::load_from_pixels
    (
        int new_width,
        int new_height,
        const unsigned char* pixel_data,
        GLuint internal_format,
        GLuint image_format,
        const texture2d_load_data& data
    )
    {
        use(this);

        width = new_width;
        height = new_height;

        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            internal_format,
            width,
            height,
            0,
            image_format,
            GL_UNSIGNED_BYTE,
            pixel_data
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, C(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, C(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, C(data.minification));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, C(data.magnification));

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    void
    texture2d::load_from_file
    (
        core::vfs::file_system* fs,
        const core::vfs::file_path& path,
        core::alpha_load alpha,
        const texture2d_load_data& data
    )
    {
        const auto loaded = core::load_image(fs, path, alpha);
        if(!loaded.image.is_valid())
        {
            LOG_ERROR("Failed to load image {0}: {1}", path, loaded.error);
            return;
        }
        load_from_image(loaded.image, alpha, data);
    }


    void
    texture2d::load_from_image
    (
        const core::image& image,
        core::alpha_load alpha,
        const texture2d_load_data& data
    )
    {
        GLuint internal_format = GL_RGB;
        GLuint image_format = GL_RGB;
        if(image.has_alpha && alpha == core::alpha_load::Keep)
        {
            internal_format = GL_RGBA;
            image_format = GL_RGBA;
        }

        load_from_pixels
        (
            image.width,
            image.height,
            image.get_pixel_data(),
            internal_format,
            image_format,
            data
        );
    }
}
