#ifndef SPACETYPER_SPRITE_H
#define SPACETYPER_SPRITE_H

#include <vector>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"

namespace euphoria::render
{
    class Texture2d;
    class SpriteRenderer;

    class Sprite
    {
    public:
        // todo: remove getters and setters
        explicit Sprite(
                std::shared_ptr<Texture2d> texture,
                const core::vec2f&         position = core::vec2f::Zero());

        std::shared_ptr<Texture2d>
        GetTexture();

        const core::vec2f&
        GetPosition() const;
        void
        SetPosition(const core::vec2f& p);

        float
        GetHeight() const;
        float
        GetWidth() const;

        void
        SetAlpha(float a);

        void
        Render(SpriteRenderer* render);

    private:
        std::shared_ptr<Texture2d> texture_;
        core::vec2f                position_;

    public:
        core::Angle rotation;

    private:
        core::scale2f scale_;
        core::Rgb     color_;
        float         alpha_;
    };

    class Layer
    {
    public:
        explicit Layer(SpriteRenderer* render);

        void
        Add(Sprite* sprite);

        void
        Remove(Sprite* sprite);

        void
        Render();

    private:
        SpriteRenderer*                                          render_;
        typedef std::vector<Sprite*>                             SpriteList;
        typedef std::map<std::shared_ptr<Texture2d>, SpriteList> SpriteMap;
        SpriteMap                                                sprites_;
    };

}  // namespace euphoria::render

#endif  // SPACETYPER_SPRITE_H
