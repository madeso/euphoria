#pragma once

#include <vector>
#include <map>
#include <memory>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"

namespace euphoria::render
{
    struct Texture2d;
    struct SpriteRenderer;

    struct Sprite
    {
        // todo(Gustav): remove getters and setters
        explicit Sprite(
                std::shared_ptr<Texture2d> texture,
                const core::vec2f& position = core::vec2f::Zero());

        [[nodiscard]] std::shared_ptr<Texture2d>
        GetTexture() const;

        [[nodiscard]] const core::vec2f&
        GetPosition() const;

        void
        SetPosition(const core::vec2f& p);

        [[nodiscard]] float
        GetHeight() const;

        [[nodiscard]] float
        GetWidth() const;

        void
        SetAlpha(float a);

        void
        Render(SpriteRenderer* render);

        std::shared_ptr<Texture2d> texture_;
        core::vec2f position_;
        core::Angle rotation;
        core::scale2f scale_;
        core::Rgb color_;
        float alpha_;
    };


    struct Layer
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
        using SpriteList = std::vector<Sprite*>;
        using SpriteMap = std::map<std::shared_ptr<Texture2d>, SpriteList>;

        SpriteRenderer* render_;
        SpriteMap sprites_;
    };
}
