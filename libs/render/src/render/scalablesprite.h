#pragma once

#include <memory>

#include "core/rect.h"


namespace eu::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace eu::core
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
                core::vfs::FileSystem* fs,
                const core::vfs::FilePath& path,
                TextureCache* cache
        );
        ~ScalableSprite();

        ScalableSprite(const ScalableSprite& other) = delete;
        void operator=(const ScalableSprite&) = delete;
        ScalableSprite(ScalableSprite&& other) = delete;
        void operator=(ScalableSprite&&) = delete;

        [[nodiscard]] core::size2f
        get_minimum_size() const;

        void
        render(SpriteRenderer* renderer, const core::Rectf& rect, const core::Rgba& tint) const;

        std::shared_ptr<Texture2> texture;

        std::vector<float> columns;
        std::vector<float> rows;

        float calculated_texture_size_columns;
        float calculated_texture_size_rows;
    };
}
