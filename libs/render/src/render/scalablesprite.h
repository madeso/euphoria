#pragma once

#include <memory>

#include "base/rect.h"


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}

namespace eu
{
    struct Rgba;
}

namespace eu::render
{
    struct Buffer2d;
    struct Texture2;
    struct TextureId;
    struct TextureCache;
    struct SpriteRenderer;

    struct ScalableSprite
    {
        ScalableSprite
        (
                io::FileSystem* fs,
                const io::FilePath& path,
                TextureCache* cache
        );
        ~ScalableSprite();

        ScalableSprite(const ScalableSprite& other) = delete;
        void operator=(const ScalableSprite&) = delete;
        ScalableSprite(ScalableSprite&& other) = delete;
        void operator=(ScalableSprite&&) = delete;

        [[nodiscard]] size2f
        get_minimum_size() const;

        void
        render(SpriteRenderer* renderer, const Rectf& rect, const Rgba& tint) const;

        std::shared_ptr<Texture2> texture;

        std::vector<float> columns;
        std::vector<float> rows;

        float calculated_texture_size_columns;
        float calculated_texture_size_rows;
    };
}
