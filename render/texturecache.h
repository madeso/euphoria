#ifndef SPACETYPER_TEXTURECACHE_H
#define SPACETYPER_TEXTURECACHE_H

#include <memory>
#include <string>

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
    }
}  // namespace euphoria::core

namespace euphoria::render
{
    struct Texture2d;

    struct TextureCache
    {
    public:
        explicit TextureCache(core::vfs::FileSystem* fs);
        ~TextureCache();

        std::shared_ptr<Texture2d>
        GetTexture(const std::string& path);

        std::shared_ptr<Texture2d>
        GetTextureIfNotEmpty(const std::string& path);

    private:
        struct TextureCachePimpl;
        std::unique_ptr<TextureCachePimpl> pimp_;
    };

}  // namespace euphoria::render

#endif  // SPACETYPER_TEXTURECACHE_H
