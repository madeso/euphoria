
#include <iostream>

#include "core/image.h"
#include "assert/assert.h"
#include "log/log.h"
#include "core/vfs_path.h"

#include "render/gl.h"
#include "render/texture.h"

#include "undef_windows/undef_windows.h"


namespace
{
    eu::render::gl::Int
    con(eu::render::TextureWrap v)
    {
        switch(v)
        {
        case eu::render::TextureWrap::repeat: return GL_REPEAT;
        case eu::render::TextureWrap::mirrored_repeat: return GL_MIRRORED_REPEAT;
        case eu::render::TextureWrap::clamp_to_edge: return GL_CLAMP_TO_EDGE;
        }

        DIE("Unhandled texture wrap value");
        return GL_REPEAT;
    }


    eu::render::gl::Int
    con(eu::render::FilterMagnification v)
    {
        switch(v)
        {
        case eu::render::FilterMagnification::nearest: return GL_NEAREST;
        case eu::render::FilterMagnification::linear: return GL_LINEAR;
        }


        DIE("Unhandled filter magnification value");
        return GL_LINEAR;
    }


    eu::render::gl::Int
    con(eu::render::FilterMinification v)
    {
        switch(v)
        {
        case eu::render::FilterMinification::nearest: return GL_NEAREST;
        case eu::render::FilterMinification::linear: return GL_LINEAR;
        }

        DIE("Unhandled filter minification value");
        return GL_LINEAR;
    }


    const eu::render::TextureId*&
    get_bound_texture()
    {
        static const eu::render::TextureId* bound_texture = nullptr;
        return bound_texture;
    }
}



namespace eu::render
{
    Texture2dLoadData::Texture2dLoadData()
        : wrap(TextureWrap::repeat)
        , minification(FilterMinification::linear)
        , magnification(FilterMagnification::linear)
    {
    }


    Texture2dLoadData&
    Texture2dLoadData::set_wrap(TextureWrap v)
    {
        wrap = v;
        return *this;
    }


    Texture2dLoadData&
    Texture2dLoadData::set_filter_magnification(FilterMagnification v)
    {
        magnification = v;
        return *this;
    }


    Texture2dLoadData&
    Texture2dLoadData::set_filter_minification(FilterMinification v)
    {
        minification = v;
        return *this;
    }


    TextureId::TextureId() : id(0)
    {
        glGenTextures(1, &id);
    }


    TextureId::~TextureId()
    {
        glDeleteTextures(1, &id);
    }


    GLuint
    TextureId::get_id() const
    {
        return id;
    }


    bool
    TextureId::is_currently_bound() const
    {
        return this == get_bound_texture();
    }


    void
    use(const TextureId* texture)
    {
        if(texture != nullptr)
        {
            glBindTexture(GL_TEXTURE_2D, texture->get_id());
        }
        get_bound_texture() = texture;
    }


    Texture2::Texture2()
        : width(0)
        , height(0)
    {
    }


    void
    Texture2::load_from_pixels
    (
        int new_width,
        int new_height,
        const unsigned char* pixel_data,
        GLint internal_format,
        GLuint image_format,
        const Texture2dLoadData& data
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, con(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, con(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, con(data.minification));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, con(data.magnification));

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    void
    Texture2::load_from_file
    (
        core::vfs::FileSystem* fs,
        const core::vfs::FilePath& path,
        core::AlphaLoad alpha,
        const Texture2dLoadData& data
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
    Texture2::load_from_image
    (
        const core::Image& image,
        core::AlphaLoad alpha,
        const Texture2dLoadData& data
    )
    {
        GLint internal_format = GL_RGB;
        GLuint image_format = GL_RGB;
        if(image.has_alpha && alpha == core::AlphaLoad::keep)
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
