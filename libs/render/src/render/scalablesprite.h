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
    struct buffer2d;
    struct texture2d;
    struct texture_id;
    struct texture_cache;
    struct sprite_renderer;

    struct scalable_sprite
    {
        scalable_sprite
        (
                core::vfs::FileSystem* fs,
                const core::vfs::FilePath& path,
                texture_cache* cache
        );
        ~scalable_sprite();

        NONCOPYABLE(scalable_sprite);

        [[nodiscard]] core::size2f
        get_minimum_size() const;

        void
        render(sprite_renderer* renderer, const core::Rectf& rect, const core::Rgba& tint) const;

        std::shared_ptr<texture2d> texture;

        std::vector<float> columns;
        std::vector<float> rows;

        float calculated_texture_size_columns;
        float calculated_texture_size_rows;
    };
}
