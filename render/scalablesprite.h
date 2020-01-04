#ifndef SPACETYPER_SCALABLESPRITE_H
#define SPACETYPER_SCALABLESPRITE_H

#include <memory>
#include <vector>
#include "core/rect.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }
    struct Rgba;
}  // namespace euphoria::core

namespace euphoria::render
{
    struct Buffer2d;
    struct Texture2d;
    struct TextureId;
    struct TextureCache;
    struct SpriteRenderer;

    struct ScalableSprite
    {
    public:
        ScalableSprite
        (
            core::vfs::FileSystem* fs,
            const core::vfs::FilePath& path,
            TextureCache* cache
        );

        ~ScalableSprite();

        const core::Sizef
        GetMinimumSize() const;

        void
        Render
        (
            SpriteRenderer* sr,
            const core::Rectf& pos,
            const core::Rgba& tint
        ) const;

    private:
        std::shared_ptr<Texture2d> texture_;

        std::vector<float> cols_;
        std::vector<float> rows_;

        float max_col_;
        float max_row_;
    };

}  // namespace euphoria::render

#endif  // SPACETYPER_SCALABLESPRITE_H
