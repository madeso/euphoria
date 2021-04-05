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
    struct Texture2d;
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

        [[nodiscard]] core::Sizef
        GetMinimumSize() const;

        void
        Render(SpriteRenderer* sr, const core::Rectf& pos, const core::Rgba& tint ) const;

        std::shared_ptr<Texture2d> texture_;

        std::vector<float> cols_;
        std::vector<float> rows_;

        float max_col_;
        float max_row_;
    };
}
