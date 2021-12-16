#pragma once

#include <memory>
#include <vector>

#include "core/rect.h"
#include "core/noncopyable.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::core
{
    struct Rgba;
}

namespace euphoria::render
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

        NONCOPYABLE(ScalableSprite);

        [[nodiscard]] core::Size2f
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
