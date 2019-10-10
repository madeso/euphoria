#ifndef TEXTURE_H
#define TEXTURE_H

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
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE
    };

    enum class FilterMagnification
    {
        NEAREST,
        LINEAR
    };

    enum class FilterMinification
    {
        NEAREST,
        LINEAR
        // todo: add mipmap
    };

    struct Texture2dLoadData
    {
        Texture2dLoadData();

        Texture2dLoadData&
        SetWrap(TextureWrap v);
        Texture2dLoadData&
        SetFilterMag(FilterMagnification v);
        Texture2dLoadData&
        SetFilterMin(FilterMinification v);

        TextureWrap         wrap;
        FilterMinification  min;
        FilterMagnification mag;
    };

    struct TextureId : core::Noncopyable
    {
    public:
        TextureId();
        ~TextureId();

        bool
        IsCurrentlyBound() const;
        gluint
        GetId() const;

    private:
        gluint id_;
    };

    void
    Use(const TextureId* texture);

    struct Texture2d : public TextureId
    {
    public:
        Texture2d();

        void
        LoadFromPixels(
                int                      width,
                int                      height,
                const unsigned char*     pixel_data,
                gluint                   internal_format,
                gluint                   image_format,
                const Texture2dLoadData& data);
        void
        LoadFromImage(
                const core::Image&       image,
                core::AlphaLoad          alpha,
                const Texture2dLoadData& data);
        void
        LoadFromFile(
                core::vfs::FileSystem*   fs,
                const std::string&       path,
                core::AlphaLoad          alpha,
                const Texture2dLoadData& data);

        int
        GetWidth() const;
        int
        GetHeight() const;

    private:
        int width_, height_;
    };

}  // namespace euphoria::render

#endif
