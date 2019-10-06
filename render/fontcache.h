#ifndef RENDER_FONTCACHE_H
#define RENDER_FONTCACHE_H

#include <memory>
#include <string>

namespace euphoria
{
    namespace core
    {
        namespace vfs
        {
            class FileSystem;
        }
    }  // namespace core

    namespace render
    {
        class Font;
        class TextureCache;

        class FontCache
        {
            public:
            FontCache(core::vfs::FileSystem* fs, TextureCache* cache);
            ~FontCache();

            std::shared_ptr<Font>
            GetFont(const std::string& path);

            private:
            struct FontCachePimpl;
            std::unique_ptr<FontCachePimpl> pimp;
        };
    }  // namespace render
}  // namespace euphoria

#endif  // RENDER_FONTCACHE_H
