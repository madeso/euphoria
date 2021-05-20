#pragma once

#include <memory>
#include <vector>

#include "core/rect.h"
#include "core/noncopyable.h"


namespace euphoria::core::vfs
{
    struct file_system;
    struct file_path;
}

namespace euphoria::core
{
    struct rgba;
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
                core::vfs::file_system* fs,
                const core::vfs::file_path& path,
                texture_cache* cache
        );
        ~scalable_sprite();

        NONCOPYABLE(scalable_sprite);

        [[nodiscard]] core::Sizef
        get_minimum_size() const;

        void
        render(sprite_renderer* renderer, const core::rectf& rect, const core::rgba& tint) const;

        std::shared_ptr<texture2d> texture;

        std::vector<float> columns;
        std::vector<float> rows;

        float calculated_texture_size_columns;
        float calculated_texture_size_rows;
    };
}
