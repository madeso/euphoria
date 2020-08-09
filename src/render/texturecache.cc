#include "render/texturecache.h"

#include "core/assert.h"
#include "core/cache.h"
#include "core/vfs_path.h"

#include "render/texture.h"

namespace euphoria::render
{
    struct TextureCache::TextureCachePimpl
        : core::Cache<core::vfs::FilePath, Texture2d, TextureCache::TextureCachePimpl>
    {
    public:
        explicit TextureCachePimpl(core::vfs::FileSystem* fs) : fs_(fs)
        {
            ASSERT(fs);
        }

        std::shared_ptr<Texture2d>
        Create(const core::vfs::FilePath& file)
        {
            auto ret = std::make_shared<Texture2d>();
            ret->LoadFromFile
            (
                fs_,
                file,
                core::AlphaLoad::Keep,
                Texture2dLoadData{}
            );
            return ret;
        }

    private:
        core::vfs::FileSystem* fs_;
    };


    TextureCache::TextureCache(core::vfs::FileSystem* fs)
    {
        pimp_ = std::make_unique<TextureCache::TextureCachePimpl>(fs);
    }


    TextureCache::~TextureCache() = default;


    std::shared_ptr<Texture2d>
    TextureCache::GetTexture(const core::vfs::FilePath& path)
    {
        return pimp_->Get(path);
    }


    std::shared_ptr<Texture2d>
    TextureCache::GetTexture(const std::optional<core::vfs::FilePath>& path)
    {
        if(path.has_value())
        {
            return GetTexture(path.value());
        }
        else
        {
            return nullptr;
        }
    }

}  // namespace euphoria::render
