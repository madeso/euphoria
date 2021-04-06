
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
    C(euphoria::render::TextureWrap v)
    {
        switch(v)
        {
        case euphoria::render::TextureWrap::REPEAT: return GL_REPEAT;
        case euphoria::render::TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case euphoria::render::TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        }

        DIE("Unhandled texture wrap value");
        return GL_REPEAT;
    }


    euphoria::render::gluint
    C(euphoria::render::FilterMagnification v)
    {
        switch(v)
        {
        case euphoria::render::FilterMagnification::NEAREST: return GL_NEAREST;
        case euphoria::render::FilterMagnification::LINEAR: return GL_LINEAR;
        }


        DIE("Unhandled filter magnification value");
        return GL_LINEAR;
    }


    euphoria::render::gluint
    C(euphoria::render::FilterMinification v)
    {
        switch(v)
        {
        case euphoria::render::FilterMinification::NEAREST: return GL_NEAREST;
        case euphoria::render::FilterMinification::LINEAR: return GL_LINEAR;
        }

        DIE("Unhandled filter minification value");
        return GL_LINEAR;
    }


    const euphoria::render::TextureId*&
    GetCurrentShader()
    {
        static const euphoria::render::TextureId* current_shader = nullptr;
        return current_shader;
    }
}



namespace euphoria::render
{
    Texture2dLoadData::Texture2dLoadData()
        : wrap(TextureWrap::REPEAT)
        , min(FilterMinification::LINEAR)
        , mag(FilterMagnification::LINEAR)
    {
    }


    Texture2dLoadData&
    Texture2dLoadData::SetWrap(TextureWrap v)
    {
        wrap = v;
        return *this;
    }


    Texture2dLoadData&
    Texture2dLoadData::SetFilterMag(FilterMagnification v)
    {
        mag = v;
        return *this;
    }


    Texture2dLoadData&
    Texture2dLoadData::SetFilterMin(FilterMinification v)
    {
        min = v;
        return *this;
    }


    TextureId::TextureId() : id_(0)
    {
        glGenTextures(1, &id_);
    }


    TextureId::~TextureId()
    {
        glDeleteTextures(1, &id_);
    }


    GLuint
    TextureId::GetId() const
    {
        return id_;
    }


    bool
    TextureId::IsCurrentlyBound() const
    {
        return this == GetCurrentShader();
    }


    void
    Use(const TextureId* texture)
    {
        if(texture != nullptr)
        {
            glBindTexture(GL_TEXTURE_2D, texture->GetId());
        }
        GetCurrentShader() = texture;
    }


    Texture2d::Texture2d()
        : width_(0)
        , height_(0)
    {
    }


    void
    Texture2d::LoadFromPixels
    (
        int width,
        int height,
        const unsigned char* pixel_data,
        GLuint internal_format,
        GLuint image_format,
        const Texture2dLoadData& data
    )
    {
        Use(this);
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

        width_ = width;
        height_ = height;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, C(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, C(data.wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, C(data.min));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, C(data.mag));

        glBindTexture(GL_TEXTURE_2D, 0);
    }


    void
    Texture2d::LoadFromFile
    (
        core::vfs::FileSystem* fs,
        const core::vfs::FilePath& path,
        core::AlphaLoad alpha,
        const Texture2dLoadData& data
    )
    {
        auto i = core::LoadImage(fs, path, alpha);
        if(!i.image.IsValid())
        {
            LOG_ERROR("Failed to load image {0}: {1}", path, i.error);
            return;
        }
        LoadFromImage(i.image, alpha, data);
    }


    void
    Texture2d::LoadFromImage
    (
        const core::Image& image,
        core::AlphaLoad alpha,
        const Texture2dLoadData& data
    )
    {
        GLuint internal_format = GL_RGB;
        GLuint image_format = GL_RGB;
        if(image.HasAlpha() && alpha == core::AlphaLoad::Keep)
        {
            internal_format = GL_RGBA;
            image_format = GL_RGBA;
        }

        LoadFromPixels
        (
            image.GetWidth(),
            image.GetHeight(),
            image.GetPixelData(),
            internal_format,
            image_format,
            data
        );
    }


    int
    Texture2d::GetWidth() const
    {
        return width_;
    }


    int
    Texture2d::GetHeight() const
    {
        return height_;
    }
}
